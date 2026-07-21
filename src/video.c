#include "video.h"

#include <debug.h>
#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <packet.h>
#include <stdarg.h>
#include <stdio.h>

extern void SyncDCache(void *start, void *end);
extern const unsigned char msx[];

enum {
    VIDEO_WIDTH = 640,
    VIDEO_HEIGHT = 448,
    FRAMEBUFFER_COUNT = 2,
    DRAW_PACKET_QWORDS = 512,
    TEXTURE_UPLOAD_PACKET_QWORDS = 64,
    TEST_TEXTURE_WIDTH = 32,
    TEST_TEXTURE_HEIGHT = 32,
    TEST_TEXTURE_STORAGE_WIDTH = 64,
    TEST_TEXTURE_PIXEL_COUNT =
        TEST_TEXTURE_STORAGE_WIDTH * TEST_TEXTURE_HEIGHT,
    FONT_FIRST_CHARACTER = 32,
    FONT_CHARACTER_COUNT = 96,
    FONT_GLYPH_WIDTH = 8,
    FONT_GLYPH_HEIGHT = 8,
    TEXT_FORMAT_BUFFER_SIZE = 1024,
    TEXT_PACKET_RESERVED_QWORDS = 8
};

static framebuffer_t framebuffers[FRAMEBUFFER_COUNT];
static zbuffer_t zbuffer;
static texbuffer_t test_texture;
static packet_t *draw_packet;
static int display_buffer_index;
static int draw_buffer_index;
static unsigned int test_texture_pixels[TEST_TEXTURE_PIXEL_COUNT]
    __attribute__((aligned(64)));

static unsigned int rgba(unsigned char red, unsigned char green,
                         unsigned char blue)
{
    return (unsigned int)red | ((unsigned int)green << 8) |
           ((unsigned int)blue << 16) | (0x80u << 24);
}

static void generate_test_texture(void)
{
    int x;
    int y;

    for (y = 0; y < TEST_TEXTURE_HEIGHT; ++y) {
        for (x = 0; x < TEST_TEXTURE_STORAGE_WIDTH; ++x) {
            unsigned int pixel;

            if (x >= TEST_TEXTURE_WIDTH) {
                test_texture_pixels[y * TEST_TEXTURE_STORAGE_WIDTH + x] = 0;
                continue;
            }

            int checker = ((x / 4) + (y / 4)) & 1;
            bool border = x < 2 || x >= TEST_TEXTURE_WIDTH - 2 || y < 2 ||
                          y >= TEST_TEXTURE_HEIGHT - 2;
            bool emblem = (x >= 8 && x <= 10) || (x >= 21 && x <= 23) ||
                           (x >= 9 && x <= 22 && y >= x - 1 && y <= x + 1);

            pixel = checker ? rgba(0x10, 0x50, 0x78) :
                              rgba(0x08, 0x18, 0x30);
            if (border || emblem) {
                pixel = rgba(0xe8, 0xe0, 0x90);
            }

            /* Distinct corners make texture orientation easy to verify. */
            if (x < 6 && y < 6) {
                pixel = rgba(0xe0, 0x30, 0x30);
            } else if (x >= 26 && y < 6) {
                pixel = rgba(0x30, 0xd0, 0x50);
            } else if (x < 6 && y >= 26) {
                pixel = rgba(0x30, 0x60, 0xe0);
            } else if (x >= 26 && y >= 26) {
                pixel = rgba(0xe0, 0xc0, 0x30);
            }

            test_texture_pixels[y * TEST_TEXTURE_STORAGE_WIDTH + x] = pixel;
        }
    }

    SyncDCache(test_texture_pixels,
               test_texture_pixels + TEST_TEXTURE_PIXEL_COUNT);
}

static bool ranges_overlap(unsigned int first_address, unsigned int first_size,
                           unsigned int second_address,
                           unsigned int second_size)
{
    return first_address < second_address + second_size &&
           second_address < first_address + first_size;
}

static bool test_texture_layout_is_valid(void)
{
    unsigned int framebuffer_size =
        graph_vram_size(VIDEO_WIDTH, VIDEO_HEIGHT, GS_PSM_32,
                        GRAPH_ALIGN_PAGE);
    unsigned int texture_size =
        graph_vram_size(TEST_TEXTURE_STORAGE_WIDTH, TEST_TEXTURE_HEIGHT,
                        GS_PSM_32, GRAPH_ALIGN_BLOCK);
    int index;

    if (test_texture.address + texture_size > GRAPH_VRAM_MAX_WORDS) {
        return false;
    }

    for (index = 0; index < FRAMEBUFFER_COUNT; ++index) {
        if (ranges_overlap(test_texture.address, texture_size,
                           framebuffers[index].address, framebuffer_size)) {
            return false;
        }
    }

    return true;
}

static void submit_packet(qword_t *end)
{
    dma_wait_fast();
    dma_channel_send_normal(DMA_CHANNEL_GIF, draw_packet->data,
                            end - draw_packet->data, 0, 0);
    draw_wait_finish();
}

static void select_draw_buffer(int buffer_index)
{
    qword_t *q = draw_packet->data;

    q = draw_framebuffer(q, 0, &framebuffers[buffer_index]);
    q = draw_finish(q);
    submit_packet(q);
}

static void clear_draw_buffer(void)
{
    qword_t *q = draw_packet->data;

    q = draw_clear(q, 0, 0.0f, 0.0f, VIDEO_WIDTH, VIDEO_HEIGHT, 0x08, 0x0c,
                   0x12);
    q = draw_finish(q);
    submit_packet(q);
}

static bool upload_test_texture(void)
{
    packet_t *upload_packet =
        packet_init(TEXTURE_UPLOAD_PACKET_QWORDS, PACKET_NORMAL);
    qword_t *q;

    if (upload_packet == NULL) {
        return false;
    }

    q = upload_packet->data;
    q = draw_texture_transfer(q, test_texture_pixels,
                              TEST_TEXTURE_STORAGE_WIDTH,
                              TEST_TEXTURE_HEIGHT, GS_PSM_32,
                              test_texture.address, test_texture.width);
    q = draw_texture_flush(q);

    dma_channel_send_chain(DMA_CHANNEL_GIF, upload_packet->data,
                           q - upload_packet->data, 0, 0);
    dma_wait_fast();
    packet_free(upload_packet);
    return true;
}

bool video_initialize(void)
{
    qword_t *q;
    int index;

    init_scr();

    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);

    graph_vram_clear();
    for (index = 0; index < FRAMEBUFFER_COUNT; ++index) {
        framebuffers[index].width = VIDEO_WIDTH;
        framebuffers[index].height = VIDEO_HEIGHT;
        framebuffers[index].mask = 0;
        framebuffers[index].psm = GS_PSM_32;
        framebuffers[index].address =
            graph_vram_allocate(VIDEO_WIDTH, VIDEO_HEIGHT, GS_PSM_32,
                                GRAPH_ALIGN_PAGE);

        if ((int)framebuffers[index].address < 0) {
            graph_vram_clear();
            dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
            return false;
        }
    }

    /* GS texture-buffer width is encoded in 64-pixel units. */
    test_texture.width = TEST_TEXTURE_STORAGE_WIDTH;
    test_texture.psm = GS_PSM_32;
    test_texture.address = graph_vram_allocate(
        TEST_TEXTURE_STORAGE_WIDTH, TEST_TEXTURE_HEIGHT, GS_PSM_32,
        GRAPH_ALIGN_BLOCK);
    if ((int)test_texture.address < 0 || !test_texture_layout_is_valid()) {
        graph_vram_clear();
        dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
        return false;
    }
    test_texture.info.width = draw_log2(TEST_TEXTURE_WIDTH);
    test_texture.info.height = draw_log2(TEST_TEXTURE_HEIGHT);
    test_texture.info.components = TEXTURE_COMPONENTS_RGBA;
    test_texture.info.function = TEXTURE_FUNCTION_DECAL;

    display_buffer_index = 1;
    draw_buffer_index = 0;

    graph_initialize(framebuffers[display_buffer_index].address, VIDEO_WIDTH,
                     VIDEO_HEIGHT, GS_PSM_32, 0, 0);

    zbuffer.enable = DRAW_DISABLE;
    zbuffer.address = 0;
    zbuffer.mask = 0;
    zbuffer.zsm = 0;

    draw_packet = packet_init(DRAW_PACKET_QWORDS, PACKET_NORMAL);
    if (draw_packet == NULL) {
        graph_vram_clear();
        dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
        return false;
    }

    q = draw_packet->data;
    q = draw_setup_environment(q, 0, &framebuffers[draw_buffer_index],
                               &zbuffer);
    q = draw_finish(q);
    submit_packet(q);
    clear_draw_buffer();

    select_draw_buffer(display_buffer_index);
    clear_draw_buffer();
    select_draw_buffer(draw_buffer_index);

    generate_test_texture();
    if (!upload_test_texture()) {
        packet_free(draw_packet);
        draw_packet = NULL;
        graph_shutdown();
        graph_vram_clear();
        dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
        return false;
    }
    return true;
}

void video_begin_frame(void)
{
    clear_draw_buffer();
}

void video_draw_filled_rect(float x, float y, float width, float height,
                            unsigned char red, unsigned char green,
                            unsigned char blue)
{
    rect_t rectangle;
    qword_t *q = draw_packet->data;

    rectangle.v0.x = x;
    rectangle.v0.y = y;
    rectangle.v0.z = 0;
    rectangle.v1.x = x + width;
    rectangle.v1.y = y + height;
    rectangle.v1.z = 0;
    rectangle.color.r = red;
    rectangle.color.g = green;
    rectangle.color.b = blue;
    rectangle.color.a = 0x80;
    rectangle.color.q = 1.0f;

    q = draw_rect_filled(q, 0, &rectangle);
    q = draw_finish(q);
    submit_packet(q);
}

void video_draw_test_sprite(float x, float y, float width, float height)
{
    clutbuffer_t clut = {0};
    lod_t lod = {0};
    texrect_t rectangle;
    texwrap_t wrap;
    qword_t *q = draw_packet->data;

    lod.calculation = LOD_USE_K;
    lod.max_level = 0;
    lod.mag_filter = LOD_MAG_NEAREST;
    lod.min_filter = LOD_MIN_NEAREST;
    lod.mipmap_select = LOD_MIPMAP_REGISTER;
    lod.l = 0;
    lod.k = 0.0f;

    clut.storage_mode = CLUT_STORAGE_MODE1;
    clut.load_method = CLUT_NO_LOAD;

    wrap.horizontal = WRAP_CLAMP;
    wrap.vertical = WRAP_CLAMP;
    wrap.minu = 0;
    wrap.maxu = TEST_TEXTURE_WIDTH - 1;
    wrap.minv = 0;
    wrap.maxv = TEST_TEXTURE_HEIGHT - 1;

    rectangle.v0.x = x;
    rectangle.v0.y = y;
    rectangle.v0.z = 0;
    rectangle.t0.u = 0.0f;
    rectangle.t0.v = 0.0f;
    rectangle.v1.x = x + width;
    rectangle.v1.y = y + height;
    rectangle.v1.z = 0;
    /* libdraw converts texel-space coordinates to GS 12.4 fixed-point UV. */
    rectangle.t1.u = (float)TEST_TEXTURE_WIDTH;
    rectangle.t1.v = (float)TEST_TEXTURE_HEIGHT;
    rectangle.color.r = 0x80;
    rectangle.color.g = 0x80;
    rectangle.color.b = 0x80;
    rectangle.color.a = 0x80;
    rectangle.color.q = 1.0f;

    /* Bind immediately before use so prior primitive state cannot affect it. */
    q = draw_texture_sampling(q, 0, &lod);
    q = draw_texture_wrapping(q, 0, &wrap);
    q = draw_texturebuffer(q, 0, &test_texture, &clut);
    q = draw_finish(q);
    submit_packet(q);

    q = draw_packet->data;
    q = draw_rect_textured(q, 0, &rectangle);
    q = draw_finish(q);
    submit_packet(q);
}

void video_draw_text(int x, int y, const char *format, ...)
{
    char text[TEXT_FORMAT_BUFFER_SIZE];
    float origin_x = (float)(x * FONT_GLYPH_WIDTH);
    float cursor_x = origin_x;
    float cursor_y = (float)(y * FONT_GLYPH_HEIGHT);
    const unsigned char *character;
    qword_t *q = draw_packet->data;
    va_list arguments;

    va_start(arguments, format);
    vsnprintf(text, sizeof(text), format, arguments);
    va_end(arguments);

    for (character = (const unsigned char *)text; *character != '\0';
         ++character) {
        unsigned int glyph = *character;
        int row;

        if (glyph == '\n') {
            cursor_x = origin_x;
            cursor_y += FONT_GLYPH_HEIGHT;
            continue;
        }

        if (glyph < FONT_FIRST_CHARACTER ||
            glyph >= FONT_FIRST_CHARACTER + FONT_CHARACTER_COUNT) {
            glyph = '?';
        }

        for (row = 0; row < FONT_GLYPH_HEIGHT; ++row) {
            unsigned char bits = msx[glyph * FONT_GLYPH_HEIGHT + row];
            int column = 0;

            while (column < FONT_GLYPH_WIDTH) {
                int run_start;
                rect_t rectangle;

                while (column < FONT_GLYPH_WIDTH &&
                       (bits & (0x80u >> column)) == 0) {
                    ++column;
                }
                if (column == FONT_GLYPH_WIDTH) {
                    break;
                }

                run_start = column;
                while (column < FONT_GLYPH_WIDTH &&
                       (bits & (0x80u >> column)) != 0) {
                    ++column;
                }

                if (q - draw_packet->data >=
                    DRAW_PACKET_QWORDS - TEXT_PACKET_RESERVED_QWORDS) {
                    q = draw_finish(q);
                    submit_packet(q);
                    q = draw_packet->data;
                }

                rectangle.v0.x = cursor_x + run_start;
                rectangle.v0.y = cursor_y + row;
                rectangle.v0.z = 0;
                rectangle.v1.x = cursor_x + column;
                rectangle.v1.y = cursor_y + row + 1.0f;
                rectangle.v1.z = 0;
                rectangle.color.r = 0xff;
                rectangle.color.g = 0xff;
                rectangle.color.b = 0xff;
                rectangle.color.a = 0x80;
                rectangle.color.q = 1.0f;
                q = draw_rect_filled(q, 0, &rectangle);
            }
        }

        cursor_x += FONT_GLYPH_WIDTH;
    }

    if (q != draw_packet->data) {
        q = draw_finish(q);
        submit_packet(q);
    }
}

void video_present_frame(void)
{
    qword_t *q = draw_packet->data;
    int completed_buffer_index = draw_buffer_index;

    q = draw_finish(q);
    submit_packet(q);

    graph_wait_vsync();
    graph_set_framebuffer_filtered(
        framebuffers[completed_buffer_index].address, VIDEO_WIDTH, GS_PSM_32,
        0, 0);

    display_buffer_index = completed_buffer_index;
    draw_buffer_index = 1 - completed_buffer_index;
    select_draw_buffer(draw_buffer_index);
}

int video_get_display_buffer_index(void)
{
    return display_buffer_index;
}

int video_get_draw_buffer_index(void)
{
    return draw_buffer_index;
}

int video_get_test_texture_width(void)
{
    return TEST_TEXTURE_WIDTH;
}

int video_get_test_texture_height(void)
{
    return TEST_TEXTURE_HEIGHT;
}

const char *video_get_test_texture_format(void)
{
    return "GS_PSM_32 RGBA";
}

void video_shutdown(void)
{
    if (draw_packet != NULL) {
        packet_free(draw_packet);
        draw_packet = NULL;
    }

    graph_shutdown();
    graph_vram_free(framebuffers[0].address);
    graph_vram_free(framebuffers[1].address);
    graph_vram_free(test_texture.address);
    dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
}

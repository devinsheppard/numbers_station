#include "video.h"

#include <debug.h>
#include <dma.h>
#include <draw.h>
#include <graph.h>
#include <gs_psm.h>
#include <packet.h>
#include <stdarg.h>

enum {
    VIDEO_WIDTH = 640,
    VIDEO_HEIGHT = 448,
    FRAMEBUFFER_COUNT = 2,
    DRAW_PACKET_QWORDS = 64
};

static framebuffer_t framebuffers[FRAMEBUFFER_COUNT];
static zbuffer_t zbuffer;
static packet_t *draw_packet;
static int display_buffer_index;
static int draw_buffer_index;

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

void video_draw_text(int x, int y, const char *format, ...)
{
    va_list arguments;

    scr_setXY(x, y);
    va_start(arguments, format);
    scr_vprintf(format, arguments);
    va_end(arguments);
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

void video_shutdown(void)
{
    if (draw_packet != NULL) {
        packet_free(draw_packet);
        draw_packet = NULL;
    }

    graph_shutdown();
    graph_vram_free(framebuffers[0].address);
    graph_vram_free(framebuffers[1].address);
    dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
}

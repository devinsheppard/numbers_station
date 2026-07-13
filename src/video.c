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
    DRAW_PACKET_QWORDS = 64
};

static framebuffer_t framebuffer;
static zbuffer_t zbuffer;
static packet_t *draw_packet;

static void submit_packet(qword_t *end)
{
    dma_wait_fast();
    dma_channel_send_normal(DMA_CHANNEL_GIF, draw_packet->data,
                            end - draw_packet->data, 0, 0);
    draw_wait_finish();
}

bool video_initialize(void)
{
    qword_t *q;

    init_scr();

    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);

    framebuffer.width = VIDEO_WIDTH;
    framebuffer.height = VIDEO_HEIGHT;
    framebuffer.mask = 0;
    framebuffer.psm = GS_PSM_32;
    framebuffer.address = 0;

    zbuffer.enable = DRAW_DISABLE;
    zbuffer.address = 0;
    zbuffer.mask = 0;
    zbuffer.zsm = 0;

    draw_packet = packet_init(DRAW_PACKET_QWORDS, PACKET_NORMAL);
    if (draw_packet == NULL) {
        dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
        return false;
    }

    q = draw_packet->data;
    q = draw_setup_environment(q, 0, &framebuffer, &zbuffer);
    q = draw_finish(q);
    submit_packet(q);

    return true;
}

void video_begin_frame(void)
{
    qword_t *q;

    graph_wait_vsync();
    q = draw_packet->data;
    q = draw_clear(q, 0, 0.0f, 0.0f, VIDEO_WIDTH, VIDEO_HEIGHT, 0x08, 0x0c,
                   0x12);
    q = draw_finish(q);
    submit_packet(q);
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

void video_shutdown(void)
{
    if (draw_packet != NULL) {
        packet_free(draw_packet);
        draw_packet = NULL;
    }

    scr_clear();
    dma_channel_shutdown(DMA_CHANNEL_GIF, 0);
}

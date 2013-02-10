#include <stdint.h>

volatile uint32_t *GPU_READ   = (uint32_t *)0x2000B880;
volatile uint32_t *GPU_POLL   = (uint32_t *)0x2000B890;
volatile uint32_t *GPU_SENDER = (uint32_t *)0x2000B894;
volatile uint32_t *GPU_STATUS = (uint32_t *)0x2000B898;
volatile uint32_t *GPU_CONFIG = (uint32_t *)0x2000B89C;
volatile uint32_t *GPU_WRITE  = (uint32_t *)0x2000B8A0;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t buffer_width;
    uint32_t buffer_height;
    uint32_t pitch;
    uint32_t bit_depth;
    uint32_t x;
    uint32_t y;
    void     *buffer;
    uint32_t buffer_size;
} GFX_INIT_REQUEST __attribute__ ((aligned (16)));

void send_mail(uint32_t message, uint32_t box) {
    while (*GPU_STATUS & 0x80000000);

    message = (message & 0xFFFFFFF0) | (box & 0xF);
    *GPU_WRITE = message;
}

uint32_t recv_mail(uint32_t box) {
    uint32_t value;
    box &= 0xF;
    while (1) {
        while (*GPU_STATUS & 0x40000000);
        value = *GPU_READ;
        if ((value & 0xF) == box) {
            break;
        }
    }

    return value & ~ 0xF;
}

GFX_INIT_REQUEST init_request;

void *set_gfx_mode(uint32_t w, uint32_t h, uint32_t bpp) {
    init_request.width  = init_request.buffer_width = w;
    init_request.height = init_request.buffer_height = h;
    init_request.buffer = 0;
    init_request.pitch  = init_request.x = init_request.y = init_request.buffer_size = 0;
    init_request.bit_depth = bpp;

    send_mail((uint32_t)&init_request, 1);
    uint32_t reply = recv_mail(1);

    if (reply == 0) {
        return (void*)init_request.buffer;
    }
    return 0;
}

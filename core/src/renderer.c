/*
 * renderer.c — платформо-независимый framebuffer.
 *
 * Этот файл не содержит вызовов SDL. Платформенный бэкенд
 * забирает содержимое framebuffer через core_renderer_framebuffer()
 * и отправляет его в оконную систему.
 */

#include "core.h"

#include <string.h>
#include <stdlib.h>

#define MAX_WIDTH  1920
#define MAX_HEIGHT 1080

static uint32_t g_framebuffer[MAX_WIDTH * MAX_HEIGHT];
static int g_width  = 0;
static int g_height = 0;

/* Встроенный шрифт 5x7 (определён в font.c). */
extern const uint8_t *core_font_glyph(char c, int *w, int *h);

bool core_renderer_init(int width, int height, const char *title) {
    (void)title;
    if (width <= 0 || height <= 0 ||
        width > MAX_WIDTH || height > MAX_HEIGHT) {
        return false;
    }
    g_width = width;
    g_height = height;
    memset(g_framebuffer, 0,
           sizeof(uint32_t) * (size_t)width * (size_t)height);
    return true;
}

void core_renderer_shutdown(void) {
    g_width = 0;
    g_height = 0;
}

void core_renderer_clear(core_color_t color) {
    int total = g_width * g_height;
    for (int i = 0; i < total; ++i) {
        g_framebuffer[i] = color;
    }
}

void core_renderer_rect(int x, int y, int w, int h, core_color_t color) {
    if (w <= 0 || h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w;
    int y1 = y + h;
    if (x1 > g_width)  x1 = g_width;
    if (y1 > g_height) y1 = g_height;
    if (x0 >= x1 || y0 >= y1) return;

    for (int yy = y0; yy < y1; ++yy) {
        uint32_t *row = &g_framebuffer[yy * g_width];
        for (int xx = x0; xx < x1; ++xx) {
            row[xx] = color;
        }
    }
}

void core_renderer_rect_outline(int x, int y, int w, int h,
                                int thickness, core_color_t color) {
    if (thickness <= 0 || w <= 0 || h <= 0) return;
    core_renderer_rect(x, y, w, thickness, color);
    core_renderer_rect(x, y + h - thickness, w, thickness, color);
    core_renderer_rect(x, y, thickness, h, color);
    core_renderer_rect(x + w - thickness, y, thickness, h, color);
}

void core_renderer_text(int x, int y, const char *text, core_color_t color) {
    if (!text) return;
    int cx = x;
    while (*text) {
        int gw = 0, gh = 0;
        const uint8_t *glyph = core_font_glyph(*text, &gw, &gh);
        if (glyph) {
            for (int row = 0; row < gh; ++row) {
                for (int col = 0; col < gw; ++col) {
                    if (glyph[row] & (1u << (gw - 1 - col))) {
                        core_renderer_rect(cx + col, y + row, 1, 1, color);
                    }
                }
            }
        }
        cx += gw + 1;
        ++text;
    }
}

void core_renderer_present(void) {
    /* Реализуется платформенным бэкендом. */
}

const uint32_t *core_renderer_framebuffer(int *w, int *h) {
    if (w) *w = g_width;
    if (h) *h = g_height;
    return g_framebuffer;
}
/* test_renderer.c — проверка работы framebuffer. */

#include <stdio.h>
#include <stdlib.h>
#include "core.h"

extern const uint32_t *core_renderer_framebuffer(int *w, int *h);

#define ASSERT(cond, msg)                                             \
    do {                                                              \
        if (!(cond)) {                                                \
            fprintf(stderr, "FAIL %s:%d: %s\n",                       \
                    __FILE__, __LINE__, msg);                         \
            exit(1);                                                  \
        }                                                             \
    } while (0)

static void test_clear(void) {
    core_renderer_init(100, 100, "test");
    core_renderer_clear(0xAABBCC);
    int w, h;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    ASSERT(w == 100 && h == 100, "размеры framebuffer");
    for (int i = 0; i < 100 * 100; ++i) {
        ASSERT(fb[i] == 0xAABBCC, "цвет после clear");
    }
    core_renderer_shutdown();
}

static void test_rect_clipping(void) {
    core_renderer_init(20, 20, "test");
    core_renderer_clear(0x000000);
    /* Рисуем прямоугольник, выходящий за границы */
    core_renderer_rect(-5, -5, 10, 10, 0xFF0000);
    int w, h;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    /* Пиксель (0,0) должен быть закрашен */
    ASSERT(fb[0] == 0xFF0000, "пиксель (0,0)");
    /* Пиксель (6,6) должен остаться чёрным */
    ASSERT(fb[6 * 20 + 6] == 0x000000, "пиксель (6,6)");
    core_renderer_shutdown();
}

static void test_rect_negative_size(void) {
    core_renderer_init(20, 20, "test");
    core_renderer_clear(0x111111);
    /* Отрицательные размеры игнорируются в api_core.c,
     * но core_renderer_rect должен корректно отработать. */
    core_renderer_rect(5, 5, -10, -10, 0xFFFFFF);
    int w, h;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    for (int i = 0; i < 400; ++i) {
        ASSERT(fb[i] == 0x111111, "фон не изменился");
    }
    core_renderer_shutdown();
}

void test_renderer_run(void) {
    test_clear();
    test_rect_clipping();
    test_rect_negative_size();
    printf("  [OK] renderer\n");
}
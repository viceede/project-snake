/*
 * core.h — публичный интерфейс низкоуровневого ядра.
 *
 * Ядро не содержит игровой логики. Оно предоставляет:
 *  - абстракцию рендеринга (SDL-независимую);
 *  - абстракцию ввода;
 *  - таймеры;
 *  - PRNG;
 *  - мост C ↔ Lua.
 */

#ifndef SNAKE_CORE_H
#define SNAKE_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- Абстракция рендеринга ---------- */

typedef uint32_t core_color_t;

bool core_renderer_init(int width, int height, const char *title);
void core_renderer_shutdown(void);
void core_renderer_clear(core_color_t color);
void core_renderer_rect(int x, int y, int w, int h, core_color_t color);
void core_renderer_rect_outline(int x, int y, int w, int h,
                                int thickness, core_color_t color);
void core_renderer_text(int x, int y, const char *text, core_color_t color);
void core_renderer_present(void);

/* Доступ к внутреннему framebuffer (для бэкендов и тестов). */
const uint32_t *core_renderer_framebuffer(int *w, int *h);

/* ---------- Абстракция ввода ---------- */

typedef enum {
    CORE_EVENT_NONE = 0,
    CORE_EVENT_QUIT,
    CORE_EVENT_KEY_UP,
    CORE_EVENT_KEY_DOWN,
    CORE_EVENT_KEY_LEFT,
    CORE_EVENT_KEY_RIGHT,
    CORE_EVENT_KEY_SPACE,
    CORE_EVENT_KEY_R,
    CORE_EVENT_KEY_ESC,
    CORE_EVENT_KEY_W,
    CORE_EVENT_KEY_A,
    CORE_EVENT_KEY_S,
    CORE_EVENT_KEY_D,
    CORE_EVENT_TOUCH
} core_event_type_t;

typedef struct {
    core_event_type_t type;
    int x;
    int y;
} core_event_t;

/* Публичный API бэкендов: положить событие в очередь. */
void core_input_push(const core_event_t *ev);

/* Забрать одно событие из очереди (CORE_EVENT_NONE — если пусто). */
core_event_t core_input_poll(void);

/* ---------- Таймеры ---------- */

uint64_t core_time_ms(void);
void core_sleep_ms(uint32_t ms);

/* ---------- PRNG ---------- */

void core_random_seed(uint32_t seed);
int  core_random_int(int n);

/* ---------- Мост C ↔ Lua ---------- */

bool core_vm_init(const char *script_dir);
void core_vm_update(float dt);
void core_vm_render(void);
void core_vm_event(const core_event_t *event);
void core_vm_shutdown(void);

/* Регистрация C-функций в глобальной таблице Lua.
 * Принимает void*, чтобы core.h не тянул за собой Lua-заголовки. */
void core_api_register(void *lua_state_ptr);

#ifdef __cplusplus
}
#endif

#endif /* SNAKE_CORE_H */
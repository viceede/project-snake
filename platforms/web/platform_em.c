/*
 * platform_em.c — бэкенд для Emscripten (WebAssembly).
 *
 * В Web-версии главный цикл управляется браузером через
 * emscripten_set_main_loop. Файл scripts/main.lua упаковывается
 * внутрь .data при сборке (см. --preload-file в CMakeLists).
 */

#include <emscripten.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "core.h"

#define WINDOW_W 800
#define WINDOW_H 640
#define WINDOW_TITLE "Snake"

extern const uint32_t *core_renderer_framebuffer(int *w, int *h);
extern void core_input_push(const core_event_t *ev);
extern bool core_vm_init(const char *script_dir);
extern void core_vm_update(float dt);
extern void core_vm_render(void);
extern void core_vm_event(const core_event_t *event);
extern void core_vm_shutdown(void);

static SDL_Window   *g_win = NULL;
static SDL_Renderer *g_ren = NULL;
static SDL_Texture  *g_tex = NULL;
static uint64_t      g_prev = 0;
static bool          g_running = true;

static void push_event(core_event_type_t t, int x, int y) {
    core_event_t ev = { t, x, y };
    core_input_push(&ev);
}

static void handle_sdl_event(const SDL_Event *e) {
    if (e->type == SDL_QUIT) { push_event(CORE_EVENT_QUIT, 0, 0); return; }
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:    push_event(CORE_EVENT_KEY_UP,    0, 0); break;
            case SDLK_DOWN:  push_event(CORE_EVENT_KEY_DOWN,  0, 0); break;
            case SDLK_LEFT:  push_event(CORE_EVENT_KEY_LEFT,  0, 0); break;
            case SDLK_RIGHT: push_event(CORE_EVENT_KEY_RIGHT, 0, 0); break;
            case SDLK_SPACE: push_event(CORE_EVENT_KEY_SPACE, 0, 0); break;
            case SDLK_r:     push_event(CORE_EVENT_KEY_R,     0, 0); break;
            case SDLK_ESCAPE:push_event(CORE_EVENT_KEY_ESC,   0, 0); break;
            default: break;
        }
    }
    if (e->type == SDL_FINGERDOWN) {
        push_event(CORE_EVENT_TOUCH,
                   (int)(e->tfinger.x * WINDOW_W),
                   (int)(e->tfinger.y * WINDOW_H));
    }
}

static void present_framebuffer(void) {
    int w = 0, h = 0;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    if (!fb) return;
    if (!g_tex) {
        g_tex = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING, w, h);
    }
    SDL_UpdateTexture(g_tex, NULL, fb, w * sizeof(uint32_t));
    SDL_RenderClear(g_ren);
    SDL_RenderCopy(g_ren, g_tex, NULL, NULL);
    SDL_RenderPresent(g_ren);
}

static void main_loop(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) handle_sdl_event(&e);

    core_event_t ev;
    while ((ev = core_input_poll()).type != CORE_EVENT_NONE) {
        if (ev.type == CORE_EVENT_QUIT || ev.type == CORE_EVENT_KEY_ESC) {
            g_running = false;
            emscripten_cancel_main_loop();
            return;
        }
        core_vm_event(&ev);
    }

    uint64_t now = core_time_ms();
    float dt = (now - g_prev) / 1000.0f;
    g_prev = now;

    core_vm_update(dt);
    core_vm_render();
    present_framebuffer();
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    g_win = SDL_CreateWindow(WINDOW_TITLE, 0, 0,
                             WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    g_ren = SDL_CreateRenderer(g_win, -1,
                              SDL_RENDERER_ACCELERATED |
                              SDL_RENDERER_PRESENTVSYNC);
    core_renderer_init(WINDOW_W, WINDOW_H, WINDOW_TITLE);
    core_random_seed((uint32_t)core_time_ms());

    if (!core_vm_init("/scripts")) {
        return 1;
    }
    g_prev = core_time_ms();
    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}
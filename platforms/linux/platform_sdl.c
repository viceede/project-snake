/*
 * platform_sdl.c — бэкенд SDL2 для Linux/WSL/macOS.
 *
 * Реализует:
 *  - создание окна и SDL-рендерера;
 *  - трансляцию SDL-событий в core_event_t;
 *  - представление framebuffer на экран;
 *  - главный цикл игры.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "core.h"

#define WINDOW_W 800
#define WINDOW_H 640
#define WINDOW_TITLE "Snake"

/* Публичный API из core/src/renderer.c */
extern const uint32_t *core_renderer_framebuffer(int *w, int *h);

/* Публичный API из core/src/input.c */
extern void core_input_push(const core_event_t *ev);

/* Внутренний API ядра */
extern bool core_vm_init(const char *script_dir);
extern void core_vm_update(float dt);
extern void core_vm_render(void);
extern void core_vm_event(const core_event_t *event);
extern void core_vm_shutdown(void);

static SDL_Window   *g_win = NULL;
static SDL_Renderer *g_ren = NULL;
static SDL_Texture  *g_tex = NULL;

static bool sdl_init_all(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return false;
    }
    g_win = SDL_CreateWindow(WINDOW_TITLE,
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             WINDOW_W, WINDOW_H, 0);
    if (!g_win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }
    g_ren = SDL_CreateRenderer(g_win, -1,
                              SDL_RENDERER_ACCELERATED |
                              SDL_RENDERER_PRESENTVSYNC);
    if (!g_ren) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

static void sdl_shutdown_all(void) {
    if (g_tex) SDL_DestroyTexture(g_tex);
    if (g_ren) SDL_DestroyRenderer(g_ren);
    if (g_win) SDL_DestroyWindow(g_win);
    SDL_Quit();
}

static void push_event(core_event_type_t type, int x, int y) {
    core_event_t ev = { type, x, y };
    core_input_push(&ev);
}

static void handle_sdl_event(const SDL_Event *e) {
    if (e->type == SDL_QUIT) {
        push_event(CORE_EVENT_QUIT, 0, 0);
        return;
    }
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:    push_event(CORE_EVENT_KEY_UP,    0, 0); break;
            case SDLK_DOWN:  push_event(CORE_EVENT_KEY_DOWN,  0, 0); break;
            case SDLK_LEFT:  push_event(CORE_EVENT_KEY_LEFT,  0, 0); break;
            case SDLK_RIGHT: push_event(CORE_EVENT_KEY_RIGHT, 0, 0); break;
            case SDLK_w:     push_event(CORE_EVENT_KEY_UP,    0, 0); break;
            case SDLK_s:     push_event(CORE_EVENT_KEY_DOWN,  0, 0); break;
            case SDLK_a:     push_event(CORE_EVENT_KEY_LEFT,  0, 0); break;
            case SDLK_d:     push_event(CORE_EVENT_KEY_RIGHT, 0, 0); break;
            case SDLK_SPACE: push_event(CORE_EVENT_KEY_SPACE, 0, 0); break;
            case SDLK_r:     push_event(CORE_EVENT_KEY_R,     0, 0); break;
            case SDLK_ESCAPE:push_event(CORE_EVENT_KEY_ESC,   0, 0); break;
            default: break;
        }
    }
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        push_event(CORE_EVENT_TOUCH, e->button.x, e->button.y);
    }
}

static void present_framebuffer(void) {
    int w = 0, h = 0;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    if (!fb || w == 0 || h == 0) return;

    if (!g_tex) {
        g_tex = SDL_CreateTexture(g_ren,
                                  SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  w, h);
        if (!g_tex) return;
    }
    SDL_UpdateTexture(g_tex, NULL, fb, w * sizeof(uint32_t));
    SDL_RenderClear(g_ren);
    SDL_RenderCopy(g_ren, g_tex, NULL, NULL);
    SDL_RenderPresent(g_ren);
}

int main(int argc, char **argv) {
    const char *script_dir = (argc > 1) ? argv[1] : "scripts";

    if (!sdl_init_all()) {
        sdl_shutdown_all();
        return 1;
    }
    if (!core_renderer_init(WINDOW_W, WINDOW_H, WINDOW_TITLE)) {
        sdl_shutdown_all();
        return 1;
    }

    core_random_seed((uint32_t)SDL_GetTicks());

    if (!core_vm_init(script_dir)) {
        fprintf(stderr, "Не удалось инициализировать Lua VM\n");
        core_renderer_shutdown();
        sdl_shutdown_all();
        return 1;
    }

    bool running = true;
    uint64_t prev = core_time_ms();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            handle_sdl_event(&e);
        }
        core_event_t ev;
        while ((ev = core_input_poll()).type != CORE_EVENT_NONE) {
            if (ev.type == CORE_EVENT_QUIT || ev.type == CORE_EVENT_KEY_ESC) {
                running = false;
                break;
            }
            core_vm_event(&ev);
        }
        if (!running) break;

        uint64_t now = core_time_ms();
        float dt = (now - prev) / 1000.0f;
        prev = now;

        core_vm_update(dt);
        core_vm_render();
        present_framebuffer();

        SDL_Delay(1);
    }

    core_vm_shutdown();
    core_renderer_shutdown();
    sdl_shutdown_all();
    return 0;
}
/* test_api.c — проверка C-функций, экспортируемых в Lua. */

#include <stdio.h>
#include <stdlib.h>
#include "minilua.h"
#include "core.h"

#define ASSERT(cond, msg)                                             \
    do {                                                              \
        if (!(cond)) {                                                \
            fprintf(stderr, "FAIL %s:%d: %s\n",                       \
                    __FILE__, __LINE__, msg);                         \
            exit(1);                                                  \
        }                                                             \
    } while (0)

/* Проверяем, что draw_rect корректно обрабатывает
 * отрицательные размеры (не рисует). */
static void test_draw_rect_negative(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    core_api_register(L);

    core_renderer_init(20, 20, "test");
    core_renderer_clear(0x111111);

    lua_getglobal(L, "draw_rect");
    lua_pushinteger(L, 5);
    lua_pushinteger(L, 5);
    lua_pushinteger(L, -10);
    lua_pushinteger(L, -10);
    lua_pushinteger(L, 0xFFFFFF);
    int rc = lua_pcall(L, 5, 0, 0);
    ASSERT(rc == LUA_OK, "draw_rect вызов");

    extern const uint32_t *core_renderer_framebuffer(int *w, int *h);
    int w, h;
    const uint32_t *fb = core_renderer_framebuffer(&w, &h);
    for (int i = 0; i < 400; ++i) {
        ASSERT(fb[i] == 0x111111, "framebuffer не изменён");
    }
    core_renderer_shutdown();
    lua_close(L);
}

/* Проверяем, что random(n) возвращает значения в [0, n). */
static void test_random_range(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    core_api_register(L);
    core_random_seed(42);

    for (int i = 0; i < 1000; ++i) {
        lua_getglobal(L, "random");
        lua_pushinteger(L, 10);
        lua_pcall(L, 1, 1, 0);
        lua_Integer v = lua_tointeger(L, -1);
        ASSERT(v >= 0 && v < 10, "random в диапазоне");
        lua_pop(L, 1);
    }
    lua_close(L);
}

/* Проверяем, что random(0) возвращает 0 и не падает. */
static void test_random_zero(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    core_api_register(L);

    lua_getglobal(L, "random");
    lua_pushinteger(L, 0);
    lua_pcall(L, 1, 1, 0);
    ASSERT(lua_tointeger(L, -1) == 0, "random(0) == 0");
    lua_pop(L, 1);
    lua_close(L);
}

void test_api_run(void) {
    test_draw_rect_negative();
    test_random_range();
    test_random_zero();
    printf("  [OK] api\n");
}
/*
 * api_core.c — регистрация C-функций, доступных из Lua.
 */

#include "minilua.h"
#include "core.h"

#include <stdio.h>

/* ---------- Функции, вызываемые из Lua ---------- */

/* draw_rect(x, y, w, h, color) */
static int l_draw_rect(lua_State *L) {
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    core_color_t c = (core_color_t)luaL_checkinteger(L, 5);

    if (w <= 0 || h <= 0) return 0;
    core_renderer_rect(x, y, w, h, c);
    return 0;
}

/* draw_rect_outline(x, y, w, h, thickness, color) */
static int l_draw_rect_outline(lua_State *L) {
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    int t = (int)luaL_checkinteger(L, 5);
    core_color_t c = (core_color_t)luaL_checkinteger(L, 6);
    core_renderer_rect_outline(x, y, w, h, t, c);
    return 0;
}

/* draw_text(x, y, text, color) */
static int l_draw_text(lua_State *L) {
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    const char *s = luaL_checkstring(L, 3);
    core_color_t c = (core_color_t)luaL_checkinteger(L, 4);
    core_renderer_text(x, y, s, c);
    return 0;
}

/* clear(color) */
static int l_clear(lua_State *L) {
    core_color_t c = (core_color_t)luaL_checkinteger(L, 1);
    core_renderer_clear(c);
    return 0;
}

/* random(n) -> [0, n) */
static int l_random(lua_State *L) {
    int n = (int)luaL_checkinteger(L, 1);
    lua_pushinteger(L, n > 0 ? core_random_int(n) : 0);
    return 1;
}

/* time_ms() -> целое число миллисекунд */
static int l_time_ms(lua_State *L) {
    lua_pushinteger(L, (lua_Integer)core_time_ms());
    return 1;
}

/* log(text) */
static int l_log(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
    fprintf(stderr, "[lua] %s\n", s);
    return 0;
}

/* ---------- Регистрация ---------- */

void core_api_register(void *lua_state_ptr) {
    lua_State *L = (lua_State *)lua_state_ptr;

    lua_register(L, "draw_rect",         l_draw_rect);
    lua_register(L, "draw_rect_outline", l_draw_rect_outline);
    lua_register(L, "draw_text",         l_draw_text);
    lua_register(L, "clear",             l_clear);
    lua_register(L, "random",            l_random);
    lua_register(L, "time_ms",           l_time_ms);
    lua_register(L, "log",               l_log);

    /* Константы для событий ввода */
    lua_pushinteger(L, CORE_EVENT_QUIT);        lua_setglobal(L, "EV_QUIT");
    lua_pushinteger(L, CORE_EVENT_KEY_UP);      lua_setglobal(L, "EV_UP");
    lua_pushinteger(L, CORE_EVENT_KEY_DOWN);    lua_setglobal(L, "EV_DOWN");
    lua_pushinteger(L, CORE_EVENT_KEY_LEFT);    lua_setglobal(L, "EV_LEFT");
    lua_pushinteger(L, CORE_EVENT_KEY_RIGHT);   lua_setglobal(L, "EV_RIGHT");
    lua_pushinteger(L, CORE_EVENT_KEY_SPACE);   lua_setglobal(L, "EV_SPACE");
    lua_pushinteger(L, CORE_EVENT_KEY_R);       lua_setglobal(L, "EV_RESTART");
    lua_pushinteger(L, CORE_EVENT_KEY_ESC);     lua_setglobal(L, "EV_ESC");
    lua_pushinteger(L, CORE_EVENT_TOUCH);       lua_setglobal(L, "EV_TOUCH");
}
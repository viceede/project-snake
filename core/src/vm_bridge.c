/*
 * vm_bridge.c — управление жизненным циклом Lua VM
 * и загрузка игровой логики.
 *
 * ВНИМАНИЕ: MINILUA_IMPLEMENTATION определён в отдельном файле
 * core/src/lua_impl.c. Здесь мы только используем Lua C API.
 */

#include "minilua.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static lua_State *g_L = NULL;

static bool load_script(const char *script_dir, const char *file) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", script_dir, file);

    if (luaL_loadfile(g_L, path) != LUA_OK) {
        fprintf(stderr, "[vm] Ошибка загрузки %s: %s\n",
                path, lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
        return false;
    }
    if (lua_pcall(g_L, 0, 0, 0) != LUA_OK) {
        fprintf(stderr, "[vm] Ошибка исполнения %s: %s\n",
                path, lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
        return false;
    }
    return true;
}

bool core_vm_init(const char *script_dir) {
    g_L = luaL_newstate();
    if (!g_L) {
        fprintf(stderr, "[vm] Не удалось создать Lua VM\n");
        return false;
    }
    luaL_openlibs(g_L);

    core_api_register(g_L);

    if (!load_script(script_dir, "main.lua")) {
        lua_close(g_L);
        g_L = NULL;
        return false;
    }
    return true;
}

void core_vm_update(float dt) {
    if (!g_L) return;
    lua_getglobal(g_L, "update");
    if (!lua_isfunction(g_L, -1)) {
        lua_pop(g_L, 1);
        return;
    }
    lua_pushnumber(g_L, dt);
    if (lua_pcall(g_L, 1, 0, 0) != LUA_OK) {
        fprintf(stderr, "[vm] update() ошибка: %s\n",
                lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
    }
}

void core_vm_render(void) {
    if (!g_L) return;
    lua_getglobal(g_L, "render");
    if (!lua_isfunction(g_L, -1)) {
        lua_pop(g_L, 1);
        return;
    }
    if (lua_pcall(g_L, 0, 0, 0) != LUA_OK) {
        fprintf(stderr, "[vm] render() ошибка: %s\n",
                lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
    }
}

void core_vm_event(const core_event_t *event) {
    if (!g_L || !event) return;
    lua_getglobal(g_L, "on_event");
    if (!lua_isfunction(g_L, -1)) {
        lua_pop(g_L, 1);
        return;
    }
    lua_newtable(g_L);
    lua_pushinteger(g_L, (lua_Integer)event->type);
    lua_setfield(g_L, -2, "type");
    lua_pushinteger(g_L, event->x);
    lua_setfield(g_L, -2, "x");
    lua_pushinteger(g_L, event->y);
    lua_setfield(g_L, -2, "y");

    if (lua_pcall(g_L, 1, 0, 0) != LUA_OK) {
        fprintf(stderr, "[vm] on_event() ошибка: %s\n",
                lua_tostring(g_L, -1));
        lua_pop(g_L, 1);
    }
}

void core_vm_shutdown(void) {
    if (g_L) {
        lua_close(g_L);
        g_L = NULL;
    }
}
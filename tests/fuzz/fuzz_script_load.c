/*
 * fuzz_script_load.c — фаззинг загрузчика Lua-скриптов.
 *
 * Подаём произвольные байты как исходник Lua и проверяем,
 * что загрузчик не падает и не утекает память.
 */

#define MINILUA_IMPLEMENTATION
#include "minilua.h"
#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    lua_State *L = luaL_newstate();
    if (!L) return 0;
    luaL_openlibs(L);

    if (luaL_loadbuffer(L, (const char *)data, size, "fuzz") == LUA_OK) {
        lua_pcall(L, 0, 0, 0);
    } else {
        lua_pop(L, 1);
    }
    lua_close(L);
    return 0;
}
/*
 * lua_impl.c — единственный translation unit, в котором
 * разворачивается реализация minilua.
 *
 * ВНИМАНИЕ: в актуальной версии minilua.h (Lua 5.5.0)
 * макрос называется LUA_IMPL, а не MINILUA_IMPLEMENTATION.
 *
 * Порядок строк критичен: #define LUA_IMPL
 * должен идти ДО #include "minilua.h".
 */

#define LUA_IMPL
#include "minilua.h"

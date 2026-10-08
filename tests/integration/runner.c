/*
 * runner.c — запускает все *.lua файлы из указанной директории
 * как интеграционные тесты игровой логики.
 *
 * Реализация Lua находится в core/src/lua_impl.c и попадает
 * в линковку через snake_core. Здесь мы только используем
 * Lua C API, поэтому #define MINILUA_IMPLEMENTATION
 * в этом файле ОТСУТСТВУЕТ.
 *
 * Аргументы командной строки:
 *   argv[1] — каталог с тестовыми файлами .lua
 *   argv[2] — корень проекта (для корректного package.path)
 */

#include "minilua.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* ---------- Заглушки для функций ядра ---------- */

static int l_stub_draw_rect(lua_State *L)   { (void)L; return 0; }
static int l_stub_draw_text(lua_State *L)   { (void)L; return 0; }
static int l_stub_clear(lua_State *L)       { (void)L; return 0; }
static int l_stub_log(lua_State *L) {
    const char *s = luaL_checkstring(L, 1);
    fprintf(stderr, "[lua] %s\n", s);
    return 0;
}
static int l_stub_random(lua_State *L) {
    int n = (int)luaL_checkinteger(L, 1);
    lua_pushinteger(L, n > 0 ? (rand() % n) : 0);
    return 1;
}
static int l_stub_time_ms(lua_State *L) {
    lua_pushinteger(L, 0);
    return 1;
}

static void register_stubs(lua_State *L) {
    lua_register(L, "draw_rect",         l_stub_draw_rect);
    lua_register(L, "draw_rect_outline", l_stub_draw_rect);
    lua_register(L, "draw_text",         l_stub_draw_text);
    lua_register(L, "clear",             l_stub_clear);
    lua_register(L, "random",            l_stub_random);
    lua_register(L, "time_ms",           l_stub_time_ms);
    lua_register(L, "log",               l_stub_log);

    lua_pushinteger(L, 0); lua_setglobal(L, "EV_QUIT");
    lua_pushinteger(L, 1); lua_setglobal(L, "EV_UP");
    lua_pushinteger(L, 2); lua_setglobal(L, "EV_DOWN");
    lua_pushinteger(L, 3); lua_setglobal(L, "EV_LEFT");
    lua_pushinteger(L, 4); lua_setglobal(L, "EV_RIGHT");
    lua_pushinteger(L, 5); lua_setglobal(L, "EV_SPACE");
    lua_pushinteger(L, 6); lua_setglobal(L, "EV_RESTART");
    lua_pushinteger(L, 7); lua_setglobal(L, "EV_ESC");
    lua_pushinteger(L, 8); lua_setglobal(L, "EV_TOUCH");
}

/* ---------- Настройка package.path ---------- */

/*
 * Формируем package.path так, чтобы работал require("scripts.game")
 * независимо от текущего рабочего каталога.
 *
 * Добавляем два шаблона:
 *   <project_root>/?.lua         — для require("scripts.game")
 *                                  → <project_root>/scripts/game.lua
 *   <project_root>/scripts/?.lua — на случай require("game")
 */
static void setup_package_path(lua_State *L, const char *project_root) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    const char *old = lua_tostring(L, -1);

    char newpath[4096];
    snprintf(newpath, sizeof(newpath),
             "%s;%s/?.lua;%s/scripts/?.lua;%s/scripts/?/init.lua",
             old ? old : "",
             project_root,
             project_root,
             project_root);

    lua_pop(L, 1);                    /* старый path */
    lua_pushstring(L, newpath);
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);                    /* package */
}

/* ---------- Запуск одного файла ---------- */

static int run_one_file(const char *dir,
                        const char *file,
                        const char *project_root) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    register_stubs(L);
    setup_package_path(L, project_root);

    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dir, file);

    if (luaL_dofile(L, path) != LUA_OK) {
        fprintf(stderr, "FAIL %s: %s\n", path, lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }
    lua_close(L);
    printf("  [OK] %s\n", file);
    return 0;
}

/* ---------- main ---------- */

int main(int argc, char **argv) {
    const char *dir          = (argc > 1) ? argv[1] : "tests/integration";
    const char *project_root = (argc > 2) ? argv[2] : ".";

    DIR *d = opendir(dir);
    if (!d) {
        perror("opendir");
        return 1;
    }

    printf("=== Интеграционные тесты Lua ===\n");
    printf("  Каталог тестов: %s\n", dir);
    printf("  Корень проекта: %s\n", project_root);

    struct dirent *ent;
    int failures = 0;
    while ((ent = readdir(d)) != NULL) {
        size_t len = strlen(ent->d_name);
        if (len > 4 && strcmp(ent->d_name + len - 4, ".lua") == 0) {
            failures += run_one_file(dir, ent->d_name, project_root);
        }
    }
    closedir(d);

    if (failures) {
        fprintf(stderr, "Провалено тестов: %d\n", failures);
        return 1;
    }
    printf("Все интеграционные тесты пройдены.\n");
    return 0;
}
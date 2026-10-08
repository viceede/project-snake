# ============================================================
# Тесты проекта:
#   - модульные (C, уровень ядра);
#   - интеграционные (Lua, уровень игровой логики).
#
# Реализация Lua находится в отдельной библиотеке lua_impl,
# она подтягивается автоматически через snake_core (PUBLIC).
# Здесь достаточно линковаться только с snake_core.
# ============================================================

# ------------------------------------------------------------
# Модульные тесты на C.
# ------------------------------------------------------------
add_executable(test_core_unit
    unit/test_main.c
    unit/test_renderer.c
    unit/test_input.c
    unit/test_api.c
)

target_include_directories(test_core_unit PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(test_core_unit PRIVATE snake_core)

add_test(NAME core_unit COMMAND test_core_unit)

# ------------------------------------------------------------
# Интеграционные тесты на Lua.
#
# runner.c обходит каталог tests/integration/*.lua и выполняет
# каждый файл. Первый аргумент — каталог с тестами,
# второй аргумент — корень проекта, чтобы runner.c мог
# выставить корректный package.path для require("scripts.*").
# ------------------------------------------------------------
add_executable(test_lua_integration
    integration/runner.c
)

target_include_directories(test_lua_integration PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(test_lua_integration PRIVATE snake_core)

add_test(
    NAME lua_integration
    COMMAND test_lua_integration
            ${CMAKE_SOURCE_DIR}/tests/integration
            ${CMAKE_SOURCE_DIR}
)
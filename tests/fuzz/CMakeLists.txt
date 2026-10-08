# ============================================================
# Фаззинг-цели проекта.
#
# Требует Clang с libFuzzer. Если компилятор не Clang —
# цели пропускаются с предупреждением.
#
# Реализация Lua находится в lua_impl и подтягивается
# автоматически через snake_core (PUBLIC). Здесь достаточно
# линковаться только с snake_core или с lua_impl.
# ============================================================

if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING "Фаззинг требует Clang. Пропуск целей фаззинга.")
    return()
endif()

# ------------------------------------------------------------
# fuzz_script_load — фаззинг загрузчика Lua-скриптов.
#
# На вход подаются произвольные байты, интерпретируемые как
# исходник Lua. Цель проверяет, что загрузчик не падает и не
# утекает память при любом входе.
#
# Файл fuzz_script_load.c НЕ определяет MINILUA_IMPLEMENTATION,
# реализация Lua линкуется через lua_impl.
# ------------------------------------------------------------
add_executable(fuzz_script_load fuzz_script_load.c)

target_include_directories(fuzz_script_load PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party
)

# snake_core тянет lua_impl через PUBLIC, но здесь нам нужна
# только сама Lua. Линкуемся с lua_impl напрямую, чтобы не
# подтягивать лишние зависимости (SDL и т. п.).
target_link_libraries(fuzz_script_load PRIVATE lua_impl)

target_compile_options(fuzz_script_load PRIVATE
    -fsanitize=fuzzer,address -g -O1
)

target_link_options(fuzz_script_load PRIVATE
    -fsanitize=fuzzer,address
)

# ------------------------------------------------------------
# fuzz_input_parser — фаззинг очереди событий ввода.
#
# Подаваемые байты интерпретируются как поток событий
# (тип + координаты). Проверяется, что очередь не переполняется
# некорректно и не содержит гонок.
# ------------------------------------------------------------
add_executable(fuzz_input_parser fuzz_input_parser.c)

target_include_directories(fuzz_input_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(fuzz_input_parser PRIVATE snake_core)

target_compile_options(fuzz_input_parser PRIVATE
    -fsanitize=fuzzer,address -g -O1
)

target_link_options(fuzz_input_parser PRIVATE
    -fsanitize=fuzzer,address
)
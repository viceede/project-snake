# ============================================================
# Фаззинг-цели проекта.
#
# Требует Clang с libFuzzer. Если компилятор не Clang —
# цели пропускаются с предупреждением.
#
# Все бинарники складываются в корень build-каталога,
# рядом с основным snake, чтобы команды запуска были
# одинаковыми локально и в CI.
# ============================================================

if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING "Фаззинг требует Clang. Пропуск целей фаззинга.")
    return()
endif()

# ------------------------------------------------------------
# fuzz_script_load — фаззинг загрузчика Lua-скриптов.
# ------------------------------------------------------------
add_executable(fuzz_script_load fuzz_script_load.c)

target_include_directories(fuzz_script_load PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(fuzz_script_load PRIVATE lua_impl)

target_compile_options(fuzz_script_load PRIVATE
    -fsanitize=fuzzer,address -g -O1
)

target_link_options(fuzz_script_load PRIVATE
    -fsanitize=fuzzer,address
)

set_target_properties(fuzz_script_load PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ------------------------------------------------------------
# fuzz_input_parser — фаззинг очереди событий ввода.
#
# ВНИМАНИЕ: имя файла — fuzz_input_parser.c (parser, не parcer).
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

set_target_properties(fuzz_input_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
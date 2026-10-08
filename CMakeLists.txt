cmake_minimum_required(VERSION 3.20)

project(snake
    VERSION 1.0.0
    DESCRIPTION "Кроссплатформенная ретро-игра Змейка"
    LANGUAGES C
)

# Стандарт C11, строгие предупреждения
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Общие флаги компиляции
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Флаги для релизной сборки (оптимизация)
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g -DDEBUG")

# ---- Опции проекта ----
option(SNAKE_BUILD_TESTS "Собирать тесты" ON)
option(SNAKE_BUILD_FUZZ  "Собирать фаззинг-цели" OFF)
option(SNAKE_ENABLE_ASAN "Включить AddressSanitizer" OFF)

if(SNAKE_ENABLE_ASAN AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# ---- Низкоуровневое ядро (статическая библиотека) ----
add_library(snake_core STATIC
    core/src/vm_bridge.c
    core/src/api_core.c
    core/src/renderer.c
    core/src/input.c
)

target_include_directories(snake_core PUBLIC
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# ---- Платформенные бэкенды и основной исполняемый файл ----
if(EMSCRIPTEN)
    message(STATUS "Сборка под Emscripten (Web/WASM)")
    add_executable(snake platforms/web/platform_em.c)
    target_include_directories(snake PRIVATE core/include)
    target_link_libraries(snake PRIVATE snake_core)
    target_link_options(snake PRIVATE
        -sUSE_SDL=2
        -sALLOW_MEMORY_GROWTH=1
        -sASYNCIFY
        --preload-file ${CMAKE_SOURCE_DIR}/scripts@/scripts
        -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']
    )
    set_target_properties(snake PROPERTIES
        SUFFIX ".html"
        OUTPUT_NAME "snake"
    )

elseif(WIN32)
    message(STATUS "Сборка под Windows (MinGW)")
    add_executable(snake platforms/windows/platform_win.c)
    target_include_directories(snake PRIVATE core/include)
    target_link_libraries(snake PRIVATE snake_core mingw32 SDL2main SDL2)

else()
    message(STATUS "Сборка под Unix/Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)

    add_executable(snake platforms/linux/platform_sdl.c)
    target_include_directories(snake PRIVATE
        core/include
        ${SDL2_INCLUDE_DIRS}
    )
    target_link_libraries(snake PRIVATE snake_core ${SDL2_LIBRARIES} m)
    target_link_directories(snake PRIVATE ${SDL2_LIBRARY_DIRS})
endif()

# Копирование Lua-скриптов рядом с исполняемым файлом (кроме Emscripten)
if(NOT EMSCRIPTEN)
    add_custom_command(TARGET snake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/scripts
                $<TARGET_FILE_DIR:snake>/scripts
        COMMENT "Копирование Lua-скриптов рядом с исполняемым файлом"
    )
endif()

# ---- Тесты ----
if(SNAKE_BUILD_TESTS AND NOT EMSCRIPTEN)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---- Фаззинг ----
if(SNAKE_BUILD_FUZZ AND NOT MSVC AND NOT EMSCRIPTEN)
    add_subdirectory(tests/fuzz)
endif()
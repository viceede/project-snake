# ============================================================
# config.mk — единый конфигурационный файл проекта Змейка.
#
# Все параметры сборки, пути к зависимостям и утилиты
# задаются здесь. Makefile и скрипты используют эти
# переменные, чтобы не дублировать логику.
#
# Файл подключается через `include config.mk` в Makefile.
# ============================================================

# ---------- Имя и версия проекта ----------
PROJECT_NAME    := snake
PROJECT_VERSION := 1.0.0

# ---------- Каталоги ----------
SRC_DIR         := .
BUILD_DIR       := build
BUILD_ASAN_DIR  := build-asan
BUILD_FUZZ_DIR  := build-fuzz
BUILD_WEB_DIR   := build-web
SCRIPTS_DIR     := scripts
THIRD_PARTY_DIR := third_party

# ---------- CMake параметры ----------
CMAKE           ?= cmake
CMAKE_GENERATOR ?=
CMAKE_BUILD_TYPE       ?= Release
CMAKE_EXTRA_FLAGS      ?=
SNAKE_BUILD_TESTS      ?= ON
SNAKE_BUILD_FUZZ       ?= OFF
SNAKE_ENABLE_ASAN      ?= OFF

# ---------- Утилиты ----------
# nproc есть не везде: на Windows через MSYS2 используем число ядер из env.
ifeq ($(OS),Windows_NT)
    NPROC := $(NUMBER_OF_PROCESSORS)
    RM_RF := rm -rf
    EXE_EXT := .exe
else
    NPROC := $(shell nproc 2>/dev/null || echo 4)
    RM_RF := rm -rf
    EXE_EXT :=
endif

# ---------- Зависимости ----------
# Минимальные версии и пакеты для каждой платформы.

# Linux / WSL
LINUX_APT_PACKAGES := \
    build-essential \
    cmake \
    git \
    libsdl2-dev \
    pkg-config \
    curl

# Только для фаззинга
LINUX_APT_FUZZ_PACKAGES := clang llvm

# Windows (MSYS2 + MinGW64)
MINGW_PACKAGES := \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-make \
    mingw-w64-x86_64-SDL2 \
    mingw-w64-x86_64-pkg-config

# Web (Emscripten)
EMSDK_DIR ?= $(HOME)/emsdk
EMSDK_ENV := $(EMSDK_DIR)/emsdk_env.sh

# ---------- Внешние зависимости ----------
MINILUA_URL := https://raw.githubusercontent.com/edubart/minilua/main/minilua.h
MINILUA_PATH := $(THIRD_PARTY_DIR)/minilua.h
MINILUA_MIN_SIZE := 100000   # минимальный размер файла в байтах

# ---------- Определение платформы ----------
UNAME_S := $(shell uname -s 2>/dev/null || echo unknown)
UNAME_M := $(shell uname -m 2>/dev/null || echo unknown)

ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
else ifeq ($(UNAME_S),Darwin)
    PLATFORM := macos
else ifeq ($(UNAME_S),MINGW64_NT-10.0)
    PLATFORM := windows
else
    PLATFORM := unknown
endif

# ---------- Локальный bin-каталог ----------
BIN := $(BUILD_DIR)/$(PROJECT_NAME)$(EXE_EXT)
TEST_CORE_BIN := $(BUILD_DIR)/tests/test_core_unit$(EXE_EXT)
TEST_LUA_BIN  := $(BUILD_DIR)/tests/test_lua_integration$(EXE_EXT)
FUZZ_SCRIPT_BIN := $(BUILD_FUZZ_DIR)/fuzz_script_load$(EXE_EXT)
FUZZ_INPUT_BIN  := $(BUILD_FUZZ_DIR)/fuzz_input_parser$(EXE_EXT)

# ---------- Цветной вывод ----------
ifeq ($(PLATFORM),linux)
    COLOR_OK  := \033[0;32m
    COLOR_ERR := \033[0;31m
    COLOR_INFO:= \033[0;36m
    COLOR_RST := \033[0m
else
    COLOR_OK  :=
    COLOR_ERR :=
    COLOR_INFO:=
    COLOR_RST :=
endif

define log_info
	@printf "$(COLOR_INFO)[info]$(COLOR_RST) %s\n" "$(1)"
endef

define log_ok
	@printf "$(COLOR_OK)[ok]$(COLOR_RST)   %s\n" "$(1)"
endef

define log_err
	@printf "$(COLOR_ERR)[err]$(COLOR_RST)  %s\n" "$(1)"
endef
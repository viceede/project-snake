# ============================================================
# Makefile — единая точка входа для сборки, тестов и запуска.
#
# Все параметры задаются в config.mk.
# Платформа определяется автоматически.
#
# Основные цели:
#   make setup          — подготовка окружения (minilua.h, проверки)
#   make deps           — установка системных зависимостей (Linux)
#   make build          — собрать игру
#   make test           — прогнать тесты (unit + integration)
#   make run            — запустить игру
#   make asan           — сборка и прогон с AddressSanitizer
#   make fuzz           — короткий прогон фаззинга
#   make web            — сборка под WebAssembly (Emscripten)
#   make web-serve      — запустить локальный HTTP-сервер для Web-версии
#   make clean          — удалить артефакты сборки
#   make distclean      — clean + удалить minilua.h
#   make help           — эта справка
# ============================================================

include config.mk

# ------- Спец-цели, не создающие файлов -------
.PHONY: help setup deps build test run asan fuzz web web-serve \
        clean distclean info

# ------- По умолчанию -------
.DEFAULT_GOAL := help

# ============================================================
# help — справка по целям
# ============================================================
help:
	@echo "Проект: $(PROJECT_NAME) $(PROJECT_VERSION)"
	@echo "Платформа: $(PLATFORM)"
	@echo ""
	@echo "Основные цели:"
	@echo "  make setup       — подготовить окружение (скачать minilua.h, проверить cmake)"
	@echo "  make deps        — установить системные зависимости (Linux/WSL)"
	@echo "  make build       — собрать игру и тесты"
	@echo "  make test        — прогнать все тесты"
	@echo "  make run         — запустить игру"
	@echo ""
	@echo "Дополнительные цели:"
	@echo "  make asan        — сборка с AddressSanitizer + UBSan и прогон тестов"
	@echo "  make fuzz        — короткий прогон фаззинга (30 сек на цель)"
	@echo "  make web         — сборка под WebAssembly через Emscripten"
	@echo "  make web-serve   — запустить локальный HTTP-сервер для Web-сборки"
	@echo "  make clean       — удалить каталоги сборки"
	@echo "  make distclean   — clean + удалить third_party/minilua.h"
	@echo "  make info        — показать текущую конфигурацию"

# ============================================================
# info — показать конфигурацию
# ============================================================
info:
	@echo "PROJECT_NAME      = $(PROJECT_NAME)"
	@echo "PROJECT_VERSION   = $(PROJECT_VERSION)"
	@echo "PLATFORM          = $(PLATFORM)"
	@echo "CMAKE             = $(CMAKE)"
	@echo "CMAKE_BUILD_TYPE  = $(CMAKE_BUILD_TYPE)"
	@echo "SNAKE_BUILD_TESTS = $(SNAKE_BUILD_TESTS)"
	@echo "SNAKE_BUILD_FUZZ  = $(SNAKE_BUILD_FUZZ)"
	@echo "SNAKE_ENABLE_ASAN = $(SNAKE_ENABLE_ASAN)"
	@echo "BUILD_DIR         = $(BUILD_DIR)"
	@echo "MINILUA_PATH      = $(MINILUA_PATH)"

# ============================================================
# setup — подготовка окружения
# ============================================================
setup: $(MINILUA_PATH)
	$(call log_info,Проверка CMake)
	@command -v $(CMAKE) >/dev/null 2>&1 || \
		{ $(call log_err,CMake не найден. Установите его: make deps); exit 1; }
	$(call log_ok,Окружение готово)
	@$(MAKE) --no-print-directory info

# Загрузка minilua.h, если файл отсутствует или слишком мал
$(MINILUA_PATH):
	$(call log_info,Скачивание minilua.h)
	@mkdir -p $(THIRD_PARTY_DIR)
	@curl -fsSL -o $(MINILUA_PATH) $(MINILUA_URL)
	@size=$$(wc -c < $(MINILUA_PATH)); \
	if [ "$$size" -lt $(MINILUA_MIN_SIZE) ]; then \
		$(call log_err,minilua.h слишком мал: $$size байт); \
		exit 1; \
	fi
	$(call log_ok,minilua.h скачан)

# ============================================================
# deps — установка системных зависимостей
# ============================================================
deps:
ifeq ($(PLATFORM),linux)
	$(call log_info,Установка пакетов через apt)
	sudo apt-get update
	sudo apt-get install -y $(LINUX_APT_PACKAGES)
	$(call log_ok,Пакеты установлены)
else ifeq ($(PLATFORM),windows)
	$(call log_info,Windows + MSYS2: убедитесь, что установлены пакеты:)
	@for p in $(MINGW_PACKAGES); do echo "    $$p"; done
	@echo "Установка: pacman -S <package>"
else
	$(call log_err,Автоматическая установка зависимостей поддерживается только для Linux/WSL)
	@exit 1
endif

# ============================================================
# build — собрать игру и тесты
# ============================================================
build: setup
	$(call log_info,Конфигурация CMake ($(CMAKE_BUILD_TYPE)))
	@$(CMAKE) -S $(SRC_DIR) -B $(BUILD_DIR) \
		$(if $(CMAKE_GENERATOR),-G "$(CMAKE_GENERATOR)") \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DSNAKE_BUILD_TESTS=$(SNAKE_BUILD_TESTS) \
		-DSNAKE_BUILD_FUZZ=$(SNAKE_BUILD_FUZZ) \
		$(CMAKE_EXTRA_FLAGS)
	$(call log_info,Сборка (-j$(NPROC)))
	@$(CMAKE) --build $(BUILD_DIR) -j$(NPROC)
	$(call log_ok,Сборка завершена: $(BIN))

# ============================================================
# test — прогнать все тесты
# ============================================================
test: build
	$(call log_info,Прогон CTest)
	@cd $(BUILD_DIR) && ctest --output-on-failure
	$(call log_ok,Все тесты пройдены)

# ============================================================
# run — запустить игру
# ============================================================
run: build
	$(call log_info,Запуск $(BIN))
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)$(EXE_EXT)

# ============================================================
# asan — сборка и тесты с AddressSanitizer + UBSan
# ============================================================
asan: setup
	$(call log_info,Конфигурация CMake с ASan/UBSan)
	@CC=clang $(CMAKE) -S $(SRC_DIR) -B $(BUILD_ASAN_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_C_COMPILER=clang \
		-DSNAKE_BUILD_TESTS=ON \
		-DSNAKE_ENABLE_ASAN=ON
	$(call log_info,Сборка (-j$(NPROC)))
	@$(CMAKE) --build $(BUILD_ASAN_DIR) -j$(NPROC)
	$(call log_info,Прогон тестов под ASan/UBSan)
	@cd $(BUILD_ASAN_DIR) && ctest --output-on-failure
	$(call log_ok,ASan/UBSan: чисто)

# ============================================================
# fuzz — короткий прогон фаззинга
# ============================================================
FUZZ_TIME ?= 30

fuzz: setup
	$(call log_info,Конфигурация CMake для фаззинга (Clang))
	@CC=clang $(CMAKE) -S $(SRC_DIR) -B $(BUILD_FUZZ_DIR) \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_C_COMPILER=clang \
		-DSNAKE_BUILD_FUZZ=ON \
		-DSNAKE_BUILD_TESTS=OFF
	$(call log_info,Сборка фаззинг-целей)
	@$(CMAKE) --build $(BUILD_FUZZ_DIR) -j$(NPROC)
	$(call log_info,Фаззинг fuzz_script_load ($(FUZZ_TIME) сек))
	@$(FUZZ_SCRIPT_BIN) -max_total_time=$(FUZZ_TIME)
	$(call log_info,Фаззинг fuzz_input_parser ($(FUZZ_TIME) сек))
	@$(FUZZ_INPUT_BIN) -max_total_time=$(FUZZ_TIME)
	$(call log_ok,Фаззинг завершён без крашей)

# ============================================================
# web — сборка под WebAssembly через Emscripten
# ============================================================
web: setup
	$(call log_info,Загрузка окружения Emscripten из $(EMSDK_ENV))
	@test -f $(EMSDK_ENV) || \
		{ $(call log_err,Emscripten не найден. Установите emsdk в $(EMSDK_DIR)); exit 1; }
	@bash -c "source $(EMSDK_ENV) && \
		emcmake $(CMAKE) -S $(SRC_DIR) -B $(BUILD_WEB_DIR) \
			-DCMAKE_BUILD_TYPE=Release && \
		$(CMAKE) --build $(BUILD_WEB_DIR) -j$(NPROC)"
	$(call log_ok,Web-сборка: $(BUILD_WEB_DIR)/$(PROJECT_NAME).html)

web-serve: web
	$(call log_info,HTTP-сервер на http://localhost:8000/$(PROJECT_NAME).html)
	@cd $(BUILD_WEB_DIR) && python3 -m http.server 8000

# ============================================================
# clean / distclean
# ============================================================
clean:
	$(call log_info,Удаление каталогов сборки)
	@$(RM_RF) $(BUILD_DIR) $(BUILD_ASAN_DIR) $(BUILD_FUZZ_DIR) $(BUILD_WEB_DIR)
	$(call log_ok,Готово)

distclean: clean
	$(call log_info,Удаление скачанных зависимостей)
	@$(RM_RF) $(MINILUA_PATH)
	$(call log_ok,Готово)
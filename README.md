# Змейка — кроссплатформенная ретро-игра

Семестровый проект по курсу РКПС.
Реализация классической игры «Змейка» с низкоуровневым ядром на C
и платформо-независимой игровой логикой на Lua.

## Архитектура

```
┌──────────────────────────────────┐
│   Игровая логика (Lua)           │
│   scripts/*.lua                  │
├──────────────────────────────────┤
│   Мост C ↔ Lua (C API)           │
│   core/src/vm_bridge.c           │
├──────────────────────────────────┤
│   Платформо-независимое ядро (C) │
│   renderer.c, input.c, api_core  │
├──────────────────────────────────┤
│   Платформенный бэкенд           │
│   SDL2 / Emscripten / Win32      │
└──────────────────────────────────┘
```

- **Ядро на C**: рендеринг, ввод, таймеры, PRNG — без игровой логики.
- **Игровая логика на Lua**: правила, состояния, коллизии — полностью
  переносима и легко тестируется.
- **Платформенные бэкенды**: SDL2 (Linux/WSL/Windows),
  Emscripten (WebAssembly для браузера).

## Поддерживаемые платформы

| Платформа | Процессор | Графика | Ввод | Статус |
|---|---|---|---|---|
| WSL / Linux | x86_64 | SDL2 | клавиатура | ✅ |
| Windows (MinGW) | x86_64 | SDL2 | клавиатура | ✅ |
| Web (WASM) | wasm32 | SDL2 Canvas | клавиатура/тач | ✅ |
| MIPS (big-endian) | mips32 | SDL2 | клавиатура | 🧪 |

## Зависимости

- GCC или Clang (C11)
- CMake ≥ 3.20
- SDL2 (для Linux/Windows)
- Emscripten (опционально, для Web)
- Clang с libFuzzer (опционально, для фаззинга)

Установка на WSL/Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential cmake git \
                 libsdl2-dev libsdl2-ttf-dev \
                 clang llvm valgrind
```

## Сборка и запуск

### Linux / WSL

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/snake
```

### Windows (MinGW + MSYS2)

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/snake.exe
```

### Web (Emscripten)

```bash
emcmake cmake -S . -B build-web
cmake --build build-web
# Открыть build-web/snake.html в браузере через локальный сервер
python3 -m http.server -d build-web 8000
```

## Управление

| Клавиша | Действие |
|---|---|
| ← ↑ → ↓ | Изменение направления |
| W A S D | Альтернативное управление |
| Пробел | Пауза / продолжить |
| R | Начать заново |
| Esc | Выход |

## Тестирование

```bash
# Сборка с тестами
cmake -S . -B build -DSNAKE_BUILD_TESTS=ON
cmake --build build

# Запуск модульных и интеграционных тестов
cd build && ctest --output-on-failure
```

### Фаззинг

```bash
cmake -S . -B build-fuzz -DSNAKE_BUILD_FUZZ=ON \
      -DCMAKE_C_COMPILER=clang
cmake --build build-fuzz
./build-fuzz/fuzz_script_load -max_total_time=60
```

## Структура репозитория

```
.
├── core/            # Низкоуровневое ядро на C
├── scripts/         # Игровая логика на Lua
├── platforms/       # Платформенные бэкенды
├── tests/           # Тесты (unit, integration, fuzz)
├── third_party/     # minilua.h
├── CMakeLists.txt
├── .github/workflows/ci.yml
└── README.md
```

## Лицензия

MIT License — см. файл `LICENSE`.
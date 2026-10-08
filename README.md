# Змейка — кроссплатформенная ретро-игра

Семестровый проект по курсу РКПС.
Классическая «Змейка» с низкоуровневым ядром на **C** и платформо-независимой
игровой логикой на **Lua**.

---

## Что это за проект

Игра разделена на три слоя:

- **Ядро на C** (`core/`) — рендеринг в собственный framebuffer, очередь
  событий ввода, таймеры, генератор случайных чисел, встроенный шрифт 5×7
  и мост C ↔ Lua. Никаких игровых правил здесь нет.
- **Игровая логика на Lua** (`scripts/`) — правила игры, состояние змейки,
  коллизии, отрисовка игровых объектов. Полностью переносима и легко
  тестируется.
- **Платформенные бэкенды** (`platforms/`) — тонкий слой, который создаёт
  окно, читает ввод и отправляет содержимое framebuffer на экран.

Такое разделение позволяет запускать одну и ту же игру на Linux, Windows
и в браузере, меняя только бэкенд.

---

## Схема архитектуры

```
┌──────────────────────────────────┐
│   Игровая логика (Lua)           │
│   scripts/*.lua                  │
├──────────────────────────────────┤
│   Мост C ↔ Lua (Lua C API)       │
│   core/src/vm_bridge.c           │
├──────────────────────────────────┤
│   Платформо-независимое ядро (C) │
│   renderer.c, input.c, timing.c  │
├──────────────────────────────────┤
│   Платформенный бэкенд           │
│   SDL2 / Emscripten / Win32      │
└──────────────────────────────────┘
```

---

## Поддерживаемые платформы

| Платформа       | Процессор | Графика      | Ввод             | Статус   |
|-----------------|-----------|--------------|------------------|----------|
| WSL / Linux     | x86_64    | окно SDL2    | клавиатура       | работает |
| Windows (MinGW) | x86_64    | окно SDL2    | клавиатура       | работает |
| Web (WASM)      | wasm32    | SDL2 Canvas  | клавиатура, тач  | работает |

Проект использует три процессора с разной разрядностью (x86_64 и wasm32),
три категории ОС (Unix, Windows, Web) и три способа ввода
(клавиатура, мышь, касание).

---

## Требования

- GCC или Clang с поддержкой C11
- CMake ≥ 3.20
- SDL2
- Python 3 (для локального запуска Web-версии)
- Emscripten (опционально, для сборки под Web)

### Установка на WSL / Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake git \
                 libsdl2-dev pkg-config
```

### Установка на Windows (MSYS2 + MinGW)

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-SDL2 \
          make
```

### Установка Emscripten

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

---

## Подготовка репозитория

Перед первой сборкой нужно скачать `minilua.h` — однофайловую сборку Lua,
которая используется как скриптовый движок.

```bash
mkdir -p third_party
curl -L -o third_party/minilua.h \
  https://raw.githubusercontent.com/edubart/minilua/main/minilua.h
```

Проверьте, что файл скачался:

```bash
wc -c third_party/minilua.h     # ожидается ~900 000 байт
```

---

## Сборка и запуск

### Linux / WSL

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSNAKE_BUILD_TESTS=ON
cmake --build build -j$(nproc)
./build/snake
```

Скрипты из `scripts/` автоматически копируются рядом с бинарником
при сборке, отдельно ничего копировать не нужно.

Если окно не открывается, проверьте переменную окружения `DISPLAY`:

```bash
echo $DISPLAY      # должно быть что-то вроде :0 или :1
```

На Windows 11 с WSL2 работает WSLg — окно откроется сразу. На Windows 10
потребуется X-сервер (VcXsrv, X410) и переменная `DISPLAY`:

```bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
```

### Windows (MSYS2 + MinGW)

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/snake.exe
```

Требуется, чтобы в `PATH` находились `mingw32-make`, `gcc` и `SDL2.dll`.
Обычно `SDL2.dll` лежит в `C:\msys64\mingw64\bin\` — скопируйте её рядом
с `snake.exe` или добавьте каталог в `PATH`.

### Web (Emscripten)

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web -j$(nproc)

# Запуск локального HTTP-сервера
python3 -m http.server -d build-web 8000
```

Откройте в браузере <http://localhost:8000/snake.html>.
Веб-версия поддерживает клавиатуру и касания (для мобильных устройств).

---

## Управление

| Клавиша    | Действие                          |
|------------|-----------------------------------|
| ← ↑ → ↓    | Изменение направления движения    |
| W A S D    | Альтернативное управление         |
| Пробел     | Пауза / продолжить                |
| R          | Начать заново после Game Over     |
| Esc        | Выход                             |

---

## Тестирование

### Быстрый прогон всех тестов

```bash
cd build
ctest --output-on-failure
```

Ожидаемый результат:

```
    Start 1: core_unit
1/2 Test #1: core_unit ....................   Passed
    Start 2: lua_integration
2/2 Test #2: lua_integration ..............   Passed

100% tests passed, 0 tests failed out of 2
```

### Модульные тесты (C)

Проверяют работу framebuffer, очереди событий и C-API, экспортируемого
в Lua: корректную обрезку прямоугольников, устойчивость к отрицательным
размерам, диапазон `random(n)`, поведение `random(0)`.

```bash
./build/tests/test_core_unit
```

### Интеграционные тесты (Lua)

Проверяют игровую логику: движение змейки, коллизии со стеной и с собой,
запрет разворота на 180°, а также свойства (все сегменты змейки уникальны,
длина монотонно не убывает при росте).

```bash
./build/tests/test_lua_integration \
    "$(pwd)/tests/integration" \
    "$(pwd)"
```

### Сборка с санитайзерами

Для отлова утечек памяти и неопределённого поведения:

```bash
CC=clang cmake -S . -B build-asan \
    -DCMAKE_BUILD_TYPE=Debug \
    -DSNAKE_BUILD_TESTS=ON \
    -DSNAKE_ENABLE_ASAN=ON
cmake --build build-asan -j$(nproc)
(cd build-asan && ctest --output-on-failure)
```

### Фаззинг (опционально)

Требуется Clang с libFuzzer. Проверяет устойчивость загрузчика Lua-скриптов
и парсера входных событий к произвольным данным.

```bash
CC=clang cmake -S . -B build-fuzz \
    -DSNAKE_BUILD_FUZZ=ON \
    -DSNAKE_BUILD_TESTS=OFF \
    -DCMAKE_C_COMPILER=clang
cmake --build build-fuzz -j$(nproc)

./build-fuzz/fuzz_script_load  -max_total_time=60
./build-fuzz/fuzz_input_parser -max_total_time=60
```

---

## Структура репозитория

```
.
├── core/                  # Низкоуровневое ядро на C
│   ├── include/core.h     # Публичный API ядра
│   └── src/
│       ├── lua_impl.c     # Реализация Lua (единственный TU)
│       ├── vm_bridge.c    # Мост C ↔ Lua
│       ├── api_core.c     # C-функции, вызываемые из Lua
│       ├── renderer.c     # Framebuffer, примитивы рисования
│       ├── input.c        # Очередь событий ввода
│       ├── timing.c       # Таймеры и PRNG
│       └── font.c         # Встроенный шрифт 5×7
├── scripts/               # Игровая логика на Lua
│   ├── main.lua           # Точка входа: update/render/on_event
│   ├── game.lua           # Конечный автомат игры
│   ├── snake.lua          # Модель змейки
│   ├── board.lua          # Игровое поле и еда
│   └── ui.lua             # Отрисовка интерфейса
├── platforms/             # Платформенные бэкенды
│   ├── linux/platform_sdl.c
│   ├── windows/platform_win.c
│   └── web/platform_em.c
├── tests/
│   ├── unit/              # Модульные тесты на C
│   ├── integration/       # Интеграционные и property-based тесты на Lua
│   └── fuzz/              # Цели для фаззинга
├── third_party/
│   └── minilua.h          # Однофайловая сборка Lua (скачивается отдельно)
├── .github/workflows/
│   └── ci.yml             # CI: Linux + Windows + ASan + фаззинг
├── CMakeLists.txt
└── README.md
```

---

## Очистка

```bash
rm -rf build build-asan build-fuzz build-web
```

---

## Лицензия

MIT License — см. файл `LICENSE`.
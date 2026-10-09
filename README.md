# Змейка — кроссплатформенная ретро-игра

[![CI](https://github.com/viceede/project-snake/actions/workflows/ci.yml/badge.svg)](https://github.com/viceede/project-snake/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/viceede/project-snake)](https://github.com/viceede/project-snake/releases/latest)

Семестровый проект по курсу «Архитектура вычислительных систем».
Классическая «Змейка» с низкоуровневым ядром на **C** и платформо-независимой
игровой логикой на **Lua**.

---

## Что это за проект

Игра разделена на три слоя:

- **Ядро на C** (`core/`) — рендеринг в собственный framebuffer, очередь
  событий ввода, таймеры, генератор случайных чисел, встроенный шрифт 5×7
  и мост C ↔ Lua. Никаких игровых правил здесь нет.
- **Игровая логика на Lua** (`scripts/`) — правила игры, состояние змейки,
  коллизии, отрисовка игровых объектов, конечный автомат состояний
  (меню → игра → пауза → конец игры). Полностью переносима и легко
  тестируется.
- **Платформенные бэкенды** (`platforms/`) — тонкий слой, который создаёт
  окно, читает ввод и отправляет содержимое framebuffer на экран.

Такое разделение позволяет запускать одну и ту же игру на Linux, Windows
и в браузере, меняя только бэкенд.

---

## Готовые сборки

Если не хотите собирать игру из исходников, скачайте готовый бинарник
на странице [Releases](https://github.com/viceede/project-snake/releases/latest).

| Платформа | Файл | Что внутри |
|---|---|---|
| Linux (x86_64) | `snake-linux-x86_64.tar.gz` | бинарник и `scripts/` |
| Windows (x86_64) | `snake-windows-x86_64.zip` | `.exe`, `SDL2.dll`, `scripts/` |

**Linux:**

```bash
tar xzf snake-linux-x86_64.tar.gz
./snake-linux-x86_64
```

Требуется установленный SDL2:

```bash
sudo apt install libsdl2-2.0-0
```

**Windows:**

Распакуйте `snake-windows-x86_64.zip` и запустите
`snake-windows-x86_64.exe`. `SDL2.dll` уже лежит рядом — дополнительно
ничего ставить не нужно.

---

## Игровой процесс

При запуске показывается стартовое меню:

```
              SNAKE
           A RETRO GAME
         PRESS R TO START
```

После нажатия `R` начинается партия: змейка двигается по полю,
собирает еду и растёт. При столкновении со стеной или собственным
телом партия заканчивается — показывается `GAME OVER` и подсказка
`PRESS R TO RESTART`. Клавиша `Пробел` ставит игру на паузу.

### Состояния игры

| Состояние   | Как попасть                 | Как выйти                            |
|-------------|-----------------------------|--------------------------------------|
| `menu`      | сразу после запуска         | `R` — начать игру, `Esc` — выход     |
| `playing`   | `R` из меню или Game Over   | `Пробел` — пауза, столкновение — game over |
| `paused`    | `Пробел` во время игры      | `Пробел` — продолжить, `R` — перезапуск |
| `game_over` | столкновение со стеной/телом| `R` — начать заново                  |

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

## Быстрый старт

Все платформы поддерживают единый интерфейс через `make`:

```bash
make setup      # подготовка окружения (скачать minilua.h)
make build      # собрать игру и тесты
make test       # прогнать тесты
make run        # запустить игру
```

Первая сборка на чистом клоне:

```bash
git clone https://github.com/viceede/project-snake.git
cd project-snake
make deps       # установить системные зависимости (Linux/WSL)
make build
make run
```

Вся конфигурация проекта вынесена в `config.mk`: пути, имена пакетов,
URL для `minilua.h`, каталоги сборки, список алиасов.

---

## Требования

- GCC или Clang с поддержкой C11
- CMake ≥ 3.20
- GNU Make
- SDL2
- curl (для скачивания `minilua.h`)
- Python 3 (для локального запуска Web-версии)
- Emscripten (опционально, для сборки под Web)

### Установка на WSL / Ubuntu / Debian

Автоматически одной командой:

```bash
make deps
```

Вручную — если `make` ещё не установлен:

```bash
sudo apt update
sudo apt install build-essential cmake git \
                 libsdl2-dev pkg-config curl
```

Для фаззинга дополнительно:

```bash
sudo apt install clang llvm
```

### Установка на Windows (MSYS2 + MinGW64)

Откройте оболочку **MSYS2 MINGW64** и установите пакеты:

```bash
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-make \
          mingw-w64-x86_64-SDL2 \
          mingw-w64-x86_64-pkg-config \
          make
```

`make deps` на Windows не устанавливает пакеты автоматически — она лишь
печатает список необходимых. Установка через `pacman` выполняется вручную.

### Установка Emscripten

```bash
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
cd ~/emsdk
./emsdk install latest
./emsdk activate latest
```

Инициализацию `emsdk_env.sh` выполнять вручную не нужно — цель `make web`
загружает окружение автоматически.

---

## Команды Makefile

| Команда          | Что делает                                              |
|------------------|---------------------------------------------------------|
| `make help`      | Справка по всем целям                                   |
| `make info`      | Показать текущую конфигурацию                           |
| `make setup`     | Скачать `minilua.h`, проверить CMake                    |
| `make deps`      | Установить системные зависимости (Linux / WSL)          |
| `make build`     | Собрать игру и тесты                                    |
| `make test`      | Прогнать все тесты через CTest                          |
| `make run`       | Запустить игру                                          |
| `make asan`      | Сборка с AddressSanitizer + UBSan и прогон тестов       |
| `make fuzz`      | Короткий прогон фаззинга (30 сек на цель)               |
| `make web`       | Сборка под WebAssembly через Emscripten                 |
| `make web-serve` | Локальный HTTP-сервер для Web-сборки на порту 8000      |
| `make clean`     | Удалить каталоги сборки                                 |
| `make distclean` | `clean` + удалить `third_party/minilua.h`               |

Переопределение параметров «на лету»:

```bash
make build CMAKE_BUILD_TYPE=Debug
make test  CMAKE_BUILD_TYPE=Debug
make fuzz  FUZZ_TIME=120
make web   EMSDK_DIR=/opt/emsdk
```

---

## Сборка и запуск по платформам

### Linux / WSL

```bash
make deps       # один раз: установит apt-пакеты
make setup      # скачает minilua.h (если ещё не скачан)
make build
make test
make run
```

Если окно не открывается, проверьте переменную окружения `DISPLAY`:

```bash
echo $DISPLAY      # должно быть что-то вроде :0 или :1
```

На Windows 11 с WSL2 работает WSLg — окно откроется сразу.
На Windows 10 потребуется X-сервер (VcXsrv, X410):

```bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
make run
```

### Windows (MSYS2 + MinGW64)

Убедитесь, что запущена оболочка **MSYS2 MINGW64** (не MSYS, не UCRT64).
Пакеты ставятся через `pacman` (см. раздел «Установка» выше), затем:

```bash
make build
make test
make run
```

Требуется, чтобы `SDL2.dll` была доступна в `PATH`. Обычно она лежит в
`C:\msys64\mingw64\bin\` — эта папка уже в `PATH` при активном MINGW64.

### Web (Emscripten)

```bash
make web            # соберёт build-web/snake.html
make web-serve      # запустит сервер на http://localhost:8000
```

Откройте в браузере <http://localhost:8000/snake.html>.
Веб-версия поддерживает клавиатуру и касания (для мобильных устройств).

Если `emsdk` установлен в нестандартное место:

```bash
make web EMSDK_DIR=/opt/emsdk
```

---

## Управление

| Клавиша    | Действие                                            | Когда работает |
|------------|-----------------------------------------------------|----------------|
| R          | Начать игру / перезапустить после Game Over         | menu, paused, game_over |
| Стрелки    | Изменение направления движения                      | playing |
| W A S D    | Альтернативное управление                           | playing |
| Пробел     | Пауза / продолжить                                  | playing, paused |
| Esc        | Выход                                               | в любом состоянии |

---

## Тестирование

### Быстрый прогон всех тестов

```bash
make test
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

Проверяют игровую логику:

- стартовое состояние — `menu`;
- движение змейки и рост при поедании еды;
- коллизии со стеной и собственным телом;
- запрет разворота на 180°;
- запрет смены направления в состоянии `menu`;
- свойства (все сегменты змейки уникальны, длина монотонно не убывает
  при росте).

```bash
./build/tests/test_lua_integration \
    "$(pwd)/tests/integration" \
    "$(pwd)"
```

### Сборка с санитайзерами

```bash
make asan
```

Цель собирает проект в `build-asan/` с Clang и флагами
`-fsanitize=address,undefined`, после чего автоматически прогоняет тесты.

### Фаззинг (опционально)

Требуется Clang с libFuzzer.

```bash
make fuzz                 # 30 секунд на каждую цель
make fuzz FUZZ_TIME=120   # 2 минуты на каждую цель
```

Цель собирает `fuzz_script_load` (устойчивость загрузчика Lua-скриптов
к произвольным данным) и `fuzz_input_parser` (устойчивость очереди
событий к произвольному потоку байтов).

---

## Непрерывная интеграция и релизы

При каждом пуше в `main` запускаются четыре workflow в GitHub Actions:

- **Linux (GCC)** — сборка и прогон тестов;
- **Linux (Clang + ASan/UBSan)** — проверка на утечки и UB;
- **Windows (MinGW)** — кросс-платформенная сборка и тесты;
- **Fuzzing** — короткий прогон libFuzzer.

При пуше тега вида `v*` (например, `v1.0.0`) запускается workflow
**Release**, который собирает бинарники для Linux и Windows
и публикует их на странице **Releases** с описанием и инструкциями
по запуску. Тот же workflow можно запустить вручную: **Actions** →
**Release** → **Run workflow**.

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
│   ├── game.lua           # Конечный автомат (menu/playing/paused/game_over)
│   ├── snake.lua          # Модель змейки
│   ├── board.lua          # Игровое поле и еда
│   └── ui.lua             # Отрисовка меню, поля, счёта и сообщений
├── platforms/             # Платформенные бэкенды
│   ├── linux/platform_sdl.c
│   ├── windows/platform_win.c
│   └── web/platform_em.c
├── tests/
│   ├── unit/              # Модульные тесты на C
│   ├── integration/       # Интеграционные и property-based тесты на Lua
│   └── fuzz/              # Цели для фаззинга
├── third_party/
│   └── minilua.h          # Однофайловая сборка Lua (скачивается make setup)
├── .github/workflows/
│   ├── ci.yml             # CI: Linux + Windows + ASan + фаззинг
│   └── release.yml        # Публикация релизов по тегу v*
├── CMakeLists.txt         # CMake-описание сборки
├── Makefile               # Единая точка входа с алиасами
├── config.mk              # Конфигурация: пути, пакеты, флаги
└── README.md
```

---

## Очистка

```bash
make clean        # удалить каталоги build/, build-asan/, build-fuzz/, build-web/
make distclean    # make clean + удалить third_party/minilua.h
```

---

## Лицензия

MIT License — см. файл `LICENSE`.
# Сторонние зависимости

## minilua.h

Однофайловая встраиваемая сборка Lua (в текущей версии — Lua 5.5.0).

Источник: <https://github.com/edubart/minilua>
Лицензия: MIT (та же, что и у Lua).

### Скачивание

Файл `minilua.h` **не хранится в репозитории** — его нужно скачать
перед первой сборкой:

```bash
mkdir -p third_party
curl -L -o third_party/minilua.h \
  https://raw.githubusercontent.com/edubart/minilua/main/minilua.h
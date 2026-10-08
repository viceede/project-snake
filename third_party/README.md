# Сторонние зависимости

## minilua.h

Однофайловая встраиваемая сборка Lua 5.4.
Источник: https://github.com/edubart/minilua
Лицензия: MIT (та же, что и у Lua).

Файл `minilua.h` не хранится в репозитории — его нужно скачать
перед первой сборкой:

```bash
mkdir -p third_party
curl -L -o third_party/minilua.h \
  https://raw.githubusercontent.com/edubart/minilua/main/minilua.h
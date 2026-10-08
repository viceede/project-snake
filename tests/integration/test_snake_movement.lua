-- test_snake_movement.lua — проверяем базовое движение змейки.

package.path = "./?.lua;" .. package.path
local Snake = require("scripts.snake")

-- Инициализация
local s = Snake.new(5, 5)
assert(s:length() == 3, "начальная длина 3")
assert(s:head_x() == 5 and s:head_y() == 5, "начальная голова (5,5)")

-- Движение без роста
s:move(6, 5, false)
assert(s:head_x() == 6, "новая голова по X")
assert(s:length() == 3, "длина не изменилась")
assert(not s:occupies(2, 5), "хвост исчез")

-- Движение с ростом
s:move(7, 5, true)
assert(s:length() == 4, "длина увеличилась")
assert(s:occupies(6, 5), "старая голова стала сегментом")

print("OK: snake movement")
-- test_collision.lua — проверяем столкновения.

package.path = "./?.lua;" .. package.path
local Game = require("scripts.game")

-- Тест 1: столкновение со стеной
local g = Game.new(10, 10)
g.tick_interval = 0  -- ускоряем время
for i = 1, 20 do g:update(1.0) end
assert(g.state == "game_over", "столкновение со стеной")

-- Тест 2: разворот на 180° запрещён
local g2 = Game.new(20, 20)
g2:set_direction(EV_LEFT)   -- изначально движемся вправо
assert(g2.next_dir.x == 1, "разворот не применён")

-- Тест 3: корректная смена направления
g2:set_direction(EV_UP)
assert(g2.next_dir.x == 0 and g2.next_dir.y == -1, "направление вверх")

print("OK: collision and direction")
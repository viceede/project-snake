-- test_collision.lua — проверяем столкновения и работу состояний.

package.path = "./?.lua;" .. package.path
local Game = require("scripts.game")

-- Тест 1: стартовое состояние — menu
local g0 = Game.new(10, 10)
assert(g0.state == "menu", "стартовое состояние — menu")

-- Тест 2: столкновение со стеной
local g = Game.new(10, 10)
g:start()                -- выходим из меню
g.tick_interval = 0      -- ускоряем время
for i = 1, 20 do g:update(1.0) end
assert(g.state == "game_over", "столкновение со стеной")

-- Тест 3: разворот на 180° запрещён
local g2 = Game.new(20, 20)
g2:start()
g2:set_direction(EV_LEFT)   -- изначально движемся вправо
assert(g2.next_dir.x == 1, "разворот не применён")

-- Тест 4: корректная смена направления
g2:set_direction(EV_UP)
assert(g2.next_dir.x == 0 and g2.next_dir.y == -1, "направление вверх")

-- Тест 5: в меню направление не меняется
local g3 = Game.new(20, 20)
assert(g3.state == "menu", "стартовое состояние — menu")
g3:set_direction(EV_UP)
assert(g3.next_dir.x == 1 and g3.next_dir.y == 0,
       "в меню направление не меняется")

print("OK: collision, direction and menu state")
-- test_property.lua — property-based тесты для змейки.

package.path = "./?.lua;" .. package.path
local Snake = require("scripts.snake")

-- Свойство 1: все сегменты змейки уникальны
local function unique_segments(s)
    local seen = {}
    for _, seg in ipairs(s.body) do
        local key = seg.x .. "," .. seg.y
        if seen[key] then return false end
        seen[key] = true
    end
    return true
end

-- Свойство 2: длина не убывает при росте
local function grow_monotonic()
    local s = Snake.new(5, 5)
    local prev = s:length()
    for i = 1, 20 do
        s:move(5 + i, 5, true)
        local cur = s:length()
        if cur < prev then return false end
        prev = cur
    end
    return true
end

-- Свойство 3: при отсутствии роста длина сохраняется
local function move_preserves_length()
    local s = Snake.new(10, 10)
    local len = s:length()
    for i = 1, 50 do
        s:move(10 + i, 10, false)
        if s:length() ~= len then return false end
    end
    return true
end

-- Генерируем 100 случайных последовательностей движений
-- и проверяем инварианты.
math.randomseed(12345)
for trial = 1, 100 do
    local s = Snake.new(0, 0)
    for step = 1, 30 do
        local nx = math.random(-100, 100)
        local ny = math.random(-100, 100)
        s:move(nx, ny, math.random(0, 1) == 1)
        assert(unique_segments(s), "уникальность сегментов нарушена")
    end
end

assert(grow_monotonic(), "монотонный рост")
assert(move_preserves_length(), "сохранение длины")

print("OK: property-based tests")
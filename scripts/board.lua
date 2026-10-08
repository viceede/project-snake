-- board.lua — модель игрового поля и еды.

local Board = {}
Board.__index = Board

function Board.new(w, h)
    local self = setmetatable({}, Board)
    self.w = w
    self.h = h
    self.food = nil
    return self
end

function Board:set_food(x, y)
    self.food = {x = x, y = y}
end

function Board:has_food(x, y)
    return self.food and self.food.x == x and self.food.y == y
end

return Board
-- snake.lua — модель змейки: сегменты, голова, хвост.

local Snake = {}
Snake.__index = Snake

function Snake.new(x, y)
    local self = setmetatable({}, Snake)
    -- Тело: от головы к хвосту
    self.body = {
        {x = x,     y = y},
        {x = x - 1, y = y},
        {x = x - 2, y = y},
    }
    return self
end

function Snake:head_x() return self.body[1].x end
function Snake:head_y() return self.body[1].y end
function Snake:length() return #self.body end

function Snake:occupies(x, y)
    for _, seg in ipairs(self.body) do
        if seg.x == x and seg.y == y then return true end
    end
    return false
end

function Snake:move(new_x, new_y, grow)
    table.insert(self.body, 1, {x = new_x, y = new_y})
    if not grow then
        table.remove(self.body)
    end
end

return Snake
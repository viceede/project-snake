-- game.lua — конечный автомат игры, движение змейки, коллизии.

local Snake = require("scripts.snake")
local Board = require("scripts.board")

local Game = {}
Game.__index = Game

-- Константы состояний
local STATE_PLAYING  = "playing"
local STATE_PAUSED   = "paused"
local STATE_GAME_OVER = "game_over"

function Game.new(w, h)
    local self = setmetatable({}, Game)
    self.board_w = w
    self.board_h = h
    self:reset()
    return self
end

function Game:reset()
    self.board = Board.new(self.board_w, self.board_h)
    self.snake = Snake.new(math.floor(self.board_w / 2),
                           math.floor(self.board_h / 2))
    self.dir = { x = 1, y = 0 }
    self.next_dir = { x = 1, y = 0 }
    self.score = 0
    self.state = STATE_PLAYING
    self.timer = 0
    self.tick_interval = 0.15
    self:spawn_food()
end

function Game:toggle_pause()
    if self.state == STATE_PLAYING then
        self.state = STATE_PAUSED
    elseif self.state == STATE_PAUSED then
        self.state = STATE_PLAYING
    end
end

function Game:spawn_food()
    local x, y
    repeat
        x = random(self.board_w)
        y = random(self.board_h)
    until not self.snake:occupies(x, y)
    self.board:set_food(x, y)
end

function Game:set_direction(ev_type)
    local d
    if ev_type == EV_UP    then d = {x = 0, y = -1}
    elseif ev_type == EV_DOWN  then d = {x = 0, y = 1}
    elseif ev_type == EV_LEFT  then d = {x = -1, y = 0}
    elseif ev_type == EV_RIGHT then d = {x = 1, y = 0}
    end
    if not d then return end

    -- Запрет разворота на 180°: новое направление не должно
    -- быть противоположно текущему.
    if d.x == -self.dir.x and d.y == -self.dir.y then
        return
    end
    self.next_dir = d
end

function Game:update(dt)
    if self.state ~= STATE_PLAYING then return end

    self.timer = self.timer + dt
    if self.timer < self.tick_interval then return end
    self.timer = 0

    -- Применяем отложенное направление
    self.dir = self.next_dir

    -- Новая позиция головы
    local hx = self.snake:head_x() + self.dir.x
    local hy = self.snake:head_y() + self.dir.y

    -- Столкновение со стеной
    if hx < 0 or hx >= self.board_w or
       hy < 0 or hy >= self.board_h then
        self.state = STATE_GAME_OVER
        return
    end

    -- Столкновение с собственным телом
    if self.snake:occupies(hx, hy) then
        self.state = STATE_GAME_OVER
        return
    end

    local ate = self.board:has_food(hx, hy)
    self.snake:move(hx, hy, ate)

    if ate then
        self.score = self.score + 1
        self:spawn_food()
    end
end

return Game
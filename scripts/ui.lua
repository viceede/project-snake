-- ui.lua — отрисовка игрового поля, счётчика и сообщений.

local UI = {}
UI.__index = UI

-- Цвета
local COLOR_BG         = 0x101820
local COLOR_GRID       = 0x182028
local COLOR_SNAKE_HEAD = 0x66FF66
local COLOR_SNAKE_BODY = 0x33AA33
local COLOR_FOOD       = 0xFF4040
local COLOR_TEXT       = 0xFFFFFF
local COLOR_OVERLAY    = 0x000000

function UI.new(board_w, board_h, cell)
    local self = setmetatable({}, UI)
    self.board_w = board_w
    self.board_h = board_h
    self.cell = cell
    self.margin = 40
    return self
end

function UI:draw(game)
    local W = self.board_w * self.cell
    local H = self.board_h * self.cell

    -- Фон
    clear(COLOR_BG)

    -- Сетка
    for i = 0, self.board_w do
        draw_rect(self.margin + i * self.cell, self.margin,
                  1, H, COLOR_GRID)
    end
    for j = 0, self.board_h do
        draw_rect(self.margin, self.margin + j * self.cell,
                  W, 1, COLOR_GRID)
    end

    -- Еда
    if game.board.food then
        local f = game.board.food
        draw_rect(self.margin + f.x * self.cell + 2,
                  self.margin + f.y * self.cell + 2,
                  self.cell - 4, self.cell - 4, COLOR_FOOD)
    end

    -- Змейка
    for i, seg in ipairs(game.snake.body) do
        local c = (i == 1) and COLOR_SNAKE_HEAD or COLOR_SNAKE_BODY
        draw_rect(self.margin + seg.x * self.cell + 1,
                  self.margin + seg.y * self.cell + 1,
                  self.cell - 2, self.cell - 2, c)
    end

    -- Счёт
    draw_text(self.margin, 10, "SCORE " .. game.score, COLOR_TEXT)

    -- Сообщения
    if game.state == "game_over" then
        draw_text(W / 2 - 40, H / 2, "GAME OVER", COLOR_TEXT)
        draw_text(W / 2 - 40, H / 2 + 20, "PRESS R", COLOR_TEXT)
    elseif game.state == "paused" then
        draw_text(W / 2 - 30, H / 2, "PAUSED", COLOR_TEXT)
    end
end

return UI
-- ui.lua — отрисовка игрового поля, счётчика и сообщений.

local Game = require("scripts.game")

local UI = {}
UI.__index = UI

-- Цвета
local COLOR_BG         = 0x101820
local COLOR_GRID       = 0x182028
local COLOR_SNAKE_HEAD = 0x66FF66
local COLOR_SNAKE_BODY = 0x33AA33
local COLOR_FOOD       = 0xFF4040
local COLOR_TEXT       = 0xFFFFFF
local COLOR_HINT       = 0x88AA88
local COLOR_TITLE      = 0xFFCC44

function UI.new(board_w, board_h, cell)
    local self = setmetatable({}, UI)
    self.board_w = board_w
    self.board_h = board_h
    self.cell = cell
    self.margin = 40
    return self
end

-- Вспомогательная функция: длина строки в пикселях при шрифте 5x7.
-- Каждый символ занимает 5 пикселей + 1 пиксель промежутка.
local function text_width(s)
    return #s * 6 - 1
end

-- Отрисовка строки, отцентрированной по X внутри игрового поля.
local function draw_centered(self, text, y, color)
    local field_width = self.board_w * self.cell
    local w = text_width(text)
    local x = self.margin + math.floor((field_width - w) / 2)
    draw_text(x, y, text, color)
end

function UI:draw(game)
    local W = self.board_w * self.cell
    local H = self.board_h * self.cell

    -- Фон
    clear(COLOR_BG)

    -- Сетка (видна всегда, в том числе в меню)
    for i = 0, self.board_w do
        draw_rect(self.margin + i * self.cell, self.margin,
                  1, H, COLOR_GRID)
    end
    for j = 0, self.board_h do
        draw_rect(self.margin, self.margin + j * self.cell,
                  W, 1, COLOR_GRID)
    end

    -- В меню рисуем только название и подсказку, без игровых объектов
    if game.state == Game.STATE_MENU then
        self:draw_menu(H)
        return
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

    -- Сообщения для паузы и конца игры
    if game.state == Game.STATE_GAME_OVER then
        self:draw_centered("GAME OVER", H / 2 - 20, COLOR_TEXT)
        self:draw_centered("PRESS R TO RESTART", H / 2 + 10, COLOR_HINT)
    elseif game.state == Game.STATE_PAUSED then
        self:draw_centered("PAUSED", H / 2, COLOR_TEXT)
        self:draw_centered("PRESS SPACE TO RESUME", H / 2 + 30, COLOR_HINT)
    end
end

-- Отрисовка стартового меню: название и подсказка
function UI:draw_menu(H)
    self:draw_centered("SNAKE", H / 2 - 40, COLOR_TITLE)
    self:draw_centered("A RETRO GAME", H / 2 - 10, COLOR_TEXT)
    self:draw_centered("PRESS R TO START", H / 2 + 40, COLOR_HINT)
end

-- Обёртка для использования из замыканий выше
function UI:draw_centered(text, y, color)
    local field_width = self.board_w * self.cell
    local w = text_width(text)
    local x = self.margin + math.floor((field_width - w) / 2)
    draw_text(x, y, text, color)
end

return UI
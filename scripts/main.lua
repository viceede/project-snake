-- main.lua — точка входа игровой логики.
-- Здесь связываются модули игры и определяются глобальные
-- функции update(), render(), on_event(), вызываемые из C.

local Game = require("scripts.game")
local UI   = require("scripts.ui")

-- Размеры игрового поля в клетках
local BOARD_W, BOARD_H = 24, 20
local CELL_SIZE = 24

local game = Game.new(BOARD_W, BOARD_H)
local ui   = UI.new(BOARD_W, BOARD_H, CELL_SIZE)

-- Глобальные функции, которые вызывает C-ядро
function update(dt)
    game:update(dt)

    if game.state == "game_over" then
        -- Обработка перезапуска по нажатию R
        -- (событие придёт через on_event, а здесь только логика)
    end
end

function render()
    ui:draw(game)
end

function on_event(ev)
    if ev.type == EV_ESC then
        -- Сигнал завершения: ядро само обработает выход
        return
    elseif ev.type == EV_UP or ev.type == EV_LEFT or
           ev.type == EV_DOWN or ev.type == EV_RIGHT then
        game:set_direction(ev.type)
    elseif ev.type == EV_SPACE then
        game:toggle_pause()
    elseif ev.type == EV_RESTART then
        game:reset()
    end
end
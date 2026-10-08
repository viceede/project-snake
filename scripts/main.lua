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
end

function render()
    ui:draw(game)
end

function on_event(ev)
    if ev.type == EV_ESC then
        -- Сигнал завершения: ядро само обработает выход
        return

    elseif ev.type == EV_RESTART then
        -- R работает в любом состоянии:
        --   в меню       — начать игру
        --   в игре       — перезапустить
        --   в game over  — начать заново
        if game.state == Game.STATE_MENU
           or game.state == Game.STATE_GAME_OVER then
            game:start()
        elseif game.state == Game.STATE_PAUSED then
            game:start()
        end

    elseif ev.type == EV_SPACE then
        game:toggle_pause()

    elseif ev.type == EV_UP or ev.type == EV_LEFT or
           ev.type == EV_DOWN or ev.type == EV_RIGHT then
        game:set_direction(ev.type)
    end
end
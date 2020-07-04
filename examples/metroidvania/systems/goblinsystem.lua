local Concord = require "libs.concord"

local GoblinAI = require "components.goblinai"
local Sprite = require "components.sprite"
local Position = require "components.position"
local Patrol = require "components.patrol"

local GoblinSystem = Concord.system({GoblinAI, Position, Sprite, Patrol})
local Player = require "entities.knight"

function GoblinSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local ai = e[GoblinAI]
    local sprite = e[Sprite]
    local pos = e[Position]
    local patrol = e[Patrol]

    if patrol.alert then
      sprite.sprite:play("run")
    else
      sprite.sprite:play("idle")
    end

--     if ai.idle then
--       sprite.sprite:play("idle")
--     end
    -- if Player[Position].x > pos.x then
    --   sprite.sprite:flip(false)
    -- else
    --   sprite.sprite:flip(true)
    -- end
  end
end

return GoblinSystem

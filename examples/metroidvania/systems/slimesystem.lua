local Concord = require "libs.concord"

local Patrol = require "components.patrol"
local SlimeAI = require "components.slimeai"
local Sprite = require "components.sprite"
local Kinematic = require "components.kinematic"
local Velocity = require "components.velocity"

local SlimeSystem = Concord.system({Patrol, SlimeAI, Sprite, Kinematic, Velocity})

function SlimeSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local patrol = e[Patrol]
    local slime = e[SlimeAI]
    local sprite = e[Sprite]
    local kinematic = e[Kinematic]
    local vel = e[Velocity]
    if patrol.alert then
      if kinematic.isOnFloor then
        sprite.sprite:play("prejump")
        if sprite.sprite.ended then
          vel.y = -100
        end
      else
        if vel.y < 0 then
          sprite.sprite:play("jump")
        else
          sprite.sprite:play("fall")
        end
      end
    elseif not patrol.alert and kinematic.isOnFloor then
      sprite.sprite:play("idle")
    end
  end
end

return SlimeSystem

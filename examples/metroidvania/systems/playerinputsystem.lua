local Concord = require "libs.concord"
local Player = require "components.player"
local Input = require "components.input"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Kinematic = require "components.kinematic"
local PlayerInputSystem = Concord.system({Player, Kinematic, Sprite, Velocity, Input})

function PlayerInputSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local player = e[Player]
    local vel = e[Velocity]
    local sprite = e[Sprite]
    local kinematic = e[Kinematic]
    local pressed = false
    if tico.input.isKeyDown("left") then
      vel.x = -player.speed
      pressed = true
      sprite.sprite:flip(true)
    elseif tico.input.isKeyDown("right") then
      vel.x = player.speed
      pressed = true
      sprite.sprite:flip(false)
    else
      vel.x = vel.x * 0.8
    end

    if kinematic.isOnFloor then
      if pressed then
        sprite.sprite:play("run")
      else
        sprite.sprite:play("idle")
      end
    else
      if vel.y < 0 then sprite.sprite:play("jump")
      else sprite.sprite:play("fall") end
    end

    if tico.input.isKeyPressed("x")  and kinematic.isOnFloor then
      vel.y = -player.jumpForce
      kinematic.isOnFloor = false
    elseif not kinematic.isOnFloor then
      if tico.input.isKeyReleased("x") and vel.y < -player.jumpForce/2 then
        vel.y = -player.jumpForce/2
      end
    end
  end
end

return PlayerInputSystem

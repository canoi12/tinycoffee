local Concord = require "libs.concord"

local Input = require "components.input"
local Sprite = require "components.sprite"
local Velocity = require "components.velocity"
local Kinematic = require "components.kinematic"
local Player = require "components.player"

local InputSystem = Concord.system({Input,Player})

function InputSystem:update(dt)
	for _,e in ipairs(self.pool) do
		local player = e[Player]
		local spr_c = e[Sprite]
		local sprite = spr_c.sprite
		local vel = e[Velocity]
		local kinematic = e[Kinematic]
		local pressed = false
		if tico.input.keyDown("left") then
			vel.x = -100
			pressed = true
			sprite:play("walk")
		elseif tico.input.keyDown("right") then
			vel.x = 100
			pressed = true
			sprite:play("walk")
		else
			vel.x = vel.x * 0.8
			sprite:play("idle")
		end

		if kinematic.isOnFloor then
      if pressed then
        sprite:play("walk")
      else
        sprite:play("idle")
      end
    else
      if vel.y < 0 then sprite:play("jump")
      else sprite:play("fall") end
    end

		if (tico.input.keyPressed("x") or tico.input.joyPressed(0, "a"))  and kinematic.isOnFloor then
      vel.y = -player.jumpForce
      kinematic.isOnFloor = false
      spr_c.scale.x = 0.4
      spr_c.scale.y = 1.8
    elseif not kinematic.isOnFloor then
      if (tico.input.keyReleased("x") or tico.input.joyReleased(0, "a")) and vel.y < -player.jumpForce/2 then
        vel.y = -player.jumpForce/2
      end
    end

	end
end

return InputSystem
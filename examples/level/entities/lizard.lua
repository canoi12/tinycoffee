-- local Class = require "libs.class"
-- local Lizard = Class:extend("Lizard")

-- function Lizard:constructor(object)
-- 	self.x = object.position[1]
-- 	self.y = object.position[2]

-- 	local sprite = tico.resources.getByKey(object.sprite)
-- 	self.sprite = tico.engine.newSprite(sprite)
-- 	self.size = object.size[1] / 16
-- 	self.sprite:play("idle")
-- end

-- function Lizard:update(dt)
-- 	self.sprite:update(dt)
-- end

-- function Lizard:draw()
-- 	self.sprite:draw(self.x, self.y, 0, self.size, 1)
-- end

-- return Lizard

local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Input = require "components.input"
local Hitbox = require "components.hitbox"
local Kinematic = require "components.kinematic"
local Gravity = require "components.gravity"

local Lizard = Concord.assemblage(function(e, obj)
	local pos = {x = obj.position[1], y = obj.position[2]}
	local sprite = tico.resources.getByKey(obj.sprite)
	e
	:give(Position, pos.x, pos.y)
	:give(Velocity)
	:give(Sprite, sprite)
	:give(Hitbox, obj.hitbox)
	:give(Kinematic)
	:give(Gravity)
end)

return Lizard
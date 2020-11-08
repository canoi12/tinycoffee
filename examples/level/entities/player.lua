-- local Class = require "libs.class"
local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Input = require "components.input"
local Player = require "components.player"
local Hitbox = require "components.hitbox"
local Kinematic = require "components.kinematic"
local Gravity = require "components.gravity"

local Player = Concord.assemblage(function(e, obj)
	local pos = {x = obj.position[1], y = obj.position[2]}
	local sprite = tico.resources.getByKey(obj.sprite)
	-- print(pos.x, pos.y)
	e
	:give(Position, pos.x, pos.y)
	:give(Velocity)
	:give(Sprite, sprite)
	:give(Input)
	:give(Player)
	:give(Hitbox, obj.hitbox)
	:give(Kinematic)
	:give(Gravity)
end)
-- local Player = Class:extend("Player")

-- function Player:constructor(object)
-- 	-- self.image = tico.resources.getByKey(object.texture)
-- 	self.x = object.position[1]
-- 	self.y = object.position[2]

-- 	self.angle = object.angle

-- 	local sprite = tico.resources.getByKey(object.sprite)
-- 	self.sprite = tico.engine.newSprite(sprite)
-- 	self.sprite:play("idle")
-- end

-- function Player:update(dt)
-- 	if tico.input.keyDown("right") then
-- 		self.x = self.x + 100 * dt
-- 		self.sprite:play("walk")
-- 	elseif tico.input.keyDown("left") then
-- 		self.x = self.x - 100 * dt
-- 		self.sprite:play("walk")
-- 	else
-- 		self.sprite:play("idle")
-- 	end
-- 	self.sprite:update(dt)
-- 	camera:move(-self.x+80, -self.y+46)
-- end

-- function Player:draw()
-- 	self.sprite:draw(self.x, self.y, self.angle, 1, 1, 8, 8)
-- end

return Player
local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local Hitbox = require "components.hitbox"
local Kinematic = require "components.kinematic"

local Floor = Concord.assemblage(function(e, obj)
	local pos = {x = obj.position[1], y = obj.position[2]}
	local size = {x = obj.size[1], y = obj.size[2]}

	e
	:give(Position, pos.x, pos.y)
	:give(Velocity)
	:give(Hitbox, {0, 0, size.x, size.y})
	:give(Kinematic)
end)

return Floor
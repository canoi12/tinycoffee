local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local Player = require "components.player"

local MoveSystem = Concord.system({Position, Velocity})

function MoveSystem:update(dt)
	for _,e in ipairs(self.pool) do
		local p = e[Position]
		local v = e[Velocity]

		p.x = p.x + (v.x*dt)
		p.y = p.y + (v.y*dt)
		if e:has(Player) then
			camera:move(-p.x+80, -p.y+46)
		end
	end
end

return MoveSystem
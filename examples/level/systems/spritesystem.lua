local Concord = require "libs.concord"
local Position = require "components.position"
local Sprite = require "components.sprite"
local Hitbox = require "components.hitbox"

local SpriteSystem = Concord.system({Position, Sprite})

function SpriteSystem:update(dt)
	for _,e in ipairs(self.pool) do
		local spr = e[Sprite].sprite

		spr:update(dt)
	end
end

function SpriteSystem:draw()
	for _,e in ipairs(self.pool) do
		local pos = e[Position]
		local spr = e[Sprite]

		-- print(pos.x)

		spr.scale.x = tico.math.lerp(spr.scale.x, 1, 0.2)
		spr.scale.y = tico.math.lerp(spr.scale.y, 1, 0.2)
		spr.sprite:draw(pos.x, pos.y, 0, spr.scale.x, spr.scale.y, 8, 8)
		-- if e:has(Hitbox) then
		-- 	local hb = e[Hitbox]
		-- 	tico.graphics.drawRectangle(pos.x+hb.x, pos.y+hb.y, hb.w, hb.h)
		-- end
		-- tico.graphics.fillRectangle(pos.x, pos.y, 32, 32)
	end
end

return SpriteSystem
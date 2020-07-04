local Concord = require "libs.concord"
local Position = require "components.position"
local Sprite = require "components.sprite"
local Hitbox = require "components.hitbox"
local DrawSystem = Concord.system({Position, Sprite}, {Position, Hitbox, "secondPool"})

function DrawSystem:draw()
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local spr = e[Sprite]
    spr.sprite:draw(pos.x, pos.y)
  end
  -- for _,e in ipairs(self.secondPool) do
  --   local pos = e[Position]
  --   local hitbox = e[Hitbox]
  --   local x, y, w, h = bumpWorld:getRect(hitbox)
  --   tico.graphics.drawRectangle(x, y, w, h)
  -- end
end

return DrawSystem

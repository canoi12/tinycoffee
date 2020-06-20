local Concord = require "libs.concord"
local Sprite = require "components.sprite"
local SpriteSystem = Concord.system({Sprite})

function SpriteSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local spr = e[Sprite]
    spr.sprite:update(dt)
  end
end

return SpriteSystem

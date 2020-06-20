local Concord = require "libs.concord"
local sprite = require "tico.sprite"

local Sprite = Concord.component(function(c, path, tilex, tileh, animations)
  c.tilex = tilex or 16
  c.tiley = tiley or 16
  c.sprite = sprite(path, c.tilex, c.tileh)
  animations = animations or {}
  for name,frames in pairs(animations) do
    local frms = frames
    local loop = true
    if type(frames) == "table" then
      frms = frames[1]
      loop = frames[2]
    end
    c.sprite:addAnimation(name, frms, loop)
  end
end)

return Sprite

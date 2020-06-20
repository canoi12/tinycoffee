local Concord = require "libs.concord"

local Hitbox = Concord.component(function(c, x, y, w, h)
  c.x = x or 0
  c.y = y or 0
  c.w = w or 16
  c.h = h or 16
end)

return Hitbox

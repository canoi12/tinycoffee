local Concord = require "libs.concord"

local Hitbox = Concord.component(function(c, hitbox)
  c.x = hitbox[1] or 0
  c.y = hitbox[2] or 0
  c.w = hitbox[3] or 16
  c.h = hitbox[4] or 16
end)

return Hitbox

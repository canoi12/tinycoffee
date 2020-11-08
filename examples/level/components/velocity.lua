local Concord = require "libs.concord"

local Velocity = Concord.component(function(c, x, y)
  c.x = x or 0
  c.y = y or 0
end)

return Velocity

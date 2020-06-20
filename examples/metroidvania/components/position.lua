local Concord = require "libs.concord"

local Position = Concord.component(function(c, x, y)
  c.x = x or 0
  c.y = y or 0
end)

return Position

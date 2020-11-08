local Concord = require "libs.concord"
local Gravity = Concord.component(function(c, gravity)
  c.gravity = gravity or 100
end)

return Gravity

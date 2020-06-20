local Concord = require "libs.concord"
local Kinematic = Concord.component(function(c)
  c.isOnFloor = false
  c.cols = {}
  c.isColliding = false
end)

return Kinematic

local Concord = require "libs.concord"
local Patrol = Concord.component(function(c, target, area)
  c.alert = false
  c.target = target
  c.area = area or 20
end)

return Patrol

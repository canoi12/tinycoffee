local Concord = require "libs.concord"
local Patrol = Concord.component(function(c, area)
  c.alert = false
  c.area = area or 20
end)

return Patrol

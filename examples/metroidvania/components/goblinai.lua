local Concord = require "libs.concord"
local GoblinAI = Concord.component(function(c)
  c.idle = true
  c.moving = false
  c.attack = false
end)

return GoblinAI

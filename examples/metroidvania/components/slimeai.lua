local Concord = require "libs.concord"

local SlimeAI = Concord.component(function(c)
  c.idle = true
  c.walk = false
  c.jumping = false
end)

return SlimeAI

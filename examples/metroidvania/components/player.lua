local Concord = require "libs.concord"
local Player = Concord.component(function(c)
  c.speed = 100
  c.jumpForce = 80
end)

return Player

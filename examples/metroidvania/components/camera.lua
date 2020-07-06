local Concord = require "libs.concord"

local camera = require "libs.camera"

local Camera = Concord.component(function(c, x, y, w, h)
	c.x = x
	c.y = y
	c.w = w
	c.h = h
  c.camera = camera(x, y, w, h)
  c.camera:offset("center", "center")
  c.camera:setLimits(0, 0, 640, 95)
  -- c.camera:setZoom(1)
  c.camera:setDeadzone("off")
end)

return Camera

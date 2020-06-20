local Class = require "tico.class"
local Object = Class:extend("Object")

function Object:constructor(x, y)
  self.x = x or 0
  self.y = y or 0
end

function Object:update(dt)
end

function Object:draw()
end

return Object

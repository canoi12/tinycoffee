local Concord = require "libs.concord"

local Camera = require "components.camera"
local Position = require "components.position"

local CameraSystem = Concord.system({Camera}, {Camera, Position, "second"})
local time = 0
function CameraSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local camera = e[Camera].camera
    camera:update(dt)
  end
  time = time + dt
  for _,e in ipairs(self.second) do
    local camera = e[Camera].camera
    local pos = e[Position]
    camera:follow(pos)
  end
end

function CameraSystem:predraw()
  for _,e in ipairs(self.pool) do
    local camera = e[Camera].camera
    camera:attach()
  end
end

function CameraSystem:postdraw()
  for _,e in ipairs(self.pool) do
    local camera = e[Camera].camera
--     camera:draw()
    camera:detach()
  end
end

return CameraSystem
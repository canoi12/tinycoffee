local Concord = require "libs.concord"

local Camera = require "components.camera"
local Position = require "components.position"
local Target = require "components.target"

local CameraSystem = Concord.system({Camera, Target})
local time = 0
function CameraSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local camera = e[Camera].camera
    local target = gamescene:getEntity(e[Target].id)
    camera:update(dt)
    camera:follow(target[Position])
  end
  --[[time = time + dt
  for _,e in ipairs(self.second) do
    local camera = e[Camera].camera
    local pos = e[Position]
    camera:follow(pos)
  end--]]
end

function CameraSystem:predraw()
  for _,e in ipairs(self.pool) do
    local camera = e[Camera].camera
    local target = gamescene:getEntity(e[Target].id)
    camera:attach()
    -- tico.graphics.drawRectangle(target[Position].x-8, target[Position].y-8, 16, 16)
    -- tico.graphics.drawRectangle(32, 32, 32, 32, tico.color("#000000"))
    -- tico.graphics.drawCircle(32, 32, 8, {0, 0, 0})
    -- tilemap:draw()
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

local Concord = require "libs.concord"
local Position = require "components.position"
local Hitbox = require "components.hitbox"
local Kinematic = require "components.kinematic"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Player = require "components.player"
local KinematicSystem = Concord.system({Position, Velocity, Kinematic, Hitbox})

function KinematicSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local hit = e[Hitbox]
    local kine = e[Kinematic]
    local vel = e[Velocity]

    pos.x = pos.x + vel.x * dt
    pos.y = pos.y + vel.y * dt
    local world = e.__world.physics

    local x, y, cols, len = world:move(e, pos.x+hit.x, pos.y+hit.y, colFilter)
    local wasOnFloor = kine.isOnFloor
    kine.isOnFloor = false
    if len > 0 then kine.isColliding = true end
    kine.cols = cols
    for i,col in ipairs(cols) do
      if col.normal.y == -1 then
        kine.isOnFloor = true
        if not wasOnFloor then vel.y = 0 end
        vel.y = 0
      elseif col.normal.y == 1 then
        vel.y = 0
      end
    end
    pos.x = x - hit.x
    pos.y = y - hit.y

    if e:has(Player) then camera:move(-pos.x+80, -pos.y+46) end
  end
end

-- function KinematicSystem:postdraw()
--   for _,e in ipairs(self.pool) do
--     local x, y, w, h = bumpWorld:getRect(e)
--     tico.graphics.drawRectangle(x, y, w, h)
--   end
-- end

return KinematicSystem

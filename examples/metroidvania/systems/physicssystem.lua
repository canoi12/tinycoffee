local Concord = require "libs.concord"
local Position = require "components.position"
local Hitbox = require "components.hitbox"
local Player = require "components.player"
local Kinematic = require "components.kinematic"
local PhysicsSystem = Concord.system({Position, Kinematic, Hitbox})

function PhysicsSystem:preupdate(dt)
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local hit = e[Hitbox]
    local kine = e[Kinematic]

    local x, y, cols, len = bumpWorld:move(hit, pos.x+hit.x, pos.y+hit.y)
    kine.isOnFloor = false
    if len > 0 then kine.isColliding = true end
    kine.cols = cols
    for i,col in ipairs(cols) do
      if col.normal.y == -1 then
        kine.isOnFloor = true
      end
    end
    pos.x = x - hit.x
    pos.y = y - hit.y
  end
end

return PhysicsSystem

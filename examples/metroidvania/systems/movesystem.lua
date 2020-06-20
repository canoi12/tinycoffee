local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local MoveSystem = Concord.system({Position, Velocity})

function MoveSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local vel = e[Velocity]

    pos.x = pos.x + vel.x * dt
    pos.y = pos.y + vel.y * dt
  end
end

return MoveSystem

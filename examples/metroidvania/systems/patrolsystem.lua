local Concord = require "libs.concord"
local utils = require "libs.utils"

local Patrol = require "components.patrol"
local Position = require "components.position"

local PatrolSystem = Concord.system({Position, Patrol})

function PatrolSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local patrol = e[Patrol]

    if utils.distance(pos.x, pos.y, patrol.target.x, patrol.target.y) < patrol.area then
      patrol.alert = true
    else
      patrol.alert = false
    end
  end
end

-- function PatrolSystem:draw()
--   for _,e in ipairs(self.pool) do
--     local pos = e[Position]
--     local patrol = e[Patrol]

--     tico.graphics.drawCircle(pos.x, pos.y, patrol.area)
--   end
-- end

return PatrolSystem

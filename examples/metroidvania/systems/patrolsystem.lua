local Concord = require "libs.concord"
local utils = require "libs.utils"

local Patrol = require "components.patrol"
local Position = require "components.position"
local Target = require "components.target"

local PatrolSystem = Concord.system({Position, Patrol, Target})

function PatrolSystem:update(dt)
  for _,e in ipairs(self.pool) do
    local pos = e[Position]
    local patrol = e[Patrol]
    local target = gamescene:getEntity(e[Target].id)
    local tpos = target[Position]


    if utils.distance(pos.x, pos.y, tpos.x, tpos.y) < patrol.area then
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

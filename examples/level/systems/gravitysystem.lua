local Concord = require "libs.concord"
local Velocity = require "components.velocity"
local Gravity = require "components.gravity"
local Kinematic = require "components.kinematic"

local GravitySystem = Concord.system({Velocity, Kinematic, Gravity})

function GravitySystem:update(dt)
  for _,e in ipairs(self.pool) do
    local vel = e[Velocity]
    local grav = e[Gravity]
    local kine = e[Kinematic]
    vel.y = vel.y + grav.gravity * dt
  end
end

return GravitySystem

local Concord = require "libs.concord"
local Position = require "components.position"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Hitbox = require "components.hitbox"
local Gravity = require "components.gravity"
local Kinematic = require "components.kinematic"

local Enemy = Concord.assemblage(function(e, x, y)
  e
  :give(Position, x, y)
  :give(Velocity)
  :give(Hitbox, -8, -8, 16, 16)
  :give(Kinematic)
end)

return Enemy

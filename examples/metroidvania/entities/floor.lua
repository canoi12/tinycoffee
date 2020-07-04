local Concord = require "libs.concord"

local Position = require "components.position"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Hitbox = require "components.hitbox"
local Gravity = require "components.gravity"
local Kinematic = require "components.kinematic"

local Floor = Concord.assemblage(function(e, x, y, w, h)
  e
  :give(Position, x, y)
  :give(Velocity)
  :give(Hitbox, 0, 0, w, h)
  :give(Kinematic)
end)

return Floor

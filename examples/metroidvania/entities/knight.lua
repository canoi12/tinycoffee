local Concord = require "libs.concord"
local Knight = Concord.entity()
local Position = require "components.position"
local Velocity = require "components.velocity"
local Sprite = require "components.sprite"
local Player = require "components.player"
local Input = require "components.input"
local Hitbox = require "components.hitbox"
local Gravity = require "components.gravity"
local Kinematic = require "components.kinematic"

local Camera = require "components.camera"

Knight
  :give(Position, 32, 24)
  :give(Velocity)
  :give(Sprite, "assets/images/herochar_spritesheet.png", 16, 16,
    {
      ["idle"] = "33-36",
      ["run"] = "9-14",
      ["jump"] = "49-51",
      ["fall"] = "41-43"
    }
  )
  :give(Player)
  :give(Input)
  :give(Hitbox, -6, -8, 12, 16)
  :give(Gravity)
  :give(Kinematic)
  :give(Camera, 0, 0, 160, 95)

return Knight

local Concord = require "libs.concord"

local Enemy = require "entities.enemy"
local Sprite = require "components.sprite"
local Gravity = require "components.gravity"
local Player = require "entities.knight"
local Position = require "components.position"
local Patrol = require "components.patrol"
local SlimeAI = require "components.slimeai"
local Hitbox = require "components.hitbox"

local Slime = Concord.assemblage(function(e, x, y)
  e
  :assemble(Enemy, x, y)
  :give(Hitbox, -8, -4, 16, 12)
  :give(Sprite, "assets/images/slime_spritesheet.png", 16, 16,
    {
      ["idle"] = "46-50",
      ["prejump"] = {"16-20", false},
      ["jump"] = {"21-23", false},
      ["fall"] = {"24-28", false},
      ["land"] = {"29-31", false}
    }
  )
  :give(Gravity)
  :give(Patrol, Player[Position], 20)
  :give(SlimeAI)
end)

return Slime

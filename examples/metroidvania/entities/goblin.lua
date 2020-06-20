local Concord = require "libs.concord"
local Enemy = require "entities.enemy"

local Sprite = require "components.sprite"
local Gravity = require "components.gravity"
local GoblinAI = require "components.goblinai"
local Patrol = require "components.patrol"
local Player = require "entities.knight"
local Position = require "components.position"

local Goblin = Concord.assemblage(function(e, x, y)
  e
  :assemble(Enemy, x, y)
  :give(Sprite, "assets/images/goblin_spritesheet.png", 16, 16,
    {
      ["run"] = "1-6",
      ["idle"] = "19-22",
--       ["attack"] = "13-19",
      ["hit"] = "25-27"
    }
  )
  :give(Gravity)
  :give(GoblinAI)
  :give(Patrol, Player[Position], 30)
end)

return Goblin

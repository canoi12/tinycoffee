local Concord = require "libs.concord"

local Sprite = Concord.component(function(c, sprite)
  c.sprite = tico.engine.newSprite(sprite)
  c.sprite:play("idle")
  c.scale = {
  	x = 1,
  	y = 1
  }
end)

return Sprite

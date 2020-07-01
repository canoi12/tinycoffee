local class = require "libs.class"
local Scene = class:extend("Scene")

local Concord = require "libs.concord"

local Position = require "components.position"
local Hitbox = require "components.hitbox"


function Scene:constructor()
  self.world = Concord.world()
  self.world.onEntityAdded = function(wrl, e)
    if e:has(Hitbox) and e:has(Position) then
      local hitbox = e[Hitbox]
      local pos = e[Position]
      bumpWorld:add(hitbox, pos.x, pos.y, hitbox.w, hitbox.h)
    end
  end

  self.world.onEntityRemoved = function(wrl, e)
    if e:has(Hitbox) then
      local hitbox = e[Hitbox]
      bumpWorld:remove(hitbox)
    end
  end
end

function Scene:update(dt)
  self.world:emit("preupdate", dt)
  self.world:emit("update", dt)
  self.world:emit("postupdate", dt)
end

function Scene:draw()
  self.world:emit("predraw")
  self.world:emit("draw")
  self.world:emit("postdraw")
end

function Scene:add(e)
  self.world:addEntity(e)
end

return Scene

local Object = require "object"
local Player = Object:extend("Player")
local Vector = require "tico.vector"
local Sprite = require "tico.sprite"

function Player:constructor(x, y)
  self.pos = Vector(x, y)
  self.sprite = Sprite("assets/knight.png", 32, 32)
  self.sprite:addAnimation("idle", "1-4", true)
  self.sprite:addAnimation("walk", "5-10", true)
  self.sprite:play("idle")
  self.sword = Sprite("assets/sword.png", 16, 16)
  self.sword:addAnimation("idle", "1-1")
  self.sword:offset(-1, 6)
  self.speed = 100
  self.velocity = Vector(0, 0)
end

function Player:update(dt)
  if tico.input.isKeyDown("left") then
    self.velocity.x = -self.speed
    self.sprite:flip(true)
  elseif tico.input.isKeyDown("right") then
    self.velocity.x = self.speed
    self.sprite:flip(false)
  else
    self.velocity.x = self.velocity.x * 0.5
  end
  self.sword.flip_h = self.sprite.flip_h
  if math.abs(self.velocity.x) > 1 then
    self.sprite:play("walk")
  else
    self.sprite:play("idle")
  end

  local v = self.velocity * dt
  self.pos:add(v)
  self.sprite:update(dt)
  self.sword:update(dt)
end

function Player:draw()
  self.sword:draw(self.pos.x, self.pos.y)
  self.sprite:draw(self.pos.x, self.pos.y)
end

return Player

local carrot = {}
carrot.angle = 0
carrot.image = tico.graphics.newImage("carrot.png")
carrot.x = 0
carrot.y = 0
carrot.dx = 0
carrot.dy = 0

function carrot:new(o)
  o = o or {}
  setmetatable(o, {__index=self})
  o:init()
  return o
end

function carrot:init()
  self.speed = 100
  -- self.image:setFilter('nearest', 'nearest')
end

function carrot:update(dt)
  self.dx = (math.cos(self.angle)*self.speed)
  self.dy = (math.sin(self.angle)*self.speed)

  self.x = self.x + (self.dx*dt)
  self.y = self.y + (self.dy*dt)
end

function carrot:draw()
  -- love.graphics.draw(self.image, self.x, self.y, self.angle-math.rad(90), 1, 1, 8, 8)
  self.image:draw(self.x, self.y, self.angle - math.rad(90), 1, 1, 8, 8)
end

return carrot

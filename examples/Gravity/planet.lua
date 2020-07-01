local planet = {}

function planet:new (o)
  o = o or {}
  setmetatable(o, {__index=self})
  o:init()
  return o
end

function planet:init()
  self.r = 32
  self.x = 0
  self.y = 0
  self.angle_player = 0
  self.eyes = {
    left = {
      x = -7,
      y = -8
    },
    right = {
      x = 20,
      y = -8
    },
    radius = 7
  }
  self.orbit = self.r * 3
  self.image = nil
end

function planet:setRadius(radius)
  self.r = radius
  self.orbit = radius * 3
end

function planet:update(dt)

end

function planet:draw()
  if self.image == nil then
    tico.graphics.drawCircle(self.x, self.y, self.r)
    tico.graphics.drawCircle(self.x, self.y, self.orbit)
  else
    tico.graphics.draw(self.image, self.x, self.y, 0, 1, 1, self.image:getWidth()/2, self.image:getHeight()/2)
    tico.graphics.drawCircle(self.x, self.y, self.orbit)
  end
  local test = -1
  for i,v in pairs(self.eyes) do
    --tico.graphics.print(type(v), self.x, self.y)
    if type(v) == "table" then
      test = test * -1
      -- tico.graphics.setColor(255, 255, 255)
      tico.graphics.fillCircle(self.x + v.x, self.y+v.y, self.eyes.radius, {255, 255, 255})
      -- tico.graphics.setColor(34/255,32/255,52/255)
      tico.graphics.fillCircle((self.x + v.x) - (math.cos(self.angle_player)*4), self.y+v.y - (math.sin(self.angle_player)*4), 4, {34, 32, 52})
      -- tico.graphics.setColor(255, 255, 255)
    end
  end
  --love.graphics.print(type(self.eyes), self.x, self.y)
end

return planet

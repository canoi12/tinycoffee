local class = require "libs.class"
local Tilemap = class:extend("Tilemap")
local Tileset = class:extend("Tileset")

function Tileset:constructor(tileset)
  self.name = tileset.name
  self.image = tico.graphics.newImage("assets/" .. tileset.image)
  self.tilewidth = tileset.tilewidth
  self.tileheight = tileset.tileheight
  self.rects = {}
  local imgw, imgh = self.image:getSize()
  local tilew = self.tilewidth
  local tileh = self.tileheight
  local i = 1
  for y=0,imgh-tileh,tileh do
    for x=0,imgw-tilew,tilew do
      table.insert(self.rects, tico.graphics.newRectangle(x, y, tilew, tileh))
      -- table.insert(self.rects, tico.graphics.newQuad(x, y, tilew, tileh, imgw, imgh))
    end
  end
end

function Tilemap:constructor()
  map = require "assets.map"
  self.width = map.width
  self.height = map.height
  self.tilewidth = map.tilewidth
  self.tileheight = map.tileheight
  self.tilesets = {}
  for i,tileset in ipairs(map.tilesets) do
    table.insert(self.tilesets, Tileset(tileset))
  end
  self.data = map.layers[1].data
  for i,obj in ipairs(map.layers[2].objects) do
    bumpWorld:add("box"..obj.id, obj.x, obj.y, obj.width, obj.height)
  end
end

function Tilemap:draw()

  local x = 0
  local y = 0
  local img = self.tilesets[1].image
  for i,tile in ipairs(self.data) do
    if tile ~= 0 then
      img:draw(self.tilesets[1].rects[tile], x, y)
      -- love.graphics.draw(img, self.tilesets[1].rects[tile], x, y)
    end
    x = x + 16
    if x >= (self.width*16) then
      x = 0
      y = y + 16
    end
  end
end

return Tilemap

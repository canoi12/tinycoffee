local class = require "libs.class"
local Tilemap = class:extend("Tilemap")
local Tileset = class:extend("Tileset")

local Golbin = require "entities.goblin"

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

-- function Tilemap:constructor(filename)
--   self.map = tico.json.load(filename)
--   self.width = self.map.width
--   self.height = self.map.height
--   self.tilewidth = self.map.tilewidth
--   self.tileheight = self.map.tileheight
--   self.tilesets = {}

--   for i,tileset in ipairs(self.map.tilesets) do
--     table.insert(self.tilesets, Tileset(tileset))
--   end
--   for i,layer in ipairs(self.map.layers) do
--     if layer.type == "objectgroup" then
--         for j,obj in ipairs(layer.objects) do
--           -- gamescene:add
--         end
--     end
--   end
--   self.data = self.map.layers[1].data
--   for i,obj in ipairs(self.map.layers[2].objects) do
--     bumpWorld:add("box"..obj.id, obj.x, obj.y, obj.width, obj.height)
--   end
-- end

function Tilemap:constructor(layer, w, h, tileset)
    self.width = layer.width
    self.height = layer.height
    self.tilewidth = w
    self.tileheight = h
    self.tileset = tileset
    self.data = layer.data
end

function Tilemap:draw()
  local x = 0
  local y = 0
  local img = self.tileset.image
  for i,tile in ipairs(self.data) do
    if tile ~= 0 then
      img:draw(self.tileset.rects[tile], x, y)
      -- love.graphics.draw(img, self.tilesets[1].rects[tile], x, y)
    end
    x = x + self.tilewidth
    if x >= (self.width*self.tilewidth) then
      x = 0
      y = y + self.tileheight
    end
  end
end

return Tilemap

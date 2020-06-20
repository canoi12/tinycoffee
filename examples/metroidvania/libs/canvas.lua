local Class = require("libs.class")
---@class LunaCanvas : LunaClass
---@field width number
---@field height number
---@field canvas Canvas
---@field ration number
---@field x number
---@field y number
local Canvas = Class:extend("Canvas")

function Canvas:constructor(width, height, filter)
	local width = width or love.graphics.getWidth()
	local height = height or love.graphics.getHeight()
	local filter = filter or "nearest"
-- 	print(width, height)
	self.width = width
	self.height = height
	self.canvas = tico.graphics.newCanvas(width, height)
-- 	self.canvas:setFilter(filter, filter)
	self.scale = 1
	self.ratio = 1
	self.x = 0
	self.y = 0

	self:onResize(tico.graphics.getWidth(), tico.graphics.getHeight())

-- 	love.resize = function(w, h)
-- 		self:onResize(w, h)
-- 	end
end

function Canvas:onResize(w, h)
	local ww, hh = w, h
	local aspw, asph = ww/self.width, hh/self.height
	if aspw < asph then self.ratio = aspw
	else self.ratio = asph end
	self.x = ww/2
	self.y = hh/2
end

function Canvas:set_scale(scale)
	self.scale = scale
end

---Attach the canvas
function Canvas:attach()
	self.canvas:attach()
	tico.graphics.clear({75, 90, 90})
-- 	love.graphics.clear(0.2, 0.25, 0.25, 1)
end

---Detach the canvas
function Canvas:detach()
-- 	love.graphics.setCanvas()
  self.canvas:detach()
end

---Draw the canvas
function Canvas:draw()
  self.canvas:draw(self.x, self.y-2, 0, self.ratio, self.ratio, self.width/2, self.height/2)
end

return Canvas

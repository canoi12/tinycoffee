local Class = require("libs.class")
local utils = require("libs.utils")

---@param sprite LunaSprite
local function create_quads(sprite)
	sprite.quads = {}
	local texw, texh = sprite.image:getDimensions()
	local imgw, imgh = sprite.img_width, sprite.img_height
	local fw, fh = sprite.frame_width, sprite.frame_height

	for y = 0, imgh - fh, fh do
		for x = 0, imgw - fw, fw do
-- 			table.insert(sprite.quads, love.graphics.newQuad(sprite.ox + x, sprite.oy + y, fw, fh, texw, texh))
      table.insert(sprite.quads, tico.graphics.newRectangle(sprite.ox + x, sprite.oy + y, fw, fh))
		end
	end
end

---@class LunaSprite : LunaClass
---@field ox number
---@field oy number
---@field img_width number
---@field img_height number
---@field image Image
---@field frame_width number
---@field frame_height number
---@field animations table<string, Quad[]>
---@field animation string
---@field animation_frame number
---@field quads Quad[]
---@field flip_h number
---@field flip_v number
---@field center_x number
---@field center_y number
---@field angle number
---@field scale_x number
---@field scale_y number
---@field frame number
---@field speed number
---@field initial_speed number
local Sprite = Class:extend("Sprite")

---@overload fun(image:Image | string | LunaImage, frame_width:number, frame_height:number):void
function Sprite:constructor(image, frame_width, frame_height, options)
	self.ox = 0
	self.oy = 0
	self.img_width = 0
	self.img_height = 0
	self.transform = {} ---@type LunaTransform
	local options = options or {}
	if type(image) == "string" then
		local img ---@type LunaImage | Image
		if luna then
			img = luna.graphics.newImage(image)
		else
			img = tico.graphics.newImage(image)
		end
		self.image = img
		self.img_width, self.img_height = self.image:getDimensions()
		if img.tex then
			self.image = img.tex
			self.ox = img.x
			self.oy = img.y
			self.img_width, self.img_height = img.w, img.h
		end
	elseif type(image) == "userdata" then
		self.image = image
		self.img_width, self.img_height = self.image:getDimensions()
	elseif type(image) == "table" then
		self.image = image.tex
		self.ox = image.x
		self.oy = image.y
		self.img_width, self.img_height = image.w, image.h
	end

-- 	self.image:setFilter("nearest", "nearest")

	self.frame_width = frame_width or self.image:getWidth()
	self.frame_height = frame_height or frame_width or self.image:getHeight()

	self.animations = {}
	self.animation = ""
	self.animation_frame = 1

	self.quads = {}
	self.flip_h = 1
	self.flip_v = 1
	self.center_x = self.frame_width / 2
	self.center_y = self.frame_height / 2
	self.angle = 0
	self.scale_x = 1
	self.scale_y = 1
	self.ended = false

	self.on_start_callback = {
		fn = function()
		end,
		props = {}
	}
	self.on_end_callback = {
		fn = function()
		end,
		props = {}
	}

	create_quads(self)

	self.frame = 1
	self.initial_speed = 0.12
	self.speed = self.initial_speed
	for key, option in pairs(options) do
		self[key] = option
	end
end

---Add an animation
---@param name string Animation name
---@param frames string Animation frames
---`sprite:addAnimation(..., '1-4', ...)`
---
---`sprite:addAnimation(..., '1,2,3,4-5,9-2', ...)`
---@param loop boolean
function Sprite:addAnimation(name, frames, loop)
	local animation = {}
	animation.loop = (loop == nil) and true or loop
	for word in utils.split(frames) do
		local from, to, delta = 0, 0, 1
		if string.find(word, "-") then
			local n1, n2 = word:match("([^,]+)-([^,]+)")
			from = tonumber(n1)
			to = tonumber(n2)
			if from < to then
				delta = 1
			else
				delta = -1
			end
		else
			from = tonumber(word)
			to = from
		end

		for i = from, to, delta do
			table.insert(animation, i)
		end
	end

	self.animations[name] = animation
	self.animation = name
end

---Play animation
---@param name string
function Sprite:play(name)
  if name ~= self.animation then
    self.animation_frame = 1
    self.animation = name
  end
end

---Stop animation
function Sprite:stop()
	self.animation = ""
end

---Set sprite offset
---@overload fun(offset:number):void
---@param ox number offset x
---@param oy number offset y
---@return number, number
function Sprite:offset(cx, cy)
	self.center_x = cx or self.center_x
	self.center_y = cy or cx or self.center_y
	return self.center_x, self.center_y
end

---Set sprite image filter
---@param min FilterMode
function Sprite:setFilter(min, mag)
	self.filter = min or self.filter
end

---Set frame size
---@overload fun(size:number):void
---@param width number
---@param height number
function Sprite:setSize(width, height)
	self.frame_width = width or self.frame_width
	self.frame_height = height or width or self.frame_height
end

---Set sprite flip
---@param flip_h boolean
---@param flip_v boolean
---@return number, number
function Sprite:flip(flip_h, flip_v)
	self.flip_h = not flip_h and 1 or -1
	self.flip_v = not flip_v and 1 or -1
	return self.flip_h, self.flip_v
end

---Rotate sprite
---@param angle number
function Sprite:rotate(angle)
	self.angle = math.rad(angle)
end

---Set a callback for when animation start
---@param callback function
---@vararg any
function Sprite:onStart(callback, ...)
	if type(callback) ~= "function" then
		return
	end
	self.on_start_callback.fn = callback
	self.on_start_callback.props = {...}
end

---Set a callback for when animation end
---@param callback function
---@vararg any
function Sprite:onEnd(callback, ...)
	if type(callback) ~= "function" then
		return
	end
	self.on_end_callback.fn = callback
	self.on_end_callback.props = {...}
end

---Set animation speed
---@param speed number
function Sprite:setSpeed(speed)
	self.speed = speed or 0.12
	self.initial_speed = self.speed
end

---Set sprite scale
---@param scale_x number
---@param scale_y number
---@return number, number
function Sprite:scale(scale_x, scale_y)
	self.scale_x = scale_x or self.scale_x
	self.scale_y = scale_y or self.scale_y
	return self.scale_x, self.scale_y
end


---Update sprite
---@param dt number
function Sprite:update(dt)
  self.ended = false
	if self.animation ~= "" then
		self.speed = self.speed - (1 * dt)
		if self.speed <= 0 then
		  self.speed = self.initial_speed
			self.animation_frame = self.animation_frame + 1
			if self.animation_frame > #self.animations[self.animation] and self.animations[self.animation].loop then
				self.animation_frame = 1
				self.on_end_callback.fn(self, self.on_end_callback.props)
				self.ended = true
			end
		end
		self.frame = self.animations[self.animation][self.animation_frame] or 1
	end
	if self.frame > #self.quads then
		self.frame = 1
	end
end

---Draw sprite
---@param x number
---@param y number
function Sprite:draw(x, y)
	x = x or self.transform.position.x
	y = y or self.transform.position.y
	tico.graphics.draw(
		self.image,
		self.quads[self.frame],
		x,
		y,
		self.angle,
		self.scale_x * self.flip_h,
		self.scale_y * self.flip_v,
		self.center_x,
		self.center_y
	)
end

function Sprite:createAnimation(name, first, last)
end

return Sprite

local Class = require "tico.class"
local Sprite = Class:extend("Sprite")
local utils = require "tico.utils"

local function create_quads(sprite)
	sprite.quads = {}
	local imgw, imgh = sprite.image:getDimensions()
	local fw, fh = sprite.frame_width, sprite.frame_height

	for y = 0, imgh - fh, fh do
		for x = 0, imgw - fw, fw do
			table.insert(sprite.quads, tico.graphics.newRectangle(x, y, fw, fh))
		end
	end
end

function Sprite:constructor(image, frame_width, frame_height, options)
	local options = options or {}
	if type(image) == "string" then
		self.image = tico.graphics.newImage(image)
	elseif type(image) == "userdata" then
		self.image = image
	end
-- 	self.image:setFilter("nearest", "nearest")

	self.frame_width = frame_width or self.image:width()
	self.frame_height = frame_height or frame_width or self.image:height()

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

function Sprite:addAnimation(name, frames, loop)
	local animation = {}
	animation.loop = loop or true
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

function Sprite:play(name)
  if name ~= self.animation then
    self.animation_frame = 1
  end
	self.animation = name
end

function Sprite:stop()
	self.animation = ""
end

function Sprite:offset(cx, cy)
	self.center_x = cx or self.center_x
	self.center_y = cy or cx or self.center_y
end

function Sprite:setFilter(min, mag)
	self.filter = min or self.filter
end

function Sprite:setSize(width, height)
	self.frame_width = width or self.frame_width
	self.frame_height = height or width or self.frame_height
end

function Sprite:flip(flip_h, flip_v)
	self.flip_h = not flip_h and 1 or -1
	self.flip_v = not flip_v and 1 or -1
end

function Sprite:rotate(angle)
	self.angle = math.rad(angle)
end

function Sprite:onStart(props, callback)
	if type(callback) ~= "function" then
		return
	end
	self.on_start_callback.fn = callback
	self.on_start_callback.props = props or {}
end

function Sprite:onEnd(props, callback)
	if type(callback) ~= "function" then
		return
	end
	self.on_end_callback.fn = callback
	print(props)
	self.on_end_callback.props = props or {}
end

function Sprite:setSpeed(speed)
	self.speed = speed or 0.12
	self.initial_speed = self.speed
end

function Sprite:scale(scale_x, scale_y)
	self.scale_x = scale_x or self.scale_x
	self.scale_y = scale_y or scale_x or self.scale_y
end

function Sprite:update(dt)
	if self.animation ~= "" then
		self.speed = self.speed - (1 * dt)
		if self.speed <= 0 then
			self.speed = self.initial_speed
			self.animation_frame = self.animation_frame + 1
			if self.animation_frame > #self.animations[self.animation] then
				self.animation_frame = 1
				self.on_end_callback.fn(self, self.on_end_callback.props)
			end
		end
		self.frame = self.animations[self.animation][self.animation_frame] or 1
	end
	if self.frame > #self.quads then
		self.frame = 1
	end
end

function Sprite:draw(x, y)
--   self.image:draw(self.quads[self.frame], x, y, self.angle, self.scale_x)
	self.image:draw(
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


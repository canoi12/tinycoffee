local Class = require("libs.class")
local utils = require("libs.utils")

-- local bg = love.graphics.newImage("assets/bg.png")
-- bg:setFilter("nearest", "nearest")
-- bg:setWrap("repeat", "repeat")
local quad ---@type Quad

local math_rad = math.rad
local math_cos, math_sin = math.cos, math.sin
local abs = math.abs
---@class LunaCamera : LunaClass
---@field width number
---@field height number
---@field x number
---@field y number
---@field zoom number
---@field angle number
---@field offset_x number
---@field offset_y number
---@field target table | nil
---@field lerp number
---@field deadzone boolean
---@field limits table<key, number>
---@field deadzones table<key, number>
---@field smooth table<key, any>
local Camera = Class:extend("Camera")

function Camera:constructor(x, y, width, height, zoom, angle)
    self.width = width or 320
    self.height = height or 180
    self.x = x or self.width / 2
    self.y = y or self.height / 2
    self.zoom = zoom or 1
    local angle = angle or 0
    self.angle = math_rad(angle)
    self.offset_x = 0
    self.offset_y = 0
    self.target = nil
    self.lerp = 0.05
    self.deadzone = false

    self.limits = {
        left = 0,
        right = 100000,
        top = 0,
        bottom = 100000
    }
--     quad = love.graphics.newQuad(self.limits.left, self.limits.top, self.limits.right, self.limits.bottom, 32, 32)

    self.deadzones = {
        left = 0,
        right = self.width,
        top = 0,
        bottom = self.height
    }

    self.smooth = {
        zoom = self.zoom,
        x = self.x,
        y = self.y,
        move = {
            x = self.y,
            y = self.x
        },
        angle = self.angle
    }
end

---Return the camera coords of the given point
---@param x number
---@param y number
---@return number, number @Point in camera coords
function Camera:toCameraCoords(x, y)
    local c, s = math_cos(self.angle), math_sin(self.angle)
    x, y = x - self.x, y - self.y
    x, y = c * x - s * y, c * y - s * x
    return x * self.zoom + self.width / 2, y * self.zoom + self.height / 2
end

---Moves the camera
---@overload fun(x:number, y:number):void
---@param x number
---@param y number
---@param smooth boolean
function Camera:move(x, y, smooth)
    self.smooth.x = (x or 0)
    self.smooth.y = (y or 0)
    self.x = smooth and self.x or self.smooth.x
    self.y = smooth and self.y or self.smooth.y
end

---Zoom the camera
---@overload fun(zoom:number):void
---@param zoom number
---@param smooth boolean
function Camera:setZoom(zoom, smooth)
    self.smooth.zoom = zoom or 1
    self.zoom = smooth and self.zoom or self.smooth.zoom
end

---Set the camera's deadzone
---@overload fun(deadzone: string):void
---@param left number @Left position
---@param top number @Top position
---@param right number @Right position
---@param bottom number @Bottom position
function Camera:setDeadzone(left, top, right, bottom)
	self.deadzone = true
	if type(left) == "string" then
		self.deadzones.left = (self.width/2) - 20
		self.deadzones.right = (self.width/2) + 20
		self.deadzones.top = (self.height/2) - 40
		self.deadzones.bottom = (self.height/2) + 20
	else
			self.deadzones = {
			left = left or self.deadzones.left,
			right = right or self.deadzones.right,
			top = top or self.deadzones.top,
			bottom = bottom or self.deadzones.bottom
		}
	end
end

---Rotates the camera
---@param angle number
---@param optional smooth boolean
function Camera:rotate(angle, smooth)
    self.smooth.angle = math_rad(angle) or 0
    self.angle = smooth and self.angle or self.smooth.angle
end

---Sets the camera offset
---@overload fun(offset:number | string):void
---@param ox number | string
---@param oy number | string
function Camera:offset(ox, oy)
    if type(ox) == "string" then
        if ox == "center" then
            self.offset_x = self.width / 2
        elseif ox == "left" then
            self.offset_x = 0
        elseif ox == "right" then
            self.offset_x = self.width
        end
    elseif type(ox) == "number" then
        self.offset_x = ox
    end

    local oy = oy or ox or self.offset_y

    if type(oy) == "string" then
        if oy == "center" then
            self.offset_y = self.height / 2
        elseif oy == "top" or oy == "left" then
            self.offset_y = 0
        elseif oy == "bottom" or oy == "right" then
            self.offset_y = self.height
        end
    elseif type(oy) == "number" then
        self.offset_y = oy
    end
end

---Follow the target
---@overload fun(target:table):void
---@param target table
---@param smooth boolean
function Camera:follow(target, smooth)
    self.target = {
        smooth = smooth or false,
        obj = target
    }
end

---Set the camera's limits
---@param left number @Left position
---@param top number @Top position
---@param right number @Right position
---@param bottom number @Bottom position
function Camera:setLimits(left, top, right, bottom)
    self.limits.left = left or self.limits.left
    self.limits.right = right or self.limits.right
    self.limits.top = top or self.limits.top
    self.limits.bottom = bottom or self.limits.bottom

--     quad = love.graphics.newQuad(self.limits.left, self.limits.top, self.limits.right, self.limits.bottom, 32, 32)
end

---Update the camera
---@param dt number
function Camera:update(dt)
    if self.target then
      if self.deadzone then
        local tx, ty = self:toCameraCoords(self.target.obj.x, self.target.obj.y)
        if tx < self.deadzones.left then
            local delta = self.deadzones.left - tx
            self.smooth.x = self.x - delta
        elseif tx > self.deadzones.right then
            local delta = tx - self.deadzones.right
            self.smooth.x = self.x + delta
        end
        if ty < self.deadzones.top then
            local delta = self.deadzones.top - ty
            self.smooth.y = self.y - delta
        elseif ty > self.deadzones.bottom then
            local delta = ty - self.deadzones.bottom
            self.smooth.y = self.y + delta
			  end
			  if not self.target.smooth then
  				self.x = self.smooth.x
  				self.y = self.smooth.y
			  end
      else
        self:move(self.target.obj.x, self.target.obj.y, self.target.smooth)
      end
--       print(self.x, self.y)
    end
    self.zoom = utils.lerp(self.zoom, self.smooth.zoom, self.lerp * dt)
    self.angle = utils.lerp(self.angle, self.smooth.angle, self.lerp * dt)
    self.x = tico.math.lerp(self.x, self.smooth.x, self.lerp)
    self.y = tico.math.lerp(self.y, self.smooth.y, self.lerp)

    -- self.x =
    -- 	utils.clamp(
    -- 	self.x,
    -- 	self.limits.left - ((self.zoom - 1) * self.width / 4),
    -- 	self.limits.right - self.width + ((self.zoom - 1) * self.width / 4)
    -- )
    self.x =
        utils.clamp(
        self.x,
        self.limits.left + (self.offset_x / self.zoom),
        self.limits.right - ((self.offset_x - self.width) / -self.zoom)
    )
    -- self.y =
    -- 	utils.clamp(
    -- 	self.y,
    -- 	self.limits.top - ((self.zoom - 1) * self.height / 4),
    -- 	self.limits.bottom - self.height + ((self.zoom - 1) * self.height / 4)
    -- )
    self.y =
    utils.clamp(
        self.y,
        self.limits.top + (self.offset_y / self.zoom),
        self.limits.bottom - ((self.offset_y - self.height) / -self.zoom)
    )
--     print(self.limits.bottom, self.offset_y, self.height, self.limits.bottom - ((self.offset_y - self.height) / -self.zoom), self.y)
--     print(self.limits.top, self.limits.top + (self.offset_y / self.zoom))
--     print(self.y)
end

---Draw the camera limits and deadzone
function Camera:draw()
    local x, y = self.deadzones.left, self.deadzones.top
    local w, h = self.deadzones.right - self.deadzones.left, self.deadzones.bottom - self.deadzones.top
--     love.graphics.setColor(1, 1, 1, 1)
    tico.graphics.drawRectangle(x, y, w, h, {255, 255, 255})
--     love.graphics.setColor(1, 0, 0, 1)
    local lx, ly = self.limits.left, self.limits.top
    local lw, lh = self.limits.right - self.limits.left, self.limits.bottom - self.limits.top
    tico.graphics.drawRectangle(lx, ly, lw, lh, {255, 0, 0})
--     print(lx, ly, lw, lh)
--     love.graphics.setColor(1, 1, 1, 1)
end

-- function Camera:bg()
--     love.graphics.draw(bg, quad)
-- end

function Camera:stats(x, y)
    x = x or 0
    y = y or 0
    love.graphics.print(self.x .. "x" .. self.y, x, y)
    love.graphics.print("zoom: " .. self.zoom, x, y + 16)
end

---Attach the camera
function Camera:attach()
    tico.graphics.push()
    tico.graphics.translate(self.offset_x / self.zoom, self.offset_y / self.zoom)
    tico.graphics.rotate(self.angle)
    tico.graphics.scale(self.zoom, self.zoom)
    tico.graphics.translate(-self.x, -self.y)
    tico.graphics.scissor(0, 0, self.width, self.height)
end

---Detach the camera
function Camera:detach()
    tico.graphics.scissor()
    tico.graphics.pop()
end

return Camera

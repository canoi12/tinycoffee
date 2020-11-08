local class = require "libs.class"
local Vector = class:extend("Vector")

local math_cos, math_sin = math.cos, math.sin
local math_rad, math_deg = math.rad, math.deg
local math_atan2 = math.atan2
local math_sqrt = math.sqrt

function Vector:constructor(x, y, z, w)
  self.x = x or 0
  self.y = y or 0
end

function Vector.zero()
  return Vector()
end

function Vector.fromAngle(angle)
  local rad_angle = math_rad(angle)
  local x = math_cos(rad_angle)
  local y = math_sin(rad_angle)
  return Vector(x, y)
end

function Vector:normalize()
    local v = self:length()
    local x = self.x / v
    local y = self.y / v
    return Vector(x, y)
end

function Vector:__add(v2)
  local v = Vector(self.x + v2.x, self.y + v2.y)
  return v
end

function Vector:__sub(v2)
  local v = Vector(self.x - v2.x, self.y - v2.y)
  return v
end

function Vector.__mul(v1, v2)
  local vector = {}
  local number = nil
  if type(v1) == "number" then
      vector = v2
      number = v1
  else
      vector = v1
      number = v2
  end
  local out = Vector(vector.x * number, vector.y * number)
  return out
end

function Vector.__div(v1, v2)
  local vector = {}
  local number = nil
  if type(v1) == "number" then
      vector = v2
      number = v1
  else
      vector = v1
      number = v2
  end

  local out = Vector(vector.x / number, vector.y / number)
  return out
end

function Vector.__eq(v1, v2)
  return v1.x == v2.x and v1.y == v2.y
end

function Vector.__unm(v1)
  v1.x = -v1.x
  v1.y = -v1.y
  return v1
end

function Vector:add(other)
	self.x = self.x + other.x
	self.y = self.y + other.y
	return self
end

function Vector:sub(other)
	self.x = self.x - other.x
	self.y = self.y - other.y
	return self
end

function Vector:mul(other)
	self.x = self.x * other
	self.y = self.y * other
	return self
end

function Vector:div(other)
	self.x = self.x / other
	self.y = self.y / other
	return self
end

function Vector:normal()
    return Vector(-self.y, self.x)
end

function Vector:dot(v2)
  return (self.x * v2.x) + (self.y * v2.y)
end

function Vector:perpdot(v2)
  return (self.x * v2.y) - (self.y * v2.x)
end

function Vector:proj(v2)
  local dot = self:dot(v2)
  local mag = v2:length()
  mag = mag * mag
  return (dot / mag) * v2
end

function Vector:perpproj(v2)
  local dot = self:dot(v2)
  local mag = v2:length()
  mag = mag * mag
  return (dot / mag) * v2:perpendicular()
end

function Vector:length(sqr)
  local length = (self.x * self.x) + (self.y * self.y)
  if not sqr then
      length = math_sqrt(length)
  end
  return length
end

function Vector:angle(v2)
  if v2 then
    return math_deg(math_atan2(v2.y - self.y, v2.x - self.x))
  end
  return math_deg(math_atan2(self.y, self.x))
end

function Vector:rotate(angle)
  angle = math_rad(angle)
  local c, s = math_cos(angle), math_sin(angle)
  local rotX = self.x * c - self.y * s
  local rotY = self.x * s + self.y * c

  self.x = rotX
  self.y = rotY
  return Vector(rotX, rotY)
end

function Vector:__tostring()
  local str = "Vector(" .. self.x .. "," .. self.y
  str = str .. ")"
  return str
end

function Vector.distance(v1, v2)
  local v = v1 - v2
  return v:length()
end

function Vector:isParallel(v2)
  local v = self:normalize()
  local w = v2:normalize()

  return v == w or v == -w
end

function Vector:insideView(direction, angle, pos, distance)
  local obj_v = pos - self
  local obj_cos = obj_v:dot(direction) / (obj_v:length() * direction:length())

  return obj_cos > math_cos(math_rad(angle))
end

function Vector:drawView(direction, angle)
--   love.graphics.setColor(1, 1, 0, 1)
  love.graphics.line(self.x, self.y, self.x + direction.x, self.y + direction.y)
  local v1 = Vector.fromAngle(angle) * direction:length()
  local v2 = Vector.fromAngle(-angle) * direction:length()
  v1:draw(self)
  v2:draw(self)

  love.graphics.setColor(1, 1, 1, 1)
end

function Vector:draw(x, y)
  if type(x) == "number" or x == nil then
    x = x or 0
    y = y or 0
--     love.graphics.line(x, y, x + self.x, y + self.y)
    tico.graphics.circle(self.x, self.y, 4)
  else
--     love.graphics.line(x.x, x.y, x.x + self.x, x.y + self.y)
    tico.graphics.circle(self.x, self.y, 4)
  end
end

return Vector

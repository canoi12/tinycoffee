local Class = {}
Class.__index = Class
Class.__class = "Class"

function Class:constructor()
end

function Class:extend(name)
	local o = setmetatable({}, self)
	for k,attr in pairs(self) do
		if k:find("__") == 1 then o[k] = attr end
	end
	o.__index = o
	o.super = self
	o.__class = name or o.__name
	o.__call = o.new
	return o
end

function Class:new(...)
	local o = setmetatable({}, self)
	o.__index = o
	o:constructor(...)
	return o
end

function Class:__tostring()
	return "Instance of " .. self.__class
end

function Class:is(class)
	local mt = getmetatable(self)

	while mt do
		if (mt == class) or (mt.__class == class) then return true end
		mt = getmetatable(mt)
	end
	return false
end

function Class:__call(...)
	return self:new(...)
end

return Class

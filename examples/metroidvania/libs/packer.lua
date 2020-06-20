---@class Packer
---@field texture Drawable
---@field width number
---@field height number
---@field root table
---@field version string
local packer = {}
packer.version = "0.0.1"

local max = math.max
local lg = love.graphics

---@class PackerImage
---@field tex Drawable
---@field w number
---@field h number
---@field quad Quad
---@field x number
---@field y number
local Image = {
    getDimensions = function(s)
        return s.w, s.h
    end,
    getWidth = function(s)
        return s.w
    end,
    getHeight = function(s)
        return s.h
    end,
    newQuad = function(s, x, y, w, h)
        local tx, ty = s.tex:getDimensions()
        return love.graphics.newQuad(s.x + x, s.y + y, w, h, tx, ty)
    end
}

-- love.graphics.newImage = function(...)
-- 	local img = lg_newimg(...)
-- 	return packer:add(img)
-- end

-- love.graphics.draw = function(image, ...)
-- 	local tex = image.tex or image
-- 	local args = {...}
-- 	local quad = args[1]
-- 	if type(quad) == "userdata" then
-- 		lg_draw(tex, ...)
-- 	else
-- 		local q = image.quad
-- 		if not image.quad then
-- 			q = args[1]
-- 			table.remove(args, 1)
-- 		end
-- 		lg_draw(tex, q, unpack(args))
-- 	end
-- end

local meta = {
    __name = "Packer",
    ---@param t table
    ---@param width number
    ---@param height number
    ---@return Packer
	__call = function(t, width, height)
		local o = {}
        o.width = width or 640
        o.height = height or 480
        o.root = {
            x = 0,
            y = 0,
            w = 0,
            h = 0
        }

        o.blocks = {}
        o.texture = lg.newCanvas(o.width, o.height)
        o.texture:setFilter("nearest", "nearest")
        o.minFilter = "nearest"
        o.magFilter = "nearest"

        o.setFilter = function(t, minFilter, magFilter)
          t.tex:setFilter(minFilter, magFilter)
        end

        return setmetatable(o, {__index = packer, __name = "Packer"})
        --return t
    end
}
meta.__index = meta

---@param filename string
---@return LunaImage
function packer:newImage(filename)
  local image = lg.newImage(filename)
  return self:add(image)
end

---@param image Image
---@return PackerImage
function packer:add(image)
    image:setFilter(self.minFilter, self.magFilter)
    local block = {
        w = image:getWidth(),
        h = image:getHeight(),
        img = image,
        tex = self.texture
    }
    block = setmetatable(block, {__index = Image})

    self.root = {
        x = 0,
        y = 0,
        w = self.width,
        h = self.height
    }

    for i, block in ipairs(self.blocks) do
        block.fit = {}
    end
    table.insert(self.blocks, block)
    table.sort(
        self.blocks,
        function(a, b)
            return max(a.w, a.h) > max(b.w, b.h)
        end
    )

    self:pack()

    return block
end

function packer:pack()
    for i, block in ipairs(self.blocks) do
        node = self:find(self.root, block.w, block.h)
        if node then
            block.fit = self:split(node, block.w, block.h)
            block.x = block.fit.x
            block.y = block.fit.y
            block.quad = lg.newQuad(block.x, block.y, block.w, block.h, self.width, self.height)
        end
    end
    lg.setCanvas(self.texture)
    lg.clear(0, 0, 0, 0)
    for i, block in ipairs(self.blocks) do
        lg.draw(block.img, block.x, block.y)
    end
    lg.setCanvas()
end

function packer:find(root, w, h)
    if root.used then
        return self:find(root.right, w, h) or self:find(root.down, w, h)
    elseif w <= root.w and h <= root.h then
        return root
    else
        return nil
    end
end

function packer:split(node, w, h)
    node.used = true
    node.down = {
        x = node.x,
        y = node.y + h,
        w = node.w,
        h = node.h - h
    }

    node.right = {
        x = node.x + w,
        y = node.y,
        w = node.w - w,
        h = node.h
    }

    return node
end

function packer:atlas()
    lg.setColor(1, 1, 1, 1)
    lg.draw(self.texture)

    for i, block in ipairs(self.blocks) do
        lg.setColor(1, 0, 0, 1)
        lg.rectangle("line", block.x, block.y, block.w, block.h)
    end
    lg.setColor(1, 1, 1, 1)
    lg.rectangle("line", 0, 0, self.width, self.height)
end

---@param image PackerImage | Drawable
---@param quad Quad
---@vararg ...@rest of the draw params
function packer:draw(image, quad, ...)
  if type(quad) == "userdata" then
      lg.draw(image.tex, quad, ...)
  else
      lg.draw(image.tex, image.quad, quad, ...)
  end
end

packer = setmetatable(packer, meta)

return packer

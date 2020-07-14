local Map = tico.class:extend("Map")

local Concord = require "libs.concord"

local KinematicSystem = require "systems.kinematicsystem"
local DrawSystem = require "systems.drawsystem"
local SpriteSystem = require "systems.spritesystem"
local PlayerInputSystem = require "systems.playerinputsystem"
local PhysicsSystem = require "systems.physicssystem"
local GravitySystem = require "systems.gravitysystem"
local PatrolSystem = require "systems.patrolsystem"
local GoblinSystem = require "systems.goblinsystem"
local SlimeSystem = require "systems.slimesystem"
local CameraSystem = require "systems.camerasystem"

local Position = require "components.position"
local Hitbox = require "components.hitbox"

-- local Tilemap = require "tilemap"
local Tilemap = tico.class:extend("Tilemap")
local Tileset = tico.class:extend("Tileset")

local Camera = require "components.camera"

function Tileset:constructor(tileset)
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

local classes = {
	Goblin = require "entities.goblin",
	Floor = require "entities.floor",
	Knight = require "entities.knight",
	Camera = require "entities.camera",
	Slime = require "entities.slime"
}


function Map:constructor(filename)
	self.world = Concord.world()
	self.world:addSystems(KinematicSystem, DrawSystem, SpriteSystem, PlayerInputSystem)
  self.world:addSystems(PhysicsSystem, GravitySystem, GoblinSystem, SlimeSystem)
  self.world:addSystems(PatrolSystem, CameraSystem)

 	-- self.classes = classes

  -- self.world:addSystems(unpack(systems))


	self.map = tico.json.load(filename)
	self.cameraId = nil
	self.layers = {}
	self.width = self.map.width
	self.height = self.map.height
	self.tilewidth = self.map.tilewidth
	self.tileheight = self.map.tileheight
	self.entities = {}
	self.tilesets = {}
	self.tilemaps = {}
	self.lightCanvas = tico.graphics.newCanvas(160, 95)
  self.world.onEntityAdded = function(wrl, e)
    if e:has(Hitbox) and e:has(Position) then
      local hitbox = e[Hitbox]
      local pos = e[Position]
      bumpWorld:add(hitbox, pos.x, pos.y, hitbox.w, hitbox.h)
    end
    self.entities[e.id] = e
  end

  self.world.onEntityRemoved = function(wrl, e)
    if e:has(Hitbox) then
      local hitbox = e[Hitbox]
      bumpWorld:remove(hitbox)
    end
    self.entities[e.id] = nil
  end

  for i,tileset in ipairs(self.map.tilesets) do
  	table.insert(self.tilesets, Tileset(tileset))
  end

	for i,layer in ipairs(self.map.layers) do
		if layer.type == "objectgroup" then
			for j,obj in ipairs(layer.objects) do
  			-- for k,v in pairs(obj) do
  			-- 	print(k, v)
  			-- end
  			if classes[obj.type] then
	  			local e = nil
	  			if obj.type == "Goblin" then
	  				local tar = obj.properties[1].value
	  				e = Concord.entity():assemble(classes[obj.type], obj.x, obj.y, tar)
	  			elseif obj.type == "Floor" then
	  				e = Concord.entity():assemble(classes[obj.type], obj.x, obj.y, obj.width, obj.height)
	  			elseif obj.type == "Knight" then
	  				e = Concord.entity():assemble(classes[obj.type], obj.x, obj.y)
	  			elseif obj.type == "Camera" then
	  				self.cameraId = obj.id
	  				e = Concord.entity():assemble(classes[obj.type], obj.x, obj.y, 160, 95, obj.properties[1].value)
	  				e[Camera].camera:setLimits(0, 0, self.width*self.tilewidth, self.height*self.tileheight)
	  			elseif obj.type == "Slime" then
	  				e = Concord.entity():assemble(classes[obj.type], obj.x, obj.y, obj.properties[1].value)
	  			end
	  			if e then
		  			e.id = obj.id
						self:add(e)
					end
				end
			end
		elseif layer.type == "tilelayer" then
			table.insert(self.tilemaps, Tilemap(layer, self.tilewidth, self.tileheight, self.tilesets[1]))
		end
	end
end

function Map:update(dt)
  self.world:emit("preupdate", dt)
  self.world:emit("update", dt)
  self.world:emit("postupdate", dt)
end

local time = 0
local lightShader = tico.graphics.newEffect([[

	uniform float u_time;

	vec4 position(vec4 pos, mat4 world, mat4 view) {
		vec4 p = pos;
		p.x += sin(pos.y * 0.02 + u_time * 8) * 5;
    p.y += cos(pos.x * 0.02 + u_time * 5) * 5;
		return world * view * p;
	}

]],
[[

	vec4 effect(vec4 color, sampler2D image, vec2 tex_coord) {
		vec2 uv = tex_coord;
		uv.y += sin(tex_coord.y * 2) * 0.05;
		vec4 col = texture2D(image, tex_coord);


		return color * col;
	}
]])
function Map:draw()
  self.world:emit("predraw")
  for i,tilemap in ipairs(self.tilemaps) do
  	tilemap:draw()
  end
  local pos = self:getEntity(18)[Position]
  self.world:emit("draw")
  local w, h = tico.graphics.getSize()

  -- tico.graphics.blendMode("multiply")
  -- tico.graphics.fillRectangle(0, 0, w, h, {255, 255, 255})
  -- tico.graphics.blendMode("alpha")


  self.lightCanvas:attach()
  tico.graphics.clear()
  lightShader:attach()
  time = time + tico.timer.delta() * 0.2
  lightShader:send("u_time", time)
  for i=1,20 do
    tico.graphics.fillCircle(pos.x, pos.y, i*2, {255, 255, 255, 255 - (i*10)})
  end
  for i=1,20 do
    tico.graphics.fillCircle(210, 160, i*2, {255, 255, 255, 255 - (i*10)})
  end
  lightShader:detach()
  self.lightCanvas:detach()

  -- local p = self:getEntity(18)[Position]
  -- image1:draw(p.x, p.y)

  -- canvas:attach()


  tico.graphics.blendMode("multiply")
  local e = nil
  if self.cameraId then
  	e = self:getEntity(self.cameraId)
  end
  if e then
  	local cam = e[Camera]
	  self.lightCanvas:draw(cam.camera.x-80, cam.camera.y-48)
	end
  tico.graphics.blendMode("alpha")


  self.world:emit("postdraw")
end

function Map:add(e)
  self.world:addEntity(e)
end

function Map:getEntity(id)
	if self.entities[id] then return self.entities[id] end
	return nil
end

return Map
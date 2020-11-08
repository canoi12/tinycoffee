local Concord = require "libs.concord"
local bump = require "libs.bump"

local world = Concord.world()

local MoveSystem = require "systems.movesystem"
local SpriteSystem = require "systems.spritesystem"
local InputSystem = require "systems.inputsystem"
local KinematicSystem = require "systems.kinematicsystem"
local GravitySystem = require "systems.gravitysystem"

local Hitbox = require "components.hitbox"
local Position = require "components.position"

local objects = {}

classes = {
	player = require "entities.player",
	lizard = require "entities.lizard",
	floor = require "entities.floor"
}

function addObject(uuid, obj)
	local obj_model = tico.resources.get("object", obj.type)
	local obj_add = {}
	for k,v in pairs(obj_model) do
		if k ~= "__meta__" then
			if not obj[k] then obj[k] = obj_model[k] end
		end
	end
	local e = Concord.entity():assemble(classes[obj.type], obj)
	world:addEntity(e)
	objects[uuid] = e
end

function tico.load()
	world:addSystems(SpriteSystem, InputSystem, KinematicSystem, GravitySystem)
	world.physics = bump.newWorld(32)

	world.onEntityAdded = function(world, e)
		if e:has(Hitbox) then
			local pos = e[Position]
			local hit = e[Hitbox]
			world.physics:add(e, pos.x, pos.y, hit.w, hit.h)
			-- print(e)
		end
	end

	world.onEntityRemoved = function(world, e)
    if e:has(Hitbox) then
      local hitbox = e[Hitbox]
      world.physics:remove(e)
    end
    -- self.entities[e.id] = nil
  end

	canvas = tico.graphics.newCanvas(160, 95)
	canvas1 = tico.graphics.newCanvas(160, 95)
	camera = tico.engine.newCamera(0, 0, 160, 95)

	map = tico.resources.get("tilemap", "forest")
	tilemap = tico.engine.newTilemap(map)


	for uuid,obj in pairs(map.objects) do
		addObject(uuid, obj)
	end
end
local x = 0

function tico.editor_update(dt)
	dt = tico.timer.delta()
	-- if tico.input.keyDown("right") then
	-- 	x = x + 100 * dt
	-- 	spr:play("walk")
	-- elseif tico.input.keyDown("left") then
	-- 	x = x - 100 * dt
	-- 	spr:play("walk")
	-- else
	-- 	spr:play("idle")
	-- end
	-- spr:update(dt)
	-- player:update(dt)
	world:emit("update", dt)
	-- local map = tico.resources.get("tilemap", "forest")
	-- for uuid,obj in pairs(map.objects) do
	-- 	if not objects[uuid] then
	-- 		local obj_model = tico.resources.get("object", obj.type)
	-- 		-- print(obj.type, obj_model)
	-- 		setmetatable(obj, {__index = obj_model})
	-- 		local e = Concord.entity():assemble(classes[obj.type], obj)
	-- 		world:addEntity(e)
	-- 		objects[uuid] = e
	-- 	end
	-- end
	-- for i,obj in ipairs(objects) do
	-- 	obj:update(dt)
	-- end
end

function tico.editor_draw()
	

	canvas:attach()
	tico.graphics.clear({75, 90, 90})
	camera:attach()
	tilemap:draw()
	-- canvas1:attach()
	-- tico.graphics.clear(0, 0, 0, 0)
	world:emit("draw")
	-- canvas1:detach()
	-- canvas1:draw()
	-- -- player:draw()
	-- for i,obj in ipairs(objects) do
	-- 	obj:draw()
	-- end
	camera:detach()
	canvas:detach()

	canvas:draw(0, 0, 0, 4, 4)
	-- canvas:auto()
	-- tico.graphics.print(tico.timer.fps(), 0, 0, {0, 0, 0})
end

function tico.editor_change(type, uuid, obj)
	if type == "object_edit" then
		local object = objects[uuid]
	
		if object:has(Position) then
			local pos = object[Position]
			pos.x = obj.position[1]
			pos.y = obj.position[2]
			world.physics:update(object, pos.x, pos.y)
		end
	elseif type == "tilemap" then
		for k,o in pairs(obj.objects) do
			local object = objects[k]

			if object then
				local pos = object[Position]
				pos.x = o.position[1]
				pos.y = o.position[2]
				world.physics:update(object, pos.x, pos.y)
			else
				addObject(k, o)
			end
		end
	end
	-- print(uuid, obj)
	-- print(objects[uuid])
end
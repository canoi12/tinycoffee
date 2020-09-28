function tico.load()
	img = tico.resources.get("image", "tileset")
	player = tico.resources.get("image", "player")
	spr = tico.engine.newSprite(player, 16, 16)
	spr:add("idle", 0, 3)
	spr:add("walk", 4, 11)
	spr:play("idle")

	canvas = tico.graphics.newCanvas(160, 95)
	camera = tico.engine.newCamera(0, 0, 160, 95)

	map = tico.resources.get("tilemap", "forest")
end

local x = 0

function tico.editor_update(dt)
	if tico.input.keyDown("right") then
		x = x + 100 * dt
		spr:play("walk")
	elseif tico.input.keyDown("left") then
		x = x - 100 * dt
		spr:play("walk")
	else
		spr:play("idle")
	end
	spr:update(dt)
	camera:move(-x+80, 46)
end

function tico.editor_draw()
	canvas:attach()
	tico.graphics.clear({75, 90, 90})
	camera:attach()
	map:draw()
	spr:draw(x, 0, 0, 1, 1, 8, 8)
	camera:detach()
	canvas:detach()

	canvas:draw(0, 0, 0, 4, 4)
	-- tico.graphics.print(tico.timer.fps(), 0, 0, {0, 0, 0})
end
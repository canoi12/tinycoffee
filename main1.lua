function tico.load()
	image = tico.graphics.newImage("tileset-1.png")
	player = tico.graphics.newImage("examples/metroidvania/assets/images/goblin_run_anim_strip_6.png")
	tileset = tico.engine.load_tileset("tileset.json", image)
	tilemap = tico.engine.new_tilemap(tileset, 32, 16)
	-- tileset = tico.engine.new_tileset(image, 16, 16)
	tico.editor.load()
	tico.editor.set_tileset(tileset)
	tico.editor.set_tilemap(tilemap)
	canvas = tico.graphics.newCanvas(320, 195)
	quads = {}
	for i=1,6 do
		table.insert(quads, tico.graphics.newRectangle((i-1)*16, 0, 16, 16))
	end
end

local x = 0
local y = 32
local time = 0
local frame = 1

function move(vx, vy)
	local tx = math.floor((x+vx) / 16)
	local ty = math.floor((y+vy) / 16)

	if not tilemap:has_tile(tx, ty) then 
		x = x + vx
		y = y + vy
	end

	
end

function tico.update(dt)
	if tico.input.isDown("left") then move(-100 * dt, 0) end
	if tico.input.isDown("right") then move(100*dt, 0) end
	if tico.input.isDown("up") then move(0, -100 * dt) end
	if tico.input.isDown("down") then move(0, 100 * dt) end
	time = time + (8 * dt)
	if time >= 1 then 
		frame = frame + 1 
		time = 0
	end
	if frame > #quads then frame = 1 end

	tilemap:update()
end

function tico.draw()
	if tico.editor.begin_draw() then
		tico.graphics.clear(0, 0, 0)
		canvas:attach()
		tico.graphics.clear(75, 90, 90)
		tilemap:draw()

		player:draw(quads[frame], x, y, 0, 1, 1, 8, 8)
		canvas:detach()

		-- canvas:auto()
		canvas:draw(0, 0, 0, 4, 4)

		tico.editor.end_draw()
	end
	tico.editor.draw()
end

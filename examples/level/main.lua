function tico.load()
  tico.resources.default()
	tico.resources.load("resources.json")
	-- tico.resources.default()
	tilemap = tico.resources.get("tilemap", "teste")
	-- tileset = tico.resources.get("tileset", "forest")
	camera = tico.engine.newCamera(0, 0, 320, 190)
	canvas = tico.graphics.newCanvas(320, 190)
	-- camera:move(150, 0)
	tico.editor.load()
	tico.editor.default()
	-- tico.editor.set_tilemap("teste")
	-- tico.editor.set_tileset("knightvania")
	-- tico.editor.set_tileset("forest")
	-- map = tico.resources.get("tilemap", "teste")
	-- tico.editor.open("tilemap", "teste")
	-- tico.editor.open("tilemap", "forest")
	-- tico.editor.open("tileset", "knightvania")
	tico.editor.game_canvas(canvas)
	rect = tico.graphics.newRectangle(0, 0, canvas:getWidth()-64, canvas:getHeight()-128)
end

local x = 0

function tico.update(dt)
	if tico.editor.game_open() then
		if tico.input.isDown("left") then x = x - 100 * dt end
		if tico.input.isDown("right") then x = x + 100 * dt end
		camera:move(-x)
	end
end

function tico.draw()
	if tico.editor.game_open() then
		canvas:attach()
		tico.graphics.clear()
		camera:attach()
		tilemap:draw()
		camera:detach()
		canvas:detach()
	end
	-- print(rect:height())
	-- canvas:draw()
	-- canvas:auto()
	-- if tico.editor.begin_draw() then
		
	-- 	-- canvas:draw(0, 0, 0, 2, 2)
	-- 	tico.editor.end_draw()
	-- end
	tico.editor.draw()
end

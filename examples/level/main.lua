function tico.load()
	tico.resources.load("resources.json")
	tilemap = tico.resources.get("tilemap", "teste")
end

function tico.update(dt)
end

function tico.draw()
	tilemap:draw()
end
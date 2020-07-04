local x = 0
local y = 0
local draw_count = 50000

if love then
	function love.update(dt)
		if love.keyboard.isDown("left") then
			x = x - 100 * dt
		elseif love.keyboard.isDown("right") then
			x = x + 100 * dt
		end
	end

	function love.draw()
		for i=1,draw_count do
			love.graphics.rectangle("line", x+i, y+i, 32, 32)
		end
		love.graphics.print(love.timer.getFPS())
	end
elseif tico then
	function tico.update(dt)
		if tico.input.isDown("left") then
			x = x - 100 * dt
		elseif tico.input.isDown("right") then
			x = x + 100 * dt
		end
	end

	function tico.draw()
		tico.graphics.clear(0, 0, 0)
		print("q")
		for i=1,draw_count do
			tico.graphics.drawRectangle(x+i, y+i, 32, 32)
		end
		tico.graphics.print(tico.timer.fps())
	end
end
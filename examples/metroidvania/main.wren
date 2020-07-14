import "tico" for TicoGame
import "tico.graphics" for Graphics, Shader, Canvas, Color

class Game is TicoGame {
	construct load() {
		_shader = Shader.outline()
		_canvas = Canvas.new(160, 95)
	}

	draw() {
		Graphics.clear()
		_canvas.attach()
		Graphics.drawRectangle(0, 0, 160, 95, Color.White)
		_canvas.detach()


		_canvas.auto()
	}
}
import "tico" for TicoGame
import "tico.graphics" for Graphics, Image, Rectangle
import "tico.input" for Input
import "tico.math" for Math

class Game is TicoGame {
    construct load() {
        _image = Image.load("assets/player.png")
        _x = 64
        _angle = 0
        _rect = Rectangle.new(0, 0, 32, 32)

        System.print(_rect.width)
    }

    update(dt) {
        if (Input.isDown("right")) {
            _x = _x + 100 * dt
        }
        if (Input.isDown("left")) {
            _x = _x - 100 * dt
        }
        _angle = _angle + dt * 15
    }

    draw() {
        Graphics.clear(75, 90, 90)
        _image.draw(_rect, _x, 64, Math.rad(_angle), 2, 2, 16, 16)
    }
}
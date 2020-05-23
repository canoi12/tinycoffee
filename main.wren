import "tico.graphics" for Render, Texture, Canvas, Color
import "tico.input" for Input, Key, MouseButton
import "tico.timer" for Timer
import "tico.audio" for Audio, Sound

class Player {
  construct new(x, y) {
    _x = x
    _y = y
    _speed = 100
    _tex = Texture.load("assets/knight.png")

    _animations = {
      "idle": [
        [0, 0, 32, 32],
        [32, 0, 32, 32],
        [64, 0, 32, 32],
        [96, 0, 32, 32]
      ]
    }
    _frame = 0
    _time = 0
  }

  update(dt) {
    if (Input.isKeyDown(Key.LEFT)) _x = _x - (_speed * dt)
    if (Input.isKeyDown(Key.RIGHT)) _x = _x + (_speed * dt)
    _time = _time + dt * 8
    if (_time >= 1) {
      _time = 0
      _frame = _frame + 1
    }
    if (_frame >= _animations["idle"].count) _frame = 0
  }

  draw() {
    _tex.drawPart(_animations["idle"][_frame], _x, _y, Color.White)
    Render.drawText("shadows of\nnothing", 30, 30, Color.White)
  }
}

class Game {
  construct load() {
    _player = Player.new(0, 0)
    _canvas = Canvas.new(160, 95)
    _color = Color.new(75, 90, 90)
    System.print("simbora")
  }
  update(dt) {
    _player.update(dt)
  }
  draw() {
    var fps = Timer.getFps()
    _canvas.set()
    Render.clear(Color.BG)
    _player.draw()
    Render.drawText(fps.toString, 0, 0, Color.White)
    _canvas.unset()

    _canvas.draw(0, 0, 4, 4, Color.White)
  }
}

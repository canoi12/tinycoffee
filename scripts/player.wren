import "tico.graphics" for Texture, Render, Color
import "tico.input" for Input, Key
import "scripts.entity" for Entity
import "tico.math" for Vector2

class Knight is Entity {
  construct new(x, y) {
    super(x, y)
    _speed = 100
    _tex = Texture.load("assets/knight.png")
    _frame = 0
    _time = 0
    _flip = 1
    _velocity = Vector2.new(0, 0)
    _animation = "idle"
    _animations = {
      "idle": [
        [0, 0, 32, 32],
        [32, 0, 32, 32],
        [64, 0, 32, 32],
        [96, 0, 32, 32]
      ],
      "walk": [
        [0, 32, 32, 32],
        [32, 32, 32, 32],
        [64, 32, 32, 32],
        [96, 32, 32, 32],
        [0, 64, 32, 32],
        [32, 64, 32, 32]
      ]
    }
  }

  update(dt) {
    _velocity = Vector2.zero()
    if (Input.isKeyDown(Key.LEFT)) {
      _velocity.x = -_speed
      _flip = -1
    }
    if (Input.isKeyDown(Key.RIGHT)) {
      _velocity.x = _speed
      _flip = 1
    }

    if (Input.isKeyDown(Key.UP)) _velocity.y = -_speed
    if (Input.isKeyDown(Key.DOWN)) _velocity.y = _speed

    if (_velocity.y != 0 || _velocity.x != 0) _animation = "walk" else _animation = "idle"

//     _velocity = _velocity * dt
//     _velocity = test
    position = position + (_velocity * dt)
//     position = test

    _time = _time + dt * 8
    if (_time >= 1) {
      _time = 0
      _frame = _frame + 1
    }
    if (_frame >= _animations[_animation].count) _frame = 0

  }

  draw() {
    _tex.drawPart(_animations[_animation][_frame], position.x, position.y, 0, _flip, 1, 16, 16, Color.White)
  }
}

import "scripts.entity" for Entity
import "tico.graphics" for Texture, Color

class Enemy is Entity {
}

class Boto is Enemy {

  construct new(x, y) {
    super(x, y)
    _tex = Texture.load("assets/boto.png")
  }

  draw() {
    _tex.draw(position.x, position.y, Color.White)
  }
}

import "tico" for Config
import "tico.math" for Vector2

class Vector {
  x { _x }
  y { _y }
  x=(value) { _x = value }
  y=(value) { _y = value }

  construct new(x, y) {
    _x = x
    _y = y
  }
}

class Game {
  static config {
//     Config.title = "Knightvania"
    Config.width = 640
    Config.height = 380
  }

  static load() {
  }

  static update(dt) {
    for (i in 1..100) {
      var a = Vector.new(0, 0)
//       var b = Vector2.new(1, 1)
    }
  }

  static draw() {
  }
}

import "tico.filesystem" for Filesystem
import "tico.graphics" for Graphics
import "tico" for TicoGame

class NoGame is TicoGame {
  construct load() {
    System.print("ok")
  }
  draw() {
    var size = Graphics.getSize()
    Graphics.print("no code loaded", size[0]/2 - 40, size[1]/2)
  }
}

class GameError is TicoGame {
  construct load() {}
  construct throw(module, line, msg) {
    _message = "error in " + module + ":%(line): " + msg
//     System.print(_message)
  }

  draw() {
    System.print("testando")
    Graphics.print("Error", 32, 32)
    Graphics.print(_message, 32, 64)
  }
}

class TicoInit {
  static init() {
    __game = null
    __gameClass = null
    if (Filesystem.fileExists("main.wren")) {
      import "main" for Game
      __gameClass = Game
    } else {
      __gameClass = NoGame
    }
  }

  static load () {
    if (__gameClass) {
      __game = __gameClass.load()
    }
  }

  static step(dt) {
    if (__game) {
      __game.update(dt)
      __game.draw()
    }
  }

  static resized(width, height) {
    if (__game) {
      __game.resized(width, height)
    }
  }

  static mousepressed(x, y) {}
  static textinput(t) {}

  static error(module, line, msg) {
    __gameClass = null
    __game = GameError.throw(module, line, msg)
//     __game = null
  }
}

// if (Filesystem.fileExists("main.wren")) {
//   import "main" for Game

//   TicoInit.set(Game)
// } else {
//   TicoInit.set(NoGame)
// }

import "tico" for Config
import "tico.graphics" for Render, Color, Canvas
import "tico.graphics" for Font
import "scripts.scene" for SceneManager, GameScene

class Game {
  static config {
    Config.title = "Knightvania"
    Config.width = 640
    Config.height = 380
  }

  static load() {
    SceneManager.load()
    SceneManager.add(GameScene.new())
    __canvas = Canvas.new(320, 190)
  }

  static update(dt) {
    SceneManager.update(dt)
  }

  static draw() {
    __canvas.set()
    Render.clear(Color.BG)
    SceneManager.draw()
    __canvas.unset()
    __canvas.draw(0, 0, 2, 2, Color.White)
  }
}

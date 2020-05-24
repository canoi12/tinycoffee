import "scripts.enemy" for Boto
import "scripts.player" for Knight
import "scripts.scene" for Scene, SceneManager
import "tico.graphics" for Canvas, Render, Color

class Game {
  construct load() {
    var scene = Scene.new()
    scene.create(Knight.new(32, 32))
    scene.create(Boto.new(32, 0))
    scene.create(Boto.new(64, 90))

    SceneManager.load()
    SceneManager.add(scene)
    _canvas = Canvas.new(320, 190)
  }
  update(dt) {
    SceneManager.update(dt)
  }
  draw() {
    _canvas.set()
    Render.clear(Color.BG)
    SceneManager.draw()
//     Render.drawRectangle(256, 32, 64, 32, Color.White)
//     Render.drawRectangle(0, 0, 320, 32, Color.White)
//     Render.fillRectangle(0, 64, 320, 32, Color.White)
    Render.fillCircle(64, 32, 8, Color.White)
    Render.drawCircle(64, 128, 16, Color.White)
    Render.drawRectangle(0, 0, 320, 190, Color.White)
    _canvas.unset()
    _canvas.draw(0, 0, 2, 2, Color.White)
  }
}

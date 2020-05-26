import "scripts.enemy" for Boto
import "scripts.player" for Knight
import "scripts.scene" for Scene, SceneManager
import "tico.graphics" for Canvas, Render, Color
import "tico.audio" for Sound
import "tico" for Config

class Game {
  static config {
  }

  static load() {
    var scene = Scene.new()
    scene.create(Knight.new(32, 32))
    scene.create(Boto.new(32, 0))
    scene.create(Boto.new(64, 90))

    SceneManager.load()
    SceneManager.add(scene)
    __canvas = Canvas.new(320, 190)
  }

  static update(dt) {
    SceneManager.update(dt)
  }

  static draw() {
    Render.clear(Color.Black)
    __canvas.set()
    Render.clear(Color.BG)
    SceneManager.draw()
//     Render.drawRectangle(256, 32, 64, 32, Color.White)
//     Render.drawRectangle(0, 0, 320, 32, Color.White)
//     Render.fillRectangle(0, 64, 320, 32, Color.White)
    Render.fillCircle(64, 32, 8, Color.White)
    Render.drawCircle(64, 128, 16, Color.White)
//     Render.drawRectangle(0, 0, 320, 190, Color.White)

    Render.drawText("aeeeewçÇçÇçÇçÇç", 0, 0, Color.White)
//     Render.drawText("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 0, 16, Color.White)
    __canvas.unset()
    __canvas.draw(0, 0, 2, 2, Color.White)
  }
}
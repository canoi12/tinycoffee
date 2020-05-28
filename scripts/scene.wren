import "tico" for Camera
import "tico.math" for Vector2
import "tico.graphics" for Render, Color
import "tico.graphics" for Font
import "scripts.player" for Knight


class SceneManager {
  static add(scene) {
    __current = scene
  }

  static load() {
    __scenes = []
    __current = Null
  }

  static update(dt) {
    if (__current != Null) __current.update(dt)
  }

  static draw() {
    if (__current != Null) __current.draw()
  }
}

class Scene {
  objects {_objects}
  objects=(value) { _objects = value }
  camera {_camera}
  camera=(value) { _camera = value }

  construct new() {
    objects = []
    camera = Camera.new(0, 0, 320, 190)
    camera.center = Vector2.new(160, 95)
    camera.zoom = 1
    _font = Font.load("assets/extrude.ttf", 16)
  }

  create(object) {
    objects.add(object)
  }

  update(dt) {
    for (object in objects) {
      object.update(dt)
    }
  }
  draw() {
    camera.attach()
    for (object in objects) {
      object.draw()
    }
    Render.drawRectangle(0, 0, 32, 32, Color.White)
    Render.drawCircle(90, 90, 15, Color.White)
    _font.print("aeeeeeeee", 0, 0, Color.White)
    Font.print("aaeaeasdasdadaewa", 0, 32, Color.Black)
    camera.detach()
  }
}

class GameScene is Scene {
  construct new() {
    super()

    _player = Knight.new(0, 0)
    create(_player)
  }

  update(dt) {
    super.update(dt)
    camera.follow(_player.position)
  }
}

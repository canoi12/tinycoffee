class SceneManager {
  static add(scene) {
    __current = scene
  }

  static load() {
    __scenes = []
    __current = Null
  }

  static update(dt) {
    if (__current) __current.update(dt)
  }

  static draw() {
    if (__current) __current.draw()
  }
}

class Scene {
  objects {_objects}
  construct new() {
    _objects = []
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
    for (object in objects) {
      object.draw()
    }
  }
}

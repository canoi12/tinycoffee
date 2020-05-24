import "tico.math" for Transform

class Entity {
  position { transform.position }
  position=(value) { transform.position = value }
  scale { transform.scale }
  scale=(value) { transform.scale = value }
  angle { transform.angle }
  angle=(value) { transform.angle = angle }
  transform { _transform }
  transform=(t) { _transform = t }

  construct new(x, y) {
    this.transform = Transform.new(x, y)
  }

  update(dt) {}
  draw() {}
}

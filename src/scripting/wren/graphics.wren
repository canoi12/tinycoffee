foreign class Graphics {
  foreign static drawRectangle(x, y, width, height)
  foreign static drawRectangle(x, y, width, height, color)
  foreign static fillRectangle(x, y, width, height)
  foreign static fillRectangle(x, y, width, height, color)

  foreign static drawCircle(x, y, radius)
  foreign static drawCircle(x, y, radius, color)
  foreign static drawCircle(x, y, radius, sides, color)
  foreign static fillCircle(x, y, radius)
  foreign static fillCircle(x, y, radius, color)
  foreign static fillCircle(x, y, radius, sides, color)

  foreign static drawTriangle(x0, y0, x1, y1, x2, y2)
  foreign static drawTriangle(x0, y0, x1, y1, x2, y2, color)
  foreign static fillTriangle(x0, y0, x1, y1, x2, y2)
  foreign static fillTriangle(x0, y0, x1, y1, x2, y2, color)

  foreign static print(text, x, y)
  foreign static print(text, x, y, color)
  foreign static printf(text, x, y, sx, sy)
  foreign static printf(text, x, y, sx, sy, color)

  foreign static clear()
  foreign static clear(r, g, b)
  foreign static clear(r, g, b, a)
  foreign static clear(color)

  foreign static scissor()
  foreign static scissor(x, y, w, h)

  foreign static push()
  foreign static pop()
  foreign static translate(x, y)
  foreign static rotate(angle)
  foreign static scale(x, y)

  foreign static getWidth()
  foreign static getHeight()
  foreign static getSize()
}

foreign class Shader {
  construct new(vert, frag) {}
  construct new(frag) {}

  foreign static GBA()
  foreign static outline()

  foreign attach()
  foreign detach()

  foreign sendFloat(name, value)
  foreign sendVec2(name, value)
  foreign sendVec3(name, value)
  foreign sendVec4(name, value)
}

foreign class Image {
  construct new(data, w, h) {}
  construct load(filename) {}

  foreign getWidth()
  foreign getHeight()
  foreign getSize()

  foreign draw(x, y)
  foreign draw(x, y, color)
  foreign draw(x, y, angle, sx, sy, cx, cy)
  foreign draw(x, y, angle, sx, sy, cx, cy, color)
}

foreign class Canvas {
  construct new(width, height) {}

  foreign attach()
  foreign detach()

  foreign getWidth()
  foreign getHeight()
  foreign getSize()

  foreign draw(x, y)
  foreign draw(x, y, color)
  foreign draw(x, y, angle, sx, sy, cx, cy)
  foreign draw(x, y, angle, sx, sy, cx, cy, color)
  foreign auto()
}

class Color {
  static initColors() {
    __red = rgb(255, 0, 0)
    __white = rgb(255, 255, 255)
    __black = rgb(0, 0, 0)
  }

  static rgb(r, g, b) { [r, g, b, 255] }

  static rgba(r, g, b, a) { [r, g, b, a] }

  static Red { __red }
  static White { __white }
  static Black { __black }
}

Color.initColors()
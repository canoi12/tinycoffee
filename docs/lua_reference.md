## sumary
- [**`tico`**](#tico)
	- [**`getVersion`**](#tico.getVersion)
	- [**`class`**](#tico.class)
	- [**`audio`**](#tico.audio)
		- [**`setVolume`**](#tico.audio.setVolume)
		- [**`newSound`**](#tico.audio.newSound)
	- [**`input`**](#tico.input)
		- [**`isDown`**](#tico.input.isDown)
		- [**`isPressed`**](#tico.input.isPressed)
		- [**`isUp`**](#tico.input.isUp)
		- [**`isReleased`**](#tico.input.isReleased)
		- [**`isKeyDown`**](#tico.input.isKeyDown)
		- [**`isKeyPressed`**](#tico.input.isKeyPressed)
		- [**`isKeyUp`**](#tico.input.isKeyUp)
		- [**`isKeyReleased`**](#tico.input.isKeyReleased)
		- [**`isMouseDown`**](#tico.input.isMouseDown)
		- [**`isMousePressed`**](#tico.input.isMousePressed)
		- [**`isMouseUp`**](#tico.input.isMouseUp)
		- [**`isMouseReleased`**](#tico.input.isMouseReleased)
		- [**`getMousePos`**](#tico.input.getMousePos)
		- [**`mouseScroll`**](#tico.input.getMousePos)
	- [**`graphics`**](#tico.graphics)
		- [**`getSize`**](#tico.graphics.getSize)
		- [**`clear`**](#tico.graphics.clear)
		- [**`newImage`**](#tico.graphics.newimage)
		- [**`newCanvas`**](#tico.graphics.newCanvas)
		- [**`newRectangle`**](#tico.graphics.newRectangle)
		- [**`newFont`**](#tico.graphics.newFont)
		- [**`drawRectangle`**](#tico.graphics.drawRectangle)
		- [**`fillRectangle`**](#tico.graphics.fillRectangle)
		- [**`drawCircle`**](#tico.graphics.drawCircle)
		- [**`fillCircle`**](#tico.graphics.fillCircle)
		- [**`drawTriangle`**](#tico.graphics.drawTriangle)
		- [**`fillTriangle`**](#tico.graphics.fillTriangle)
		- [**`line`**](#tico.graphics.line)
		- [**`print`**](#tico.graphics.print)
		- [**`printf`**](#tico.graphics.printf)


# <a name="tico"></a>tico module

## <a name="tico.getVersion"></a>tico.getVersion()

- return:
	- **`version`**: **string**

# <a name="tico.graphics"></a>graphics module

## <a name="tico.graphicsgetSize"></a>tico.graphics.getSize()

Returns the current window size

-	return:
	- **`width`**: **number**
	- **`height`**: **number**

```lua
local w, h = tico.graphics.getSize()
```

## <a name="tico.graphics.clear"></a>tico.graphics.clear(r, g, b, a)
## tico.graphics.clear(color)

Clear the current framebuffer

If none argument is passed, clear to black

- args:
	- **`r`**: **number**
	- **`g`**: **number**
	- **`b`**: **number**
	- **`a`**: **number** (255)

- args:
	- **`color`**: **table** ({0, 0, 0})

```lua
function tico.draw()
	tico.graphics.clear(255, 0, 0)
end
```

```lua
Color = require "tico.color"

function tico.draw()
	tico.graphics.clear(Color.Red)
end
```

```lua
function tico.draw()
	tico.graphics.clear({255, 0, 0})
end
```


## <a name="tico.graphics.newCanvas"></a>tico.graphics.newCanvas(width, height)

Create a new canvas (OpenGL framebuffer)

- args:
	- **`width`**: **number**	
	- **`height`**: **number**	

- return:
  - **`Canvas`**: **userdata**

### Canvas functions:

- [Canvas:draw](#canvas:draw)
- [Canvas:auto](#canvas:auto)
- [Canvas:getWidth](#canvas:getWidth)
- [Canvas:getHeight](#canvas:getHeight)
- [Canvas:getSize](#canvas:getSize)
- [Canvas:attach](#canvas:attach)
- [Canvas:detach](#canvas:detach)


#### <a name="canvas:draw"></a>:draw(x, y, angle, scaleX, scaleY, centerX, centerY, color)
#### :draw(x, y, color)

Draw the canvas

All arguments are optional

- args:
	- **`x`**: **number** (0)
	- **`y`**: **number** (0)
	- **`angle`**: **number** (0)
	- **`scaleX`**: **number** (1)
	- **`scaleY`**: **number** (1)
	- **`centerX`**: **number** (0)
	- **`centerY`**: **number** (0)
	- **`color`**: **table** ({255, 255, 255})

#### <a name="canvas:draw"></a>:auto()

Draw canvas and auto scale to fit window size but maintain the ratio

#### <a name="canvas:getWidth"></a>:getWidth()

Get the canvas texture width

- return:
  - **`width`**: **number**

#### <a name="canvas:getHeight"></a>:getHeight()

Get the canvas texture height

- return:
	- **`height`**: **number**

#### <a name="canvas:getSize"></a>:getSize()

Get the canvas texture size

- return:
	- **`width`**: **number**
	- **`height`**: **number**

#### <a name="canvas:attach"></a>:attach()

Attach the Canvas

#### <a name="canvas:detach"></a>:detach()

Detach the Canvas, backing to the default framebuffer

```lua
function tico.load()
	canvas = tico.graphics.newCanvas(320, 240)
end

function tico.draw()
	canvas:attach()
	tico.graphics.clear()
	tico.graphics.drawRectangle(0, 0, 32, 32)
	canvas:detach()

	canvas:auto()
end

```

## <a name="tico.graphics.newImage">tico.graphics.newImage(filename)

Creates a new image

- args:
	- **`filename`**: **string**

- return:
	- **`Image`**: **userdata**

### Image functions

- [**`Image:draw`**](#image:draw)
- [**`Image:getWidth`**](#image:getWidth)
- [**`Image:getHeight`**](#image:getHeight)
- [**`Image:getSize`**](#image:getSize)

#### <a name="image:draw"></a>:draw(x, y, angle, scaleX, scaleY, centerX, centerY, color)

Draw the image

All arguments are optional

- args:
	- **`x`**: **number** (0)
	- **`y`**: **number** (0)
	- **`angle`**: **number** (0)
	- **`scaleX`**: **number** (1)
	- **`scaleY`**: **number** (1)
	- **`centerX`**: **number** (0)
	- **`centerY`**: **number** (0)
	- **`color`**: **table** ({255, 255, 255})

#### <a name="image:getWidth"></a>:getWidth()

Get the image's width

- return:
	- **`width`**: **number**

#### <a name="image:getHeight"></a>:getHeight()

Get the image's height

- return:
	- **`height`**: **number**

#### <a name="image:getSize"></a>:getSize()

Get the image's size

- return:
	- **`width`**: **number**
	- **`height`**: **number**

```lua
function tico.load()
	image = tico.graphics.newImage("image.png")
end

function tico.draw()
	image:draw(32, 32)
end

```

## <a name="tico.graphics.newRectangle">tico.graphics.newRectangle(x, y, w, h)

Creates a new Rectangle

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`w`**: **number**
	- **`h`**: **number**

- return:
	- **`Rectangle`**: **userdata**

### Rectangle functions

- [Rectangle:x](#rectangle:x)
- [Rectangle:y](#rectangle:y)
- [Rectangle:width](#rectangle:width)
- [Rectangle:height](#rectangle:height)

#### <a name="rectangle:width"></a>:width()

Get the rectangle width

- return
	- **`width`**: **number**

#### <a name="rectangle:height"></a>:height()

Get the rectangle height

- return
	- **`height`**: **number**

#### <a name="rectangle:x"></a>:x()

Get the rectangle x

- return
	- **`x`**: **number**

#### <a name="rectangle:y"></a>:y()

Get the rectangle y

- return
	- **`y`**: **number**


## <a name="tico.graphics.drawRectangle"></a>tico.graphics.drawRectangle(x, y, w, h, color)

Draw a lined rectangle

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`w`**: **number**
	- **`h`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.fillRectangle"></a>tico.graphics.fillRectangle(x, y, w, h, color)

Draw a filled rectangle

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`w`**: **number**
	- **`h`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.drawCircle"></a>tico.graphics.drawCircle(x, y, radius, color)

Draw a lined circle

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`radius`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.fillCircle"></a>tico.graphics.fillCircle(x, y, radius, color)

Draw a filled circle

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`radius`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.drawTriangle"></a>tico.graphics.drawTriangle(x0, y0, x1, y1, x2, y2, color)

Draw a lined triangle

- args:
	- **`x0, x1, x2`**: **number**
	- **`y0, y1, y2`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.fillTriangle"></a>tico.graphics.fillTriangle(x0, y0, x1, y1, x2, y2, color)

Draw a lined triangle

- args:
	- **`x0, x1, x2`**: **number**
	- **`y0, y1, y2`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.line"></a>tico.graphics.line(x0, y0, x1, y1, color)

Draw a line

-args:
	- **`x0, x1`**: **number**
	- **`y0, y1`**: **number**
	- **`color`**: **table** ({255, 255, 255})

## <a name="tico.graphics.scissor"></a>tico.graphics.scissor(x, y, w, h)

Set a scissor

- args:
	- **`x`**: **number**
	- **`y`**: **number**
	- **`w`**: **number**
	- **`h`**: **number**

## <a name="tico.graphics.print"></a>tico.graphics.print(text, x, y, color)
## tico.graphics.print(Font, text, x, y, color)

Draw text on the screen

- args:
	- **`font`**: **Font** (defaultFont)
	- **`text`**: **string**
	- **`x`**: **number**
	- **`y`**: **number**
	- **`color`**: **number**

## <a name="tico.graphics.printf"></a>tico.graphics.printf(text, x, y, sx, sy, color)
## tico.graphics.printf(Font, text, x, y, sx, sy, color)

extended [`tico.graphics.print`](#tico.graphics.print)
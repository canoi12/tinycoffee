## sumary
- [**`tico`**](#tico)
	- [**`getVersion`**](#tico.getVersion)
	- [**`class`**](#tico.class)
	- [**`color`**](#tico.color)
		- [**`norm`**](#tico.color.norm)
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
		- [**`isJoyDown`**](#tico.input.isJoyDown)
		- [**`isJoyPressed`**](#tico.input.isJoyPressed)
		- [**`isJoyUp`**](#tico.input.isJoyUp)
		- [**`isJoyReleased`**](#tico.input.isJoyReleased)
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
	- [**`window`**](#tico.window)
		- [**`getSize`**](#tico.window.getSize)
		- [**`getTitle`**](#tico.window.getTitle)
		- [**`setTitle`**](#tico.window.setTitle)
		- [**`getWidth`**](#tico.window.getWidth)
		- [**`setWidth`**](#tico.window.setWidth)
		- [**`getHeight`**](#tico.window.getHeight)
		- [**`setHeight`**](#tico.window.setHeight)
	- [**`timer`**](#tico.timer)
		- [**`delta`**](#tico.timer.delta)
		- [**`fps`**](#tico.timer.fps)
	- [**`math`**](#tico.math)
		- [**`lerp`**](#tico.math.lerp)
		- [**`round`**](#tico.math.round)
		- [**`clamp`**](#tico.math.clamp)
		- [**`angle`**](#tico.math.angle)
		- [**`distance`**](#tico.math.distance)
		- [**`sign`**](#tico.math.sign)
	- [**`filesystem`**](#tico.filesystem)
		- [**`read`**](#tico.filesystem.read)
		- [**`write`**](#tico.filesystem.write)
		- [**`fileExists`**](#tico.filesystem.fileExists)
		- [**`setPath`**](#tico.filesystem.setPath)
		- [**`getPath`**](#tico.filesystem.getPath)
		- [**`exePath`**](#tico.filesystem.exePath)
		- [**`resolvePath`**](#tico.filesystem.resolvePath)
		- [**`mkdir`**](#tico.filesystem.mkdir)
		- [**`rmdir`**](#tico.filesystem.rmdir)
	- [**`json`**](#tico.json)
		- [**`decode`**](#tico.json.decode)
		- [**`encode`**](#tico.json.encode)
		- [**`load`**](#tico.json.load)
		- [**`save`**](#tico.json.save)


# <a name="tico"></a>tico module

## <a name="tico.getVersion"></a>tico.getVersion()

- return:
	- **`version`**: **string**

# <a name="tico.audio"></a>audio module

## <a name="tico.audio.setVolume"></a>tico.audio.setVolume(volume)

Set the master volume

- args:
	- **`volume`**: **number**

## <a name="tico.audio.newSound"></a>tico.audio.newSound(filename)

Create a new Sound

*The current supported formats are: mp3, ogg, wav and flac*

- args:
	- **`filename`**: **string**

- return:
	- **`Sound`**: **userdata**

### Sound functions:

- [**`Sound:play`**](#sound:play)
- [**`Sound:pause`**](#sound:pause)
- [**`Sound:stop`**](#sound:stop)
- [**`Sound:volume`**](#sound:volume)
- [**`Sound:isPlaying`**](#sound:isPlaying)
- [**`Sound:isPaused`**](#sound:isPaused)

#### <a name="sound:play"></a>:play()

Play the current sound

#### <a name="sound:pause"></a>:pause()

Pause the current sound (don't rewind)

#### <a name="sound:stop"></a>:stop()

Stop the current sound (rewind sound)

#### <a name="sound:volume">:volume(volume)

Set and get the volume of the sound

- args:
	- **`volume`**: **number**

- return:
	- **`volume`**: **number**

#### <a name="sound:isPlaying"></a>:isPlaying()

Get if sound is playing

- return:
	- **`playing`**: **boolean**

#### <a name="sound:isPaused"></a>:isPaused()

Get if sound is paused

- return:
	- **`paused`**: **boolean**

```lua
function tico.load()
	sound = tico.audio.newSound("sound.mp3")
end


function tico.update(dt)
	if tico.input.isDown("mouse1") then sound:play() end
end

```

# <a name="tico.input"></a>input module

## <a name="tico.input.isDown"><a/>tico.input.isDown(name)

Return true if the key/mouse button with the given name is pressed

- args:
	- **`name`**: **string** (see `input names`)

- return:
	- **`isDown`**: **boolean**

## <a name="tico.input.isPressed"><a/>tico.input.isPressed(name)

Return true if the key/mouse button with the given name was pressed

- args:
	- **`name`**: **string** (see `input names`)

- return:
	- **`isPressed`**: **boolean**

## <a name="tico.input.isUp"><a/>tico.input.isUp(name)

Return true if the key/mouse button with the given name is not pressed

- args:
	- **`name`**: **string** (see `input names`)

- return:
	- **`isUp`**: **boolean**

## <a name="tico.input.isReleased"><a/>tico.input.isReleased(name)

Return true if the key/mouse button with the given name was released

- args:
	- **`name`**: **string** (see `input names`)

- return:
	- **`isReleased`**: **boolean**

## <a name="tico.input.isKeyDown"><a/>tico.input.isKeyDown(key)

Return true if the given key is pressed

- args:
	- **`key`**: **string** (see `input names`)

- return:
	- **`isKeyDown`**: **boolean**

## <a name="tico.input.isKeyPressed"><a/>tico.input.isKeyPressed(key)

Return true if the given key was pressed

- args:
	- **`key`**: **string** (see `input names`)

- return:
	- **`isKeyPressed`**: **boolean**

## <a name="tico.input.isKeyUp"><a/>tico.input.isKeyUp(key)

Return true if the given key is not pressed

- args:
	- **`key`**: **string** (see `input names`)

- return:
	- **`isKeyUp`**: **boolean**

## <a name="tico.input.isKeyReleased"><a/>tico.input.isKeyReleased(key)

Return true if the given key was released

- args:
	- **`key`**: **string** (see `input names`)

- return:
	- **`isKeyReleased`**: **boolean**

## <a name="tico.input.isMouseDown"><a/>tico.input.isMouseDown(button)

Return true if the given mouse button is pressed

- args:
	- **`button`**: **string** (see `input names`)

- return:
	- **`isMouseDown`**: **boolean**

## <a name="tico.input.isMousePressed"><a/>tico.input.isMousePressed(button)

Return true if the given mouse button was pressed

- args:
	- **`button`**: **number** (see `input names`)

- return:
	- **`isMousePressed`**: **boolean**

## <a name="tico.input.isMouseUp"><a/>tico.input.isMouseUp(button)

Return true if the given mouse button is not pressed

- args:
	- **`button`**: **number** (see `input names`)

- return:
	- **`isMouseUp`**: **boolean**

## <a name="tico.input.isReleased"><a/>tico.input.isReleased(button)

Return true if the given mouse button was released

- args:
	- **`button`**: **number** (see `input names`)

- return:
	- **`isReleased`**: **boolean**

## <a name="tico.input.isJoyDown"><a/>tico.input.isJoyDown(jid, name)

Return true if the joystick/gamepad button is pressed, `jid` is the number of the joystick

- args:
	- **`jid`**: **number**
	- **`name`**: **string** (see `input names`)

- return:
	- **`isJoyDown`**: **boolean**

## <a name="tico.input.isJoyPressed"><a/>tico.input.isJoyPressed(name)

Return true if the joystick/gamepad button was pressed, `jid` is the number of the joystick

- args:
	- **`jid`**: **number**
	- **`name`**: **string** (see `input names`)

- return:
	- **`isJoyPressed`**: **boolean**

## <a name="tico.input.isJoyUp"><a/>tico.input.isJoyUp(name)

Return true if the joystick/gamepad button is not pressed, `jid` is the number of the joystick

- args:
	- **`jid`**: **number**
	- **`name`**: **string** (see `input names`)

- return:
	- **`isJoyUp`**: **boolean**

## <a name="tico.input.isJoyReleased"><a/>tico.input.isJoyReleased(name)

Return true if the joystick/gamepad button was released, `jid` is the number of the joystick

- args:
	- **`jid`**: **number**
	- **`name`**: **string** (see `input names`)

- return:
	- **`isJoyReleased`**: **boolean**

# <a name="tico.math"></a>math module

## <a name="tico.math.lerp"></a>tico.math.lerp(init, end, t)

Return the linear interpolation between `init` and `end` for the given `t`

- args:
	- **`init`**: **number**
	- **`end`**: **number**
	- **`t`**: **number**

- return:
	- **`lerpVal`**: **number**

## <a name="tico.math.clamp"></a>tico.math.clamp(v, min, max)

Bounds `v` between `min` and `max`.

`if x < min` returns `min`
`if x > max` returns `max`

- args:
	- **`v`**: **number**
	- **`min`**: **number**
	- **`max`**: **number**

- return:
	- **`val`**: **number**

## <a name="tico.math.round"></a>tico.math.round(value)

Round `value`

- args:
	- **`value`**: **number**

- return:
	- **`roundedNumber`**: **number**

## <a name="tico.math.sign"></a>tico.math.sign(value)

Sign `value`
`if x < 0` return -1
`if x > 0` return 1
`if x == 0` return 0

- args:
	- **`value`**: **number**

- return:
	- **`signedValue`**: **number**

## <a name="tico.math.distance"></a>tico.math.distance(val1, val2)

Returns the distance between `val1` and `val2`

- args:
	- **`val1`**: **number**
	- **`val2`**: **number**

- return:
	- **`distance`**: **number**

## <a name="tico.math.angle"></a>tico.math.angle(x0, y0, x1, y1)

Return the angle between the two points

- args:
	- **`x0`**: **number**
	- **`y0`**: **number**
	- **`x1`**: **number**
	- **`y1`**: **number**

- return:
	- **`angle`**: **number**

# <a name="tico.filesystem"></a>filesystem module

## <a name="tico.filesystem.read"></a>tico.filesystem.read(filename)

Read content from file

- args:
	- **`filename`**: **string**

- return:
	- **`file_content`**: **string**

## <a name="tico.filesystem.write"></a>tico.filesystem.write(filename, text, mode)

Write content to file, if file don't exists, create one.

- args:
	- **`filename`**: **string**
	- **`text`**: **string**
	- **`mode`**: **string** ("w")

## <a name="tico.filesystem.fileExists"></a>tico.filesystem.fileExists(filename)

Check if the file exists

- args:
 - **`filename`**: **string**

- return:
	- **`fileExists`**: **boolean**

## <a name="tico.filesystem.setPath"></a>tico.filesystem.setPath(path)
## <a name="tico.filesystem.getPath"></a>tico.filesystem.getPath()

Get the current project path

- return:
	- **`project_path`**: **string**

## <a name="tico.filesystem.exePath"></a>tico.filesystem.exePath()

Get the exe path

- return:
	- **`exe_path`**: **string**

## <a name="tico.filesystem.resolvePath"></a>tico.filesystem.resolvePath(filename)

Resolve the filename using the current project_path

- args:
	- **`filename`**: **string**

- return:
	- **`resolved_path`**: **string**

## <a name="tico.filesystem.mkdir"></a>tico.filesystem.mkdir(name)

Create a directory

- args:
	- **`name`**: **string**

## <a name="tico.filesystem.rmdir"></a>tico.filesystem.rmdir(name)

Remove a directory

- args:
	- **`name`**: **string**

# <a name="tico.json"></a>json module

## <a name="tico.json.encode"></a>tico.json.encode(table)

Encodes a Lua table in a JSON string

- args:
	- **`table`**: **table**

- return:
	- **`json_string`**: **string**

## <a name="tico.json.decode"></a>tico.json.decode(str)

Decodes a JSON string to a Lua table

- args:
	- **`str`**: **string**

- return:
	- **`table`**: **table**

## <a name="tico.json.load"></a>tico.json.load(filename)

Load a JSON file to a Lua table

- args:
	- **`filename`**: **string**

- return:
	- **`table`**: **table**

## <a name="tico.json.save"></a>tico.json.save(filename, table)

Save a Lua table to a JSON file

- args:
	- **`filename`**: **string**
	- **`table`**: **table**

# <a name="tico.window"></a>window module
# <a name="tico.timer"></a>timer module

## <a name="tico.timer.delta"></a>tico.timer.delta()

Return the current delta time

- return:
	- **`delta`**: **number**

## <a name="tico.timer.fps"></a>tico.timer.fps()

Return the current FPS

- return:
	- **`fps`**: **number**

# <a name="tico.graphics"></a>graphics module

## <a name="tico.graphics.getSize"></a>tico.graphics.getSize()

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

- [**`Canvas:draw`**](#canvas:draw)
- [**`Canvas:auto`**](#canvas:auto)
- [**`Canvas:getWidth`**](#canvas:getWidth)
- [**`Canvas:getHeight`**](#canvas:getHeight)
- [**`Canvas:getSize`**](#canvas:getSize)
- [**`Canvas:attach`**](#canvas:attach)
- [**`Canvas:detach`**](#canvas:detach)


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

`sx` and `sy` are the scale factor

- args:
	- **`font`**: **Font** (defaultFont)
	- **`text`**: **string**
	- **`x`**: **number**
	- **`y`**: **number**
	- **`sx`**: **number**
	- **`sy`**: **number**
	- **`color`**: **number**
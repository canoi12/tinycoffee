![tinycoffee-banner](https://user-images.githubusercontent.com/15099243/87436458-7296c700-c5c3-11ea-9738-ed679731de7e.png)

# Tiny Coffee

[![Join the chat at https://gitter.im/tinycoffee/tinycoffee](https://badges.gitter.im/tinycoffee/tinycoffee.svg)](https://gitter.im/tinycoffee/tinycoffee?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Build Status](https://travis-ci.org/canoi12/tinycoffee.svg?branch=master)](https://travis-ci.org/canoi12/tinycoffee)

Tiny Coffee is a framework to develop simple games 2d games using opengl 3.

**For now it is far to be complete, it is better for you to use other game engine or framework if you working on a commercial project.

The main idea is to have all the dependencies static compiled in the executable, so there is no need of external dynamic libs other than platform specifics.

I'm using in this project:

- [GLFW](https://www.glfw.org/)
- [gl3w](https://github.com/skaslev/gl3w)
- [lua](https://www.lua.org/)
- [wren](http://wren.io/)
- [miniaudio](https://github.com/dr-soft/miniaudio/) (include dr_flac, dr_mp3 and dr_wav)
- [stb libs](https://github.com/nothings/stb) (stb_image, stb_vorbis and stb_truetype)
- [cJSON](https://github.com/DaveGamble/cJSON)
- [zip](https://github.com/kuba--/zip) (wrap for [miniz](https://github.com/richgel999/miniz))
- ~[freetype](https://www.freetype.org/)~ (changed to `stb_truetype`, but maybe give the option to use it in the future (?))
- [cimgui](https://github.com/cimgui/cimgui/) / [imgui](https://github.com/ocornut/imgui/)
- [rxi libs](https://github.com/rxi) (map, vec)

## TODO:

- [x] draw outlined shapes
- [x] draw triangles and circles
- [x] user created spritebatch
- [x] load config from json
- [ ] wrap for lua (work in progress, focusing on this one)
- [ ] wrap for wren (work in progress, stopped by now)
- [x] json parser
- [ ] autopack textures on the fly
- [x] joystick support (initial stage)
- [ ] pollish the main engine modules (graphics, filesystem, audio, math, input)
- [ ] make specific game types like camera, sprite, tileset..
- [ ] fuse game executable with zip (like love2d)
- [ ] make simple editors (sprite, tileset, scene..)
- [x] user custom shaders (like love2d and gms, with default variables for attributes and uniforms)
- [ ] support for more audio types
- [x] struct with options to init engine (need to add more options)
- [ ] load audio static (decode in memory)
- [x] drawing textures
- [x] drawing canvas
- [x] custom glsl shaders (but the shaders need to implement some attribute variables and uniforms)
- [x] keyboard and mouse input
- [ ] plugins system (in progress)
  - [ ] resource manager
    - [ ] audio
    - [x] image
    - [ ] font
    - [ ] shader
    - [ ] sprite
    - [ ] tilemap / scene
    - [x] tileset
  - [ ] editor
    - [x] tileset
    - [ ] tilemap / scene
  - [ ] lua
  - [ ] wren

## usage

if you want to use your own game loop using Tico static lib, use:

```c
// main.c
#include "tico.h"

int main(int argc, char ** argv) {
  // tc_Config config = tic_config_init("title", 640, 380, argc, argv);
  tc_Config config = tico_config_init("title", 640, 380, argc, argv);
  tc_bool success = tico_init(&config);

  tc_Image image = tico_image_load(filename);

  while (!tico_window_should_close()) { // main loop
    tico_update(); // update inputs, timers, ...

    tico_graphics_clear(color3(75, 90, 90)); // clear screen with color

    tico_begin_draw(); // begin batch render

    tico_image_draw(tex, 0, 0, WHITE); // draw texture
    tico_image_draw_scale(tex, 64, 0, 4, 4, WHITE); // draw scaled texture

    tico_end_draw(); // finish batch render
  }

  tico_terminate();

  return 0;
}

```

main.lua structure:

```lua

function tico.load()
  canvas = tico.graphics.newCanvas(160, 95)
  image = tico.graphics.newImage("image.png")

  rect = tico.graphics.newRectangle(0, 0, 32, 32)
end

function tico.update(dt)
end

function tico.draw()
  canvas:attach()
  tico.graphics.fillRectangle(32, 32, 32, 32)
  image:draw(rect, 64, 64)
  canvas:detach()
  canvas:draw(0, 0)
end

```

## build

### Dependencies
  - **`Linux`**
    - xorg-dev
    - libgl1-mesa-dev
    - gcc
    - gcc-mingw-w64-x86-64 (to build for Windows)
    - make
    - cmake
  - **`Windows`**
    - cmake
    - mingw

for now i'm developing on linux, so is easier to compile on it. 

#### Linux

```
mkdir build
cd build
cmake ..
make
```

use `cmake .. -DWIN32` to compile for Windows using mingw gcc

#### Windows

don't tested yet, but you need to use cmake and probably install mingw


## distribution

pack all your game assets (images, sound, font, scripts) in a zip called `data.pack`.
If you are using Lua or Wren, maitain the `main.lua` file in the zip root

## screenshots

![tico-0 1 5](https://user-images.githubusercontent.com/15099243/86610476-c716c280-bf83-11ea-9a56-8234d77ac150.gif)
![tico-shader-palette](https://user-images.githubusercontent.com/15099243/86610557-e6155480-bf83-11ea-9c46-d4244f686afa.gif)


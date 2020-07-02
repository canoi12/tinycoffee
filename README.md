# Tiny Coffee lib

[![Join the chat at https://gitter.im/tinycoffee/tinycoffee](https://badges.gitter.im/tinycoffee/tinycoffee.svg)](https://gitter.im/tinycoffee/tinycoffee?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/canoi12/tinycoffee.svg?branch=master)](https://travis-ci.org/canoi12/tinycoffee)

Tiny Coffee is a framework to develop simple games 2d games using opengl 3.

For now it is far to be complete, it is better for you to use other game engine or framework if you working on a commercial project.

The main idea is to have all the dependencies static compiled in the executable, so there is no need of external dynamic libs other than platform specifics.

I'm using in this project:

- [GLFW](https://www.glfw.org/)
- [gl3w](https://github.com/skaslev/gl3w)
- [lua](https://www.lua.org/)
- [wren](http://wren.io/)
- [miniaudio](https://github.com/dr-soft/miniaudio/) (include dr_flac, dr_mp3 and dr_wav)
- [stblibs](https://github.com/nothings/stb) (stb_image, stb_vorbis and stb_truetype)
- [cJSON](https://github.com/DaveGamble/cJSON)
- [zip](https://github.com/kuba--/zip) (wrap for [miniz](https://github.com/richgel999/miniz))
- ~[freetype](https://www.freetype.org/)~ (changed to `stb_truetype`, but maybe give the option to use it in the future (?))
- ~[cimgui](https://github.com/cimgui/cimgui/) / [imgui](https://github.com/ocornut/imgui/)~ (maybe in the future, as a C++ compiler is needed, and i want to maintain all in C)
- [microui](https://github.com/rxi/microui)

## TODO:

- [x] draw outlined shapes
- [x] draw triangles and circles
- [x] user created spritebatch
- [x] load config from json
- [ ] wrap for lua (work in progress, focusing on this one)
- [ ] wrap for wren (work in progress, stopped by now)
- [x] json parser
- [ ] autopack textures on the fly
- [ ] joystick support
- [ ] pollish the main engine modules (graphics, filesystem, audio, math, input)
- [ ] make specific game modules like camera, tilemap, physics, etc.
- [ ] fuse game executable with zip (like love2d)
- [ ] make simple editors (animations, tilemap, scene..)
- [ ] user custom shaders (like love2d and gms, with default variables for attributes and uniforms)
- [ ] default custom shaders (palette swap, limit palette colors, palette based shadows)
- [ ] support for more audio types
- [x] struct with options to init engine (need to add more options)
- [ ] load audio static (decode in memory)
- [x] drawing textures
- [x] drawing canvas
- [x] custom glsl shaders (but the shaders need to implement some attribute variables and uniforms)
- [x] keyboard and mouse input

## usage

```c

// main.c

#include "src/tinycoffee.h"

int main(int argc, char ** argv) {
  tc_Config config = tic_config_init("title", 640, 380, argc, argv);
  tc_bool success = tic_init(&config);

  tc_texture tex = tic_load_texture(filename);

  while (!tic_window_should_close()) { // main loop
    tic_poll_events(); // poll events
    // tic_scripting_wren_update(); // optional: update wren


    tic_clear(color3(75, 90, 90)); // clear screen with color

    tic_begin_draw(); // begin batch render

    tic_draw_texture(tex, 0, 0, WHITE); // draw texture
    tic_draw_texture_scale(tex, 64, 0, 4, 4, WHITE); // draw scaled texture

    tic_lua_step();

    // tc_scripting_wren_draw(); // optional: draw for wren

    tic_end_draw(); // finish batch render
  }

  tc_terminate();

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
  - **`Windows`**
    - use `mingw`

for now i'm developing on linux, so is easier to compile on it. 

By default tico will use LuaJIT as default, you can compile using Lua 5.4 by just adding
`NOJIT=true` to the make command, eg: `make tico NOJIT=true`

#### Linux

`make tico`

use

`make tico MODE=Release` to compile with -O3

to build for windows in Linux use

`make tico PLATFORM=Windows`

#### Windows

`make tico PLATFORM=Windows`


## distribution

pack all your game assets (images, sound, font, scripts) in a zip called `data.pack`.
If you are using Lua or Wren, maitain the `main.lua` file in the zip root

## screenshots

![screenshot1](https://user-images.githubusercontent.com/15099243/82740604-ba664400-9d20-11ea-9e89-45dc15a7282f.png)

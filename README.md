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
- [zip](https://github.com/kuba--/zip) (wrap for [miniz](https://github.com/richgel999/miniz))
- ~[freetype](https://www.freetype.org/)~ (changed to `stb_truetype`, but maybe give the option to use it in the future (?))
- ~[cimgui](https://github.com/cimgui/cimgui/) / [imgui](https://github.com/ocornut/imgui/)~ (maybe in the future, as a C++ compiler is needed, and i want to maintain all in C)
- [nuklear](https://github.com/Immediate-Mode-UI/Nuklear)
- [microui](https://github.com/rxi/microui)

## TODO:

- [x] draw outlined shapes
- [x] draw triangles and circles
- [ ] wrap for lua
- [ ] wrap for wren (work in progress)
- [ ] json parser
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

### working
- [x] drawing textures
- [x] drawing canvas
- [x] custom glsl shaders (but the shaders need to implement some attribute variables and uniforms)
- [x] keyboard and mouse input

## usage

```c

// main.c

#include "src/tinycoffee.h"

int main(int argc, char ** argv) {
  tc_bool success = tc_init("Tiny Coffee", 640, 480); // init wren

  tc_texture tex = tc_load_texture(filename);

  while (!tc_should_close()) { // main loop
    tc_poll_events(); // poll events
    tc_scripting_wren_update(); // optional: update wren


    tc_clear(color3(75, 90, 90)); // clear screen with color

    tc_begin_draw(); // begin batch render

    tc_draw_texture(tex, 0, 0, WHITE); // draw texture
    tc_draw_texture_scale(tex, 64, 0, 4, 4, WHITE); // draw scaled texture

    tc_scripting_wren_draw(); // optional: draw for wren

    tc_end_draw(); // finish batch render
  }

  tc_terminate();

  return 0;
}

```

main.wren structure:

```dart

// main.wren

import "tico.graphics" for Texture, Color
import "tico" for Config

class Game {
  static config {
    Config.title = "my game"
    Config.width = 1024
    Config.height = 768
  }

  tex {__tex}

  static load() {
    __tex = Texture.load(filename)
  }
  static update(dt) {}
  static draw() {
    tex.draw(0, 0, Color.White)
  }
}

```

## build

for now i'm developing on linux, so is easier to compile on it

just run `./build.sh 'platform'(linux or windows)` (to build for windows you will need to install gcc-mingw32 compiler)

run `./build.sh release 'platform'` for release optimizations (longer compilation time)

## distribution

pack all your game assets (images, sound, font, scripts) in a zip called data.pack, if you are using wren,
maintain the `main.wren` in the root folder of the zip

## screenshots

![screenshot1](https://user-images.githubusercontent.com/15099243/82740604-ba664400-9d20-11ea-9e89-45dc15a7282f.png)

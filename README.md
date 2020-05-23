# Tiny Coffee lib

Tiny Coffee is a framework to develop simple games 2d games using opengl 3.

For now it is far to be complete, it is better for you to use other game engine or framework if you working on a commercial project.

The main idea is to have all the dependencies static compiled in the executable, so there is no need of external dynamic libs other than platform specifics.

I'm using in this project:

- GLFW
- gl3w
- lua
- wren
- miniaudio (include dr_flac, dr_mp3 and dr_wav)
- stblibs (stb_image, stb_vorbis)
- zip (wrap for miniz)
- freetype
- cimgui

## TODO:

[ ] draw outlined shapes
[ ] draw triangles and circles
[ ] wrap for lua
[-] wrap for wren
[ ] json parser
[ ] autopack textures on the fly
[ ] joystick support
[ ] pollish the main engine modules (graphics, filesystem, audio, math, input)
[ ] make game specific modules like camera, tilemap, physics..
[ ] fuse game executable with zip (like love2d)
[ ] make simple editors using cimgui (animations, tilemap, scene..)
[-] custom shaders
[ ] support for more audio types
[ ] struct with options to init engine

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

  return 0;
}

```

## build

for now i'm developing on linux, so is easier to compile on it

just run `build.sh $platform` (to build for windows you will need to install gcc-mingw32 compiler)

## distribution

pack all your game assets (images, sound, font, scripts) in a zip called data.pack, if you are using wren,
maintain the `main.wren` in the root folder of the zip

## screenshots

![screenshot1](https://user-images.githubusercontent.com/15099243/82740604-ba664400-9d20-11ea-9e89-45dc15a7282f.png)

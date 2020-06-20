#ifndef TICO_H
#define TICO_H

#define TICO_VERSION "0.1.3"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "external/GL/gl3w.h"

#define GLFW_INCLUDE_NONE
#include "external/glfw/include/GLFW/glfw3.h"

#define STR(x) #x
#define ASSERT(s) if (!(s)) {TRACELOG("Assertion '%s' failed", STR(s)); exit(-1);}

#define TRACEERR(message...) tc_tracelog(1, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message)
#define ERRLOG(message...) tc_log(1, (const tc_uint8*)message)

#define TRACELOG(message...) tc_tracelog(0, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message)
#define LOG(message...) tc_log(0, (const tc_uint8*)message)


#ifndef TC_MALLOC
  #define TC_MALLOC malloc
#endif
#ifndef TC_CALLOC
  #define TC_CALLOC calloc
#endif
#ifndef TC_REALLOC
  #define TC_REALLOC realloc
#endif
#ifndef TC_FREE
  #define TC_FREE free
#endif

#define PI 3.14159

#define deg2rad(a) ((a) * PI/180)
#define rad2deg(a) ((a) * 180/PI)

#define tc_vertex(x, y, s, t) ((tc_Vertex){(x), (y), 1.f, 1.f, 1.f, 1.f, (s), (t)})
#define tc_vertexc(x, y, col, s, t) ((tc_Vertex){(x), (y), (col.r)/255.f, (col.g)/255.f, (col.b)/255.f, (col.a)/255.f, (s), (t)})
#define tc_rgba(r, g, b, a) ((tc_Color){(r), (g), (b), (a)})
#define tc_rgb(r, g, b) ((tc_Color){(r), (g), (b), 255})
#define tc_color(r, g, b, a) tc_rgba(r, g, b, a)
#define tc_color3(r, g, b) tc_rgb(r, g, b)
#define tc_rectf(x, y, w, h) ((tc_Rectf){(x), (y), (w), (h)})
#define tc_rect(x, y, w, h) ((tc_Rect){(x), (y), (w), (h)})

#define WHITE tc_rgb(255, 255, 255)
#define BLACK tc_rgb(0, 0, 0)
#define BLUE tc_rgb(48, 52, 109)
#define RED tc_rgb(208, 70, 72)
#define GREEN tc_rgb(52, 101, 36)
#define GRAY tc_rgb(78, 74, 78)
#define BROWN tc_rgb(133, 76, 48)
#define BG tc_rgb(75, 90, 90)

#define tc_max(a, b) ((a) < (b) ? (b) : (a))
#define tc_min(a, b) ((a) < (b) ? (a) : (b))
// #define tc_clamp(v, a, b) (tc_max((a), tc_min((v), (b))))

typedef unsigned char tc_byte;
typedef unsigned char tc_uint8;

#ifndef TCDEF
  #define TCDEF
#endif

#define tc_false 0
#define tc_true 1

typedef int tc_bool;

typedef struct {
  union {
    int data[4];
    struct {
      int x;
      int y;
      int w;
      int h;
    };
  };
} tc_Rect;

typedef struct {
  union {
    float data[4];
    struct {
      float x;
      float y;
      float w;
      float h;
    };
  };
} tc_Rectf;

typedef struct {
  union {
    tc_byte data[4];
    struct {
      tc_byte r;
      tc_byte g;
      tc_byte b;
      tc_byte a;
    };
  };
} tc_Color;

#include "external/hashmap.h"

#include "tcmath.h"
#include "window.h"

#include "audio.h"
#include "render.h"
#include "graphics.h"
#include "font.h"
#include "input.h"
#include "filesystem.h"

#include "ui/tcui.h"

#include "debug.h"

typedef enum {
  TC_INIT_ALL = 0,
  TC_INIT_GRAPHICS,
  TC_INIT_INPUT,
  TC_INIT_FILESYSTEM,
  TC_INIT_WREN,
  TC_INIT_LUA
} TC_INIT_FLAGS;

typedef struct {
  tc_uint8 title[256];
  tc_uint8 path[128];
  int width;
  int height;
  TC_INIT_FLAGS flags;
  TC_WINDOW_FLAGS windowFlags;
} tc_Config;

typedef struct {
  double currentTime;
  double lastTime;
  double fpsLastTime;
  float delta;
  int fps;
  int frames;
} tc_Timer;

#ifdef WREN_LANG
  #include "tcwren.h"
#endif
#ifdef LUA_LANG
  #include "tclua.h"
#endif

typedef struct {
  tc_Render render;
  tc_Input input;
  tc_Window window;
  tc_Config config;
  tc_Timer timer;
  tc_Font defaultFont;
#ifdef WREN_LANG
  tc_Wren wren;
#endif
#ifdef LUA_LANG
  tc_Lua lua;
#endif
  tc_bool packed;
} tc_Core;

extern tc_Core Core;

TCDEF void tc_init_config_json(tc_Config *config);
TCDEF tc_Config tc_init_config(const tc_uint8 *title, int width, int height, int argc, char ** argv);
TCDEF tc_bool tc_init(tc_Config *config);
TCDEF void tc_terminate();

TCDEF tc_bool tc_should_close();
TCDEF void tc_update();
TCDEF void tc_poll_events();
TCDEF void tc_close();

TCDEF void tc_clear(tc_Color color);
TCDEF void tc_swap_buffers();

TCDEF void tc_begin_draw();
TCDEF void tc_end_draw();

TCDEF void tc_main_loop();

TCDEF float tc_get_time();
TCDEF float tc_get_delta();
TCDEF int tc_get_fps();

/*****************
 * Utils
 *****************/
TCDEF char *tc_replace_char(char *str, tc_uint8 find, tc_uint8 replace);
TCDEF tc_uint8* tc_utf8_codepoint(tc_uint8 *p, int* codepoint);
TCDEF void tc_utf8_encode(tc_uint8* c, int codepoint);
TCDEF int tc_utf8_decode(const tc_uint8 *p);
// TCDEF void tc_scissor(int x, int y, int w, int h);
TCDEF void tc_write_buffer_header(const char *name, const char *text, size_t size);

#endif

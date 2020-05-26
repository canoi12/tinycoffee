#ifndef TINY_COFFEE_H
#define TINY_COFFEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>

#include "external/GL/gl3w.h"

#define GLFW_INCLUDE_NONE
#include "external/glfw/include/GLFW/glfw3.h"

#define VERSION "0.1.1"

#define STR(x) #x
#define ASSERT(s) if (!(s)) {printf("tinycoffee: %s:%d: %s: Assertion '%s' failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, STR(s)); exit(-1);}

#define ERROR(message...) tc_log(1, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message);

#define LOG(message...) printf("tinycoffee: "); \
printf(message);

// #define TRACELOG(message...) printf("tinycoffee: %s:%d: %s: ", __FILE__, __LINE__, __PRETTY_FUNCTION__);\
printf(message)
#define TRACELOG(message...) tc_log(0, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message);


#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define clamp(v, mi, ma) (min((ma), max((v), (mi))))

#define PI 3.14159

#define deg2rad(a) ((a) * PI/180)

#define vertex(x, y, s, t) ((tc_vertex){(x), (y), 1.f, 1.f, 1.f, 1.f, (s), (t)})
#define vertexc(x, y, col, s, t) ((tc_vertex){(x), (y), (col.r)/255.f, (col.g)/255.f, (col.b)/255.f, (col.a)/255.f, (s), (t)})
#define color(r, g, b, a) ((tc_color){(r), (g), (b), (a)})
#define color3(r, g, b) ((tc_color){(r), (g), (b), 255})

#define WHITE color3(255, 255, 255)
#define BLACK color3(0, 0, 0)
#define BLUE color3(48, 52, 109)
#define RED color3(208, 70, 72)
#define GREEN color3(52, 101, 36)
#define GRAY color3(78, 74, 78)
#define BROWN color3(133, 76, 48)
#define BG color3(75, 90, 90)

#ifndef TCDEF
  #define TCDEF
#endif

typedef uint8_t byte;

typedef int8_t tc_int8;
typedef uint8_t tc_uint8;
typedef int16_t tc_int16;
typedef uint16_t tc_uint16;
typedef int32_t tc_int32;
typedef uint32_t tc_uint32;
typedef int64_t tc_int64;
typedef uint64_t tc_uint64;

typedef enum {
  TC_FALSE = 0,
  TC_TRUE
} tc_bool;

typedef enum {
  TC_UNIFORM_INT,
  TC_UNIFORM_VEC2I,
  TC_UNIFORM_VEC3I,
  TC_UNIFORM_FLOAT,
  TC_UNIFORM_VEC2,
  TC_UNIFORM_VEC3,
  TC_UNIFORM_MATRIX
} TC_SHADER_UNIFORM_;

typedef struct {
  union {
    float data[8];
    struct {
      float pos[2];
      float color[4];
      float uv[2];
    };
    struct {
      float x;
      float y;
      float r;
      float g;
      float b;
      float a;
      float s;
      float t;
    };
  };
} tc_vertex;

typedef struct {
  union {
    float data[4];
    struct {
      float x;
      float y;
      float width;
      float height;
    };
  };
} tc_rectangle;

typedef struct tc_texture {
  unsigned int id;
  int width;
  int height;
} tc_texture;

typedef struct {
  unsigned int id;
  tc_texture tex;
} tc_canvas;

typedef struct {
  double currentTime;
  double lastTime;
  double fpsLastTime;
  float delta;
  int fps;
  int frames;
} tc_timer;

typedef struct tc_color {
    union {
        byte data[4];
        struct
        {
            byte r;
            byte g;
            byte b;
            byte a;
        };
    };
} tc_color;

typedef struct {
  unsigned int id;
} tc_shader;

#ifndef TCDEF
  #define TCDEF extern
#endif

#include "external/stb_image.h"


#include "tcmath.h"
#include "audio.h"
#include "window.h"
#include "render.h"
#include "input.h"
#include "font.h"
#include "fs.h"

typedef struct tc_config {
  char title[256];
  int width;
  int height;
  tc_bool initWren;
  tc_bool initLua;
  tc_bool packed;
  TC_WINDOW_FLAGS_ windowFlags;
  TC_INPUT_FLAGS_ inputFlags;
} tc_config;

#ifdef WREN_LANG
  #include "tcwren.h"
#endif

#ifdef LUA_LANG
  #include "tclua.h"
#endif

typedef struct {
  tc_window window;
  tc_render render;
  tc_input input;
  tc_font defaultFont;
  tc_timer timer;
  tc_bool packed;
#ifdef WREN_LANG
  tc_wren wren;
#endif
#ifdef LUA_LANG
  tc_lua lua;
#endif
} tc_core;

static tc_core CORE;

// tc_core CORE;

/* Core */
TCDEF int tc_config_init(tc_config *config, const char *title, int width, int height);
TCDEF int tc_init(tc_config *config);
TCDEF void tc_terminate();
TCDEF void tc_poll_events();
TCDEF void tc_swap_buffers();

TCDEF void tc_clear(tc_color color);
TCDEF void tc_update_timer();

TCDEF tc_bool tc_should_close();
TCDEF void tc_close();

TCDEF void tc_begin_draw();
TCDEF void tc_end_draw();

TCDEF tc_render tc_get_render();
TCDEF tc_window tc_get_window();

/* Texture */
TCDEF tc_texture tc_create_texture(void *data, tc_int16 width, tc_int16 height);
TCDEF tc_texture tc_load_texture(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_internal(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_external(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_from_memory(const tc_uint8 *buffer, size_t bufferSize);
TCDEF void tc_delete_texture(tc_texture *texture);

TCDEF void tc_draw_texture(tc_texture texture, float x, float y, tc_color color);
TCDEF void tc_draw_texture_scale(tc_texture texture, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_ex(tc_texture texture, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

TCDEF void tc_draw_texture_part(tc_texture texture, tc_rectangle rect, float x, float y, tc_color color);
TCDEF void tc_draw_texture_part_scale(tc_texture texture, tc_rectangle rect, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_part_ex(tc_texture texture, tc_rectangle rect, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

TCDEF void tc_draw_rectangle(float x, float y, float width, float height, tc_color color);
TCDEF void tc_fill_rectangle(float x, float y, float width, float height, tc_color color);
TCDEF void tc_draw_circle(float x, float y, float radius, tc_color color);
TCDEF void tc_fill_circle(float x, float y, float radius, tc_color color);
TCDEF void tc_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, tc_color color);
TCDEF void tc_fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, tc_color color);

TCDEF void tc_draw_text(const tc_uint8 *text, float x, float y, tc_color color);
TCDEF void tc_draw_text_scale(const tc_uint8 *text, float x, float y, float sx, float sy, tc_color color);
TCDEF void tc_draw_text_ex(const tc_uint8 *text, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_color color);
TCDEF void tc_draw_text_font(tc_font font, const tc_uint8 *text, float x, float y, tc_color color);
TCDEF void tc_draw_text_font_scale(tc_font font, const tc_uint8 *text, float x, float y, float sx, float sy, tc_color color);

/* Canvas */
TCDEF tc_canvas tc_create_canvas(tc_int16 width, tc_int16 height);
TCDEF void tc_delete_canvas(tc_canvas *canvas);

TCDEF void tc_set_canvas(tc_canvas canvas);
TCDEF void tc_unset_canvas();

TCDEF void tc_draw_canvas(tc_canvas canvas, float x, float y, tc_color color);
TCDEF void tc_draw_canvas_scale(tc_canvas canvas, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_canvas_ex(tc_canvas canvas, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

/* Shader */
TCDEF tc_shader tc_create_shader(const tc_uint8 *vertexSource, const tc_uint8 *fragmentSource);

TCDEF tc_uint16 tc_compile_shader(const tc_uint8 *source, tc_uint16 type);
TCDEF tc_uint16 tc_load_shader_program(tc_uint16 vertexShader, tc_uint16 fragmentShader);

TCDEF void tc_set_shader(tc_shader shader);

TCDEF tc_shader tc_load_default_shader(tc_uint16 *vertexShader, tc_uint16 *fragmentShader);
TCDEF void tc_shader_send_worldview(tc_shader shader);
TCDEF void tc_shader_send_uniform(tc_shader shader, const tc_uint8 *name, void *value, TC_SHADER_UNIFORM_ type);

/* Input */
TCDEF tc_bool tc_is_key_down(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_up(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_pressed(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_released(TC_KEYBOARD_KEY_ key);

TCDEF tc_bool tc_is_mouse_down(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_up(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_pressed(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_released(TC_MOUSE_BUTTON_ button);

/* Timer */
TCDEF float tc_get_delta();
TCDEF tc_int16 tc_get_fps();
TCDEF float tc_get_time();

/* Filesystem */

TCDEF tc_uint8 *tc_read_file(const tc_uint8 *filename, size_t *outSize);
TCDEF void tc_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_delete_file(const tc_uint8 *filename);
TCDEF tc_bool tc_file_exists(const tc_uint8 *filename);
TCDEF void tc_mkdir(const tc_uint8 *path);
TCDEF void tc_rmdir(const tc_uint8 *path);

/* Scripting */
TCDEF void tc_scripting_wren_update();
TCDEF void tc_scripting_wren_draw();

TCDEF void tc_scripting_lua_update();
TCDEF void tc_scripting_lua_draw();

/* Utils */
TCDEF tc_int8 *tc_replace_char(tc_int8 *str, tc_uint8 find, tc_uint8 replace);
TCDEF tc_uint8* tc_utf8_codepoint(tc_uint8 *p, tc_int32* codepoint);
TCDEF tc_uint16 tc_utf8_decode(const tc_uint8 *p);

/* Log */

TCDEF void tc_log(int type, const tc_uint8 *file, const tc_uint8 *function, tc_uint16 line, const tc_uint8 *fmt, ...);

#endif /* TINY_COFFEE_H */

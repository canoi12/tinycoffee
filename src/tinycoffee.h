#ifndef TINY_COFFEE_H
#define TINY_COFFEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "external/GL/gl3w.h"

#define GLFW_INCLUDE_NONE
#include "external/glfw/include/GLFW/glfw3.h"

#define STR(x) #x
#define ASSERT(s) if (!(s)) {printf("tinycoffee: %s:%d: %s: Assertion '%s' failed\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, STR(s)); exit(-1);}

#define ERROR(module, message...) fprintf(stderr, "tinycoffee: ERROR::%s::", module); \
fprintf(stderr, message)

#define LOG(message...) printf("tinycoffee: "); \
printf(message);

#define TRACELOG(message...) printf("tinycoffee: %s:%d: %s: ", __FILE__, __LINE__, __PRETTY_FUNCTION__);\
printf(message)


#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define clamp(v, mi, ma) (min((ma), max((v), (mi))))

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

#ifndef TCDEF
  #define TCDEF
#endif

typedef unsigned char byte;

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
TCDEF int tc_init(const char *title, int width, int height);
TCDEF void tc_poll_events();
TCDEF void tc_swap_buffers();

TCDEF void tc_clear(tc_color color);
TCDEF void tc_update_timer();

TCDEF int tc_should_close();
TCDEF void tc_close();

TCDEF void tc_begin_draw();
TCDEF void tc_end_draw();

TCDEF tc_render tc_get_render();
TCDEF tc_window tc_get_window();

/* Texture */
TCDEF tc_texture tc_create_texture(void *data, int width, int height);
TCDEF tc_texture tc_load_texture(const char *filename);
TCDEF tc_texture tc_load_texture_internal(const char *filename);
TCDEF tc_texture tc_load_texture_external(const char *filename);
TCDEF tc_texture tc_load_texture_from_memory(const char *buffer, size_t bufferSize);
TCDEF void tc_delete_texture(tc_texture *texture);

TCDEF void tc_draw_texture(tc_texture texture, float x, float y, tc_color color);
TCDEF void tc_draw_texture_scale(tc_texture texture, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_ex(tc_texture texture, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

TCDEF void tc_draw_texture_part(tc_texture texture, tc_rectangle rect, float x, float y, tc_color color);
TCDEF void tc_draw_texture_part_scale(tc_texture texture, tc_rectangle rect, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_part_ex(tc_texture texture, tc_rectangle rect, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

TCDEF void tc_draw_rectangle(float x, float y, float width, float height, tc_color color);
TCDEF void tc_draw_text(const char *text, float x, float y, tc_color color);
TCDEF void tc_draw_text_scale(const char *text, float x, float y, float sx, float sy, tc_color color);
TCDEF void tc_draw_text_ex(const char *text, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_color color);
TCDEF void tc_draw_text_font(tc_font font, const char *text, float x, float y, tc_color color);
TCDEF void tc_draw_text_font_scale(tc_font font, const char *text, float x, float y, float sx, float sy, tc_color color);

/* Canvas */
TCDEF tc_canvas tc_create_canvas(float width, float height);
TCDEF void tc_delete_canvas(tc_canvas *canvas);

TCDEF void tc_set_canvas(tc_canvas canvas);
TCDEF void tc_unset_canvas();

TCDEF void tc_draw_canvas(tc_canvas canvas, float x, float y, tc_color color);
TCDEF void tc_draw_canvas_scale(tc_canvas canvas, float x, float y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_canvas_ex(tc_canvas canvas, float x, float y, float angle, float scaleX, float scaleY, float centerX, float centerY, tc_color color);

/* Shader */
TCDEF tc_shader tc_create_shader(const char *vertexSource, const char *fragmentSource);

TCDEF unsigned int tc_compile_shader(const char *source, int type);
TCDEF unsigned int tc_load_shader_program(unsigned int vertexShader, unsigned int fragmentShader);

TCDEF void tc_set_shader(tc_shader shader);

TCDEF tc_shader tc_load_default_shader(unsigned int *vertexShader, unsigned int *fragmentShader);
TCDEF void tc_shader_send_worldview(tc_shader shader);
TCDEF void tc_shader_send_uniform(tc_shader shader, const char *name, void *value, TC_SHADER_UNIFORM_ type);

/* Input */
TCDEF int tc_is_key_down(TC_KEYBOARD_KEY_ key);
TCDEF int tc_is_key_up(TC_KEYBOARD_KEY_ key);
TCDEF int tc_is_key_pressed(TC_KEYBOARD_KEY_ key);
TCDEF int tc_is_key_released(TC_KEYBOARD_KEY_ key);

TCDEF int tc_is_mouse_down(TC_MOUSE_BUTTON_ button);
TCDEF int tc_is_mouse_up(TC_MOUSE_BUTTON_ button);
TCDEF int tc_is_mouse_pressed(TC_MOUSE_BUTTON_ button);
TCDEF int tc_is_mouse_released(TC_MOUSE_BUTTON_ button);

/* Timer */
TCDEF float tc_get_delta();
TCDEF int tc_get_fps();
TCDEF float tc_get_time();

/* Filesystem */

TCDEF unsigned char *tc_read_file(const char *filename, size_t *outSize);
TCDEF void tc_write_file(const char *filename, const char *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_delete_file(const char *filename);
TCDEF tc_bool tc_file_exists(const char *filename);
TCDEF void tc_mkdir(const char *path);
TCDEF void tc_rmdir(const char *path);

/* Scripting */
TCDEF void tc_scripting_wren_update();
TCDEF void tc_scripting_wren_draw();

TCDEF void tc_scripting_lua_update();
TCDEF void tc_scripting_lua_draw();

/* Utils */
TCDEF char *tc_replace_char(char *str, char find, char replace);

#endif /* TINY_COFFEE_H */

#ifndef TINY_COFFEE_H
#define TINY_COFFEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "external/GL/gl3w.h"

#define GLFW_INCLUDE_NONE
#include "external/glfw/include/GLFW/glfw3.h"

#define TICO_VERSION "0.1.3"

#define STR(x) #x
#define ASSERT(s) if (!(s)) {TRACELOG("Assertion '%s' failed", STR(s)); exit(-1);}

#define ERROR(message...) tc_log(1, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message);

#define LOG(message...) printf("tinycoffee: "); \
printf(message);

#define TRACELOG(message...) tc_log(0, (const tc_uint8*)__FILE__, (const tc_uint8*)__PRETTY_FUNCTION__, __LINE__, (const tc_uint8*)message);


#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define clamp(v, mi, ma) (min((ma), max((v), (mi))))

#define PI 3.14159

#define deg2rad(a) ((a) * PI/180)

#define vertex(x, y, s, t) ((tc_vertex){(x), (y), 1.f, 1.f, 1.f, 1.f, (s), (t)})
#define vertexc(x, y, col, s, t) ((tc_vertex){(x), (y), (col.r)/255.f, (col.g)/255.f, (col.b)/255.f, (col.a)/255.f, (s), (t)})
#define rgba(r, g, b, a) ((tc_color){(r), (g), (b), (a)})
#define rgb(r, g, b) ((tc_color){(r), (g), (b), 255})
#define color(r, g, b, a) rgba(r, g, b, a)
#define color3(r, g, b) rgb(r, g, b)
#define rect(x, y, w, h) ((tc_rect){(x), (y), (w), (h)})

#define WHITE rgb(255, 255, 255)
#define BLACK rgb(0, 0, 0)
#define BLUE rgb(48, 52, 109)
#define RED rgb(208, 70, 72)
#define GREEN rgb(52, 101, 36)
#define GRAY rgb(78, 74, 78)
#define BROWN rgb(133, 76, 48)
#define BG rgb(75, 90, 90)

typedef unsigned char byte;
typedef unsigned char tc_uint8;

#define tc_true 1
#define tc_false 0

typedef int tc_bool;

#include "tctypes.h"

#ifndef TCDEF
  #define TCDEF
#endif

#include "external/stb_image.h"

#include "tcmath.h"
#include "audio.h"
#include "window.h"
#include "render.h"
#include "input.h"
#include "font.h"
#include "fs.h"
#include "ui/tcui.h"
#include "external/cjson/cJSON.h"
#include "editors/editor.h"

typedef struct {
  int x;
  int y;
  int width;
  int height;
  vec2 scale;
  vec2 center;
  float angle;
  matrix view;
} tc_camera;

typedef struct tc_config {
  tc_uint8 title[256];
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
  mu_Context *muCtx;
#ifdef WREN_LANG
  tc_wren wren;
#endif
#ifdef LUA_LANG
  tc_lua lua;
#endif
  struct {
    tc_rect rect[4];
    tc_texture tex;
  } icons;
} tc_core;

extern tc_core CORE;

// tc_core CORE;

/***********************
 * Core
 ***********************/
TCDEF tc_bool tc_config_init(tc_config *config, const tc_uint8 *title, int width, int height);
TCDEF tc_bool tc_init(tc_config *config);
TCDEF void tc_terminate();
TCDEF void tc_poll_events();
TCDEF void tc_swap_buffers();

TCDEF void tc_main_loop();

TCDEF void tc_clear(tc_color color);
TCDEF void tc_update_timer();

TCDEF tc_bool tc_should_close();
TCDEF void tc_close();

TCDEF void tc_begin_draw();
TCDEF void tc_end_draw();

// TCDEF tc_render tc_get_render();
// TCDEF tc_window tc_get_window();

/*************************
 * Texture
 *************************/
TCDEF tc_texture tc_create_texture(void *data, int width, int height);
TCDEF tc_texture tc_load_texture(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_internal(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_external(const tc_uint8 *filename);
TCDEF tc_texture tc_load_texture_from_memory(const tc_uint8 *buffer, size_t bufferSize);
TCDEF void tc_delete_texture(tc_texture *texture);

// Draw textures
TCDEF void tc_draw_texture(tc_texture texture, int x, int y, tc_color color);
TCDEF void tc_draw_texture_scale(tc_texture texture, int x, int y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_ex(tc_texture texture, int x, int y, float angle, float scaleX, float scaleY, int centerX, int centerY, tc_color color);
TCDEF void tc_draw_texture_part(tc_texture texture, tc_rect rect, int x, int y, tc_color color);
TCDEF void tc_draw_texture_part_scale(tc_texture texture, tc_rect rect, int x, int y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_texture_part_ex(tc_texture texture, tc_rect rect, int x, int y, float angle, float scaleX, float scaleY, int centerX, int centerY, tc_color color);

// Draw shapes
TCDEF void tc_draw_rectangle(int x, int y, int width, int height, tc_color color);
TCDEF void tc_fill_rectangle(int x, int y, int width, int height, tc_color color);
TCDEF void tc_draw_circle(int x, int y, float radius, tc_color color);
TCDEF void tc_fill_circle(int x, int y, float radius, tc_color color);
TCDEF void tc_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_color color);
TCDEF void tc_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_color color);

// Draw text
TCDEF void tc_draw_text(const tc_uint8 *text, int x, int y, tc_color color);
TCDEF void tc_draw_text_scale(const tc_uint8 *text, int x, int y, float sx, float sy, tc_color color);
TCDEF void tc_draw_text_ex(const tc_uint8 *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_color color);
TCDEF void tc_draw_text_font(tc_font font, const tc_uint8 *text, int x, int y, tc_color color);
TCDEF void tc_draw_text_font_scale(tc_font font, const tc_uint8 *text, int x, int y, float sx, float sy, tc_color color);

/*********************
 * Canvas
 *********************/
TCDEF tc_canvas tc_create_canvas(int width, int height);
TCDEF void tc_delete_canvas(tc_canvas *canvas);

TCDEF void tc_set_canvas(tc_canvas canvas);
TCDEF void tc_unset_canvas();

TCDEF void tc_draw_canvas(tc_canvas canvas, int x, int y, tc_color color);
TCDEF void tc_draw_canvas_scale(tc_canvas canvas, int x, int y, float scaleX, float scaleY, tc_color color);
TCDEF void tc_draw_canvas_ex(tc_canvas canvas, int x, int y, float angle, float scaleX, float scaleY, int centerX, int centerY, tc_color color);

/**********************
 * Shader
 **********************/
TCDEF tc_shader tc_create_shader(const tc_uint8 *vertexSource, const tc_uint8 *fragmentSource);

TCDEF int tc_compile_shader(const tc_uint8 *source, int type);
TCDEF int tc_load_shader_program(int vertexShader, int fragmentShader);

TCDEF void tc_set_shader(tc_shader shader);

TCDEF tc_shader tc_load_default_shader(int *vertexShader, int *fragmentShader);
TCDEF void tc_shader_send_worldview(tc_shader shader);
TCDEF void tc_shader_send_uniform(tc_shader shader, const tc_uint8 *name, void *value, TC_SHADER_UNIFORM_ type);

/**********************
 * Input
 **********************/
TCDEF tc_bool tc_is_key_down(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_up(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_pressed(TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_is_key_released(TC_KEYBOARD_KEY_ key);

TCDEF tc_bool tc_is_mouse_down(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_up(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_pressed(TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_is_mouse_released(TC_MOUSE_BUTTON_ button);

/**********************
 * Timer
 **********************/
TCDEF float tc_get_delta();
TCDEF int tc_get_fps();
TCDEF float tc_get_time();

/*********************
 * Filesystem
 *********************/

TCDEF tc_uint8 *tc_read_file(const tc_uint8 *filename, size_t *outSize);
TCDEF void tc_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_delete_file(const tc_uint8 *filename);
TCDEF tc_bool tc_file_exists(const tc_uint8 *filename);
TCDEF void tc_mkdir(const tc_uint8 *path);
TCDEF void tc_rmdir(const tc_uint8 *path);

/*********************
 * Scripting
 *********************/
TCDEF void tc_scripting_wren_update();
TCDEF void tc_scripting_wren_draw();

TCDEF void tc_scripting_lua_update();
TCDEF void tc_scripting_lua_draw();

/*********************
 * Utils
 *********************/
TCDEF char *tc_replace_char(char *str, tc_uint8 find, tc_uint8 replace);
TCDEF tc_uint8* tc_utf8_codepoint(tc_uint8 *p, int* codepoint);
TCDEF void tc_utf8_encode(tc_uint8* c, int codepoint);
TCDEF int tc_utf8_decode(const tc_uint8 *p);
TCDEF void tc_scissor(int x, int y, int w, int h);

/*********************
 * Camera
 *********************/
// TCDEF tc_camera tc_create_camera(int x, int y, int w, int h);
// TCDEF void tc_destroy_camera(tc_camera *camera);

// TCDEF void tc_camera_rotate(float angle);
// TCDEF void tc_camera_zoom_in(float zoom);
// TCDEF void tc_camera_zoom_out(float zoom);

// TCDEF void tc_attach_camera(tc_camera *camera);
// TCDEF void tc_detach_camera();
#include "modules/camera.h"

/*********************
 * Log
 *********************/

TCDEF void tc_log(int type, const tc_uint8 *file, const tc_uint8 *function, int line, const tc_uint8 *fmt, ...);

#endif /* TINY_COFFEE_H */

#ifndef TICO_H
#define TICO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* gl3w */
#include "GL/gl3w.h"

/* glfw */
#define GLFW_INCLUDE_NONE
#include "glfw/include/GLFW/glfw3.h"

/* lua */
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"

/* utils */
#include "map.h"
#include "vec.h"
#include "utils.h"

#include "stb/stb_truetype.h"

#include "definitions.h"

#define MAXFONTCHAR 256

typedef enum {
  TIC_UNIFORM_INT,
  TIC_UNIFORM_VEC2I,
  TIC_UNIFORM_VEC3I,
  TIC_UNIFORM_VEC4I,
  TIC_UNIFORM_FLOAT,
  TIC_UNIFORM_VEC2,
  TIC_UNIFORM_VEC3,
  TIC_UNIFORM_VEC4,
  TIC_UNIFORM_MATRIX
} TIC_SHADER_UNIFORM_;

typedef struct tc_Texture {
  unsigned int id;
  int width;
  int height;
  int wrap[2];
  int filter[2];
} tc_Texture;

typedef struct tc_Image {
  tc_Texture texture;
  int width;
  int height;
} tc_Image;

typedef struct tc_Canvas {
  unsigned int id;
  tc_Texture tex;
  int width;
  int height;
} tc_Canvas;

typedef struct tc_Shader {
  unsigned int program;
} tc_Shader;

typedef struct {
  int ax; // advance.x
  int ay; // advance.y

  int bw; // bitmap.width;
  int bh; // bitmap.rows;

  int bl; // bitmap_left;
  int bt; // bitmap_top;

  float tx; // x offset of glyph in texture coordinates
} tc_CharacterInfo;


typedef struct tc_Font {
    tc_Texture texture;
    tc_CharacterInfo c[MAXFONTCHAR];

    unsigned char size;

    stbtt_fontinfo info;
    float ptsize;
    float scale;
    int baseline;
    void *data;

    unsigned int atlasVao;
    unsigned int atlasVbo;
} tc_Font;

#include "tico_math.h"

#include "filesystem.h"
#include "json.h"
#include "serialization.h"
#include "engine.h"
#include "timer.h"

#if !defined(TICO_NO_PLUGINS)
#include "plugin.h"
#endif

#if !defined(TICO_NO_AUDIO_MODULE)
  #include "audio.h"
#endif
#if !defined(TICO_NO_RENDER_MODULE)
  #include "render.h"
#endif
#if !defined(TICO_NO_GRAPHICS_MODULE) && !defined(TICO_NO_RENDER_MODULE)
  #include "graphics.h"
#endif
#if !defined(TICO_NO_WINDOW_MODULE)
  #include "window.h"
#endif
#if !defined(TICO_NO_INPUT_MODULE)
  #include "input.h"
#endif

typedef struct tc_Config {
  char title[256];
  int width;
  int height;
  int flags;
  TIC_WindowFlags_ window_flags;
  int argc;
  char ** argv;
  // list_str_t plugins;
  vec_str_t plugins;
} tc_Config;

typedef struct tc_Core {
  tc_Timer timer;
#if !defined(TICO_NO_AUDIO_MODULE)
  // tc_AudioSystem audio;
  // tc_Audio audio;
#endif
#if !defined(TICO_NO_GRAPHICS_MODULE)
  tc_Graphics graphics;
#endif
#if !defined(TICO_NO_WINDOW_MODULE)
  tc_Window window;
#endif
#if !defined(TICO_NO_INPUT_MODULE)
  tc_Input input;
#endif
  tc_Filesystem fs;
  // tc_Serialization serialization;
#if !defined(TICO_NO_PLUGINS)
  tc_PluginModule plugins;
#endif
} tc_Core;

extern tc_Core Core;

#if defined(__cplusplus)
extern "C" {
#endif

/**********************
 * Core
 **********************/

TIC_API tc_Config tico_config_load(const char *path, int argc, char ** argv);
TIC_API tc_Config tico_config_init(const char *title, int width, int height, int argc, char ** argv);

TIC_API int tico_init(tc_Config *config);
TIC_API void tico_terminate();

TIC_API void tico_poll_events();

TIC_API int tico_update();
TIC_API int tico_begin_draw();
TIC_API int tico_end_draw();

TIC_API int tico_main_loop();

/***************
 * Plugins
 ***************/

TIC_API void tico_plugins_default();
TIC_API void tico_plugins_enable(const char *name);
TIC_API void tico_plugins_disable(const char *name);
TIC_API tc_bool tico_plugins_is_active(const char *name);
TIC_API tc_Plugin* tico_plugins_get(const char *name);

TIC_API void tico_plugins_init();
TIC_API void tico_plugins_update();
TIC_API void tico_plugins_begin_draw();
TIC_API void tico_plugins_draw();
TIC_API void tico_plugins_end_draw();
TIC_API void tico_plugins_terminate();


/**********************
 * Debug
 **********************/

TIC_API void tic_tracelog(int mode, const char *func, const char *file, int line, const char *fmt, ...);
TIC_API void tic_log(int mode, const char *fmt, ...);

// INTERNAL_API(void tico_input_ok, tc_Input *input, int key);

/*===========================*
 #                           #
 #        PUBLIC API         #
 #                           #
 *===========================*/

/*==============*
 *    Timer     *
 *==============*/

TIC_API void PUBLIC_API(tico_timer_update);
TIC_API float PUBLIC_API(tico_timer_get_time);
TIC_API float PUBLIC_API(tico_timer_get_delta);
TIC_API int PUBLIC_API(tico_timer_get_fps);
TIC_API void PUBLIC_API(tico_timer_wait, float ms);

/*==============*
 *  Filesystem  *
 *==============*/

TIC_API PUBLIC_API(void tico_filesystem_set_packed, tc_bool packed);
TIC_API PUBLIC_API(tc_bool tico_filesystem_is_packed);

TIC_API PUBLIC_API(void tico_filesystem_set_path, const char *path);
TIC_API PUBLIC_API(unsigned char* tico_filesystem_get_path);
TIC_API PUBLIC_API(void tico_filesystem_resolve_path, char *outName, const char *filename);
TIC_API PUBLIC_API(unsigned char* tico_filesystem_get_exe_path);

TIC_API PUBLIC_API(unsigned char *tico_filesystem_read_internal_file, const char *filename, size_t *outSize);
TIC_API PUBLIC_API(unsigned char *tico_filesystem_read_external_file, const char *filename, size_t *outSize);

TIC_API PUBLIC_API(void tico_filesystem_write_internal_file, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode);
TIC_API PUBLIC_API(void tico_filesystem_write_external_file, const char *filename, const char* text, size_t size, TIC_WRITE_MODE_ mode);

TIC_API PUBLIC_API(tc_bool tico_filesystem_internal_file_exists, const char *filename);
TIC_API PUBLIC_API(tc_bool tico_filesystem_external_file_exists, const char *filename);

TIC_API PUBLIC_API(unsigned char *tico_filesystem_read_file, const char *filename, size_t *outSize);
TIC_API PUBLIC_API(void tico_filesystem_write_file, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode);
TIC_API PUBLIC_API(void tico_filesystem_delete_file, const char *filename);
TIC_API PUBLIC_API(tc_bool tico_filesystem_file_exists, const char *filename);
TIC_API PUBLIC_API(void tico_filesystem_mkdir, const char *path);
TIC_API PUBLIC_API(void tico_filesystem_rmdir, const char *path);

/*==============*
 *   Graphics   *
 *==============*/

// TIC_API void tico_graphics_draw(tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color);
// TIC_API void tico_graphics_draw_ex(tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color);
// TIC_API void tico_graphics_clear(tc_Color color);

// TIC_API void tico_graphics_push();
// TIC_API void tico_graphics_pop();

// TIC_API void tico_graphics_origin();
// TIC_API void tico_graphics_translate(float x, float y);
// TIC_API void tico_graphics_rotate(float angle);
// TIC_API void tico_graphics_scale(float x, float y);

// TIC_API void tico_graphics_scissor(int x, int y, int w, int h);

// TIC_API void tico_graphics_send(const char *name, void *data, TIC_SHADER_UNIFORM_ type);
// TIC_API void tico_graphics_send_count(const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type);
// TIC_API int tico_graphics_default_vertex_shader();
// TIC_API int tico_graphics_default_fragment_shader();
// TIC_API tc_Shader tico_graphics_default_shader();

void PUBLIC_API(tico_graphics_draw, tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color);
void PUBLIC_API(tico_graphics_draw_ex, tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color);
void PUBLIC_API(tico_graphics_clear, tc_Color color);

void PUBLIC_API(tico_graphics_push);
void PUBLIC_API(tico_graphics_pop);

void PUBLIC_API(tico_graphics_origin);
void PUBLIC_API(tico_graphics_translate, float x, float y);
void PUBLIC_API(tico_graphics_rotate, float angle);
void PUBLIC_API(tico_graphics_scale, float x, float y);

void PUBLIC_API(tico_graphics_scissor, int x, int y, int w, int h);

void PUBLIC_API(tico_graphics_send, const char *name, void *data, TIC_SHADER_UNIFORM_ type);
void PUBLIC_API(tico_graphics_send_count, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type);


int PUBLIC_API(tico_graphics_default_vertex_shader);
int PUBLIC_API(tico_graphics_default_fragment_shader);
tc_Shader PUBLIC_API(tico_graphics_default_shader);

void PUBLIC_API(tico_graphics_push_shader, tc_Shader shader);
void PUBLIC_API(tico_graphics_pop_shader);

void PUBLIC_API(tico_graphics_push_canvas, tc_Canvas canvas);
void PUBLIC_API(tico_graphics_pop_canvas);
tc_Canvas PUBLIC_API(tico_graphics_top_canvas, void);

void PUBLIC_API(tico_graphics_push_matrix, tc_Matrix matrix);
void PUBLIC_API(tico_graphics_pop_matrix);

/*******************
 * Shapes
 *******************/

// TIC_API void tico_graphics_draw_rectangle(float x, float y, int width, int height, tc_Color color);
// TIC_API void tico_graphics_fill_rectangle(float x, float y, float width, float height, tc_Color color);
// TIC_API void tico_graphics_draw_rect(tc_Rect rect, tc_Color color);
// TIC_API void tico_graphics_fill_rect(tc_Rect rect, tc_Color color);

// TIC_API void tico_graphics_draw_circle(int x, int y, float radius, tc_Color color);
// TIC_API void tico_graphics_fill_circle(int x, int y, float radius, tc_Color color);
// TIC_API void tico_graphics_draw_circle_ex(int x, int y, float radius, int sides, tc_Color color);
// TIC_API void tico_graphics_fill_circle_ex(int x, int y, float radius, int sides, tc_Color color);

// TIC_API void tico_graphics_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
// TIC_API void tico_graphics_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);

// TIC_API void tico_graphics_draw_line(int x0, int y0, int x1, int y1, tc_Color color);

PUBLIC_API(void tico_graphics_draw_rectangle, float x, float y, int width, int height, tc_Color color);
PUBLIC_API(void tico_graphics_fill_rectangle, float x, float y, float width, float height, tc_Color color);
PUBLIC_API(void tico_graphics_draw_rect, tc_Rect rect, tc_Color color);
PUBLIC_API(void tico_graphics_fill_rect, tc_Rect rect, tc_Color color);

PUBLIC_API(void tico_graphics_draw_circle, int x, int y, float radius, tc_Color color);
PUBLIC_API(void tico_graphics_fill_circle, int x, int y, float radius, tc_Color color);
PUBLIC_API(void tico_graphics_draw_circle_ex, int x, int y, float radius, int sides, tc_Color color);
PUBLIC_API(void tico_graphics_fill_circle_ex, int x, int y, float radius, int sides, tc_Color color);

PUBLIC_API(void tico_graphics_draw_triangle, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
PUBLIC_API(void tico_graphics_fill_triangle, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);

PUBLIC_API(void tico_graphics_draw_line, int x0, int y0, int x1, int y1, tc_Color color);


/********************
 * Text
 ********************/

// TIC_API void tico_graphics_draw_text(const char *text, int x, int y, tc_Color color);
// TIC_API void tico_graphics_draw_text_scale(const char *text, int x, int y, float sx, float sy, tc_Color color);
// TIC_API void tico_graphics_draw_text_ex(const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

// TIC_API void tico_graphics_draw_text_font(tc_Font font, const char *text, int x, int y, tc_Color color);
// TIC_API void tico_graphics_draw_text_font_scale(tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color);
// TIC_API void tico_graphics_draw_text_font_ex(tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

// TIC_API void tico_graphics_draw_text_font_scale_width(tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color);

PUBLIC_API(void tico_graphics_draw_text, const char *text, int x, int y, tc_Color color);
PUBLIC_API(void tico_graphics_draw_text_scale, const char *text, int x, int y, float sx, float sy, tc_Color color);
PUBLIC_API(void tico_graphics_draw_text_ex, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

PUBLIC_API(void tico_graphics_draw_text_font, tc_Font font, const char *text, int x, int y, tc_Color color);
PUBLIC_API(void tico_graphics_draw_text_font_scale, tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color);
PUBLIC_API(void tico_graphics_draw_text_font_ex, tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

PUBLIC_API(void tico_graphics_draw_text_font_scale_width, tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color);


/*================*
 *     Input      *
 *================*/

void PUBLIC_API(tico_input_update_mouse_scroll, float x, float y);
void PUBLIC_API(tico_input_update_key, TIC_KEY_ key, int action);
void PUBLIC_API(tico_input_update_mouse_pos, int x, int y);
void PUBLIC_API(tico_input_update_joy_button, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button, int action);
void PUBLIC_API(tico_input_update_mouse_button, TIC_MOUSEBUTTON_ button, int action);

PUBLIC_API(int tico_input_get_key_code, const char *name);
PUBLIC_API(int tico_input_get_joy_btncode, const char *name);
PUBLIC_API(int tico_input_get_joy_axiscode, const char *name);
PUBLIC_API(int tico_input_get_mouse_code, const char *name);

PUBLIC_API(tc_bool tico_input_key_down, TIC_KEY_ key);
PUBLIC_API(tc_bool tico_input_key_pressed, TIC_KEY_ key);
PUBLIC_API(tc_bool tico_input_key_up, TIC_KEY_ key);
PUBLIC_API(tc_bool tico_input_key_released, TIC_KEY_ key);

PUBLIC_API(tc_bool tico_input_mouse_down, TIC_MOUSEBUTTON_ button);
PUBLIC_API(tc_bool tico_input_mouse_pressed, TIC_MOUSEBUTTON_ button);
PUBLIC_API(tc_bool tico_input_mouse_up, TIC_MOUSEBUTTON_ button);
PUBLIC_API(tc_bool tico_input_mouse_released, TIC_MOUSEBUTTON_ button);
PUBLIC_API(void tico_input_mouse_set_cursor, int cursor);

PUBLIC_API(tc_bool tico_input_joy_down, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
PUBLIC_API(tc_bool tico_input_joy_up, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
PUBLIC_API(tc_bool tico_input_joy_pressed, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
PUBLIC_API(tc_bool tico_input_joy_released, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);

PUBLIC_API(void tico_input_get_mouse_pos, int *x, int *y);
PUBLIC_API(void tico_input_fix_mouse, );
PUBLIC_API(void tico_input_release_mouse, );
PUBLIC_API(void tico_input_get_mouse_delta, int *x, int *y);
PUBLIC_API(void tico_input_get_mouse_scroll, float *x, float *y);
PUBLIC_API(void tico_input_get_mouse_scroll_delta, float *x, float *y);

/*================*
 *     Window     *
 *================*/

TIC_API int tico_window_should_close();
TIC_API void tico_window_swap_buffers();

TIC_API void tico_window_get_pos(int *x, int *y);
TIC_API void tico_window_set_pos(int x, int y);

TIC_API int tico_window_get_width();
TIC_API void tico_window_set_width(int width);

TIC_API int tico_window_get_height();
TIC_API void tico_window_set_height( int height);

TIC_API void tico_window_get_size(int *width, int *height);
TIC_API void tico_window_set_size(int width, int height);

TIC_API const char* tico_window_get_title();
TIC_API void tico_window_set_title(const char *title);

TIC_API tc_bool tico_window_is_fullscreen();
TIC_API void tico_window_set_fullscreen(tc_bool fullscreen);
TIC_API void tico_window_toggle_fullscreen();

TIC_API tc_bool tico_window_is_resizable();
TIC_API void tico_window_set_resizable(tc_bool resizable);
TIC_API void tico_window_toggle_resizable();

TIC_API tc_bool tico_window_get_vsync();
TIC_API void tico_window_set_vsync(tc_bool vsync);

/*====================*
 *       Plugins      *
 *====================*/

TIC_API void PUBLIC_API(tico_plugin_add_plugin, const char *name, tc_Plugin plugin);
TIC_API void PUBLIC_API(tico_plugin_enable_plugin, const char *name);
TIC_API void PUBLIC_API(tico_plugin_disable_plugin, const char *name);
TIC_API tc_bool PUBLIC_API(tico_plugin_is_active, const char *name);
TIC_API tc_Plugin* PUBLIC_API(tico_plugin_get, const char *name);

TIC_API int PUBLIC_API(tico_plugin_module_init);
TIC_API void PUBLIC_API(tico_plugin_module_terminate);
TIC_API void PUBLIC_API(tico_plugin_module_update);
TIC_API void PUBLIC_API(tico_plugin_module_begin_draw);
TIC_API void PUBLIC_API(tico_plugin_module_draw);
TIC_API void PUBLIC_API(tico_plugin_module_end_draw);

#if defined(__cplusplus)
}
#endif


#endif // TICO_H
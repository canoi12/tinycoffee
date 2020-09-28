#ifndef TICO_WINDOW_H
#define TICO_WINDOW_H

#include "definitions.h"

typedef enum {
  TIC_WindowFlags_Default = 0,
  TIC_WindowFlags_FullScreen = (1 << 0),
  TIC_WindowFlags_Resizable = (1 << 1),
  TIC_WindowFlags_VSync = (1 << 2)
} TIC_WindowFlags_;

enum {
  TIC_CURSOR_ARROW = 0,
  TIC_CURSOR_TEXT,
  TIC_CURSOR_CROSSHAIR,
  TIC_CURSOR_HAND,
  TIC_CURSOR_HRESIZE,
  TIC_CURSOR_VRESIZE
};

typedef struct tc_Window {
  char title[256];
  int x;
  int y;
  int width;
  int height;

  tc_Rect bkpMode;
  GLFWcursor *mouse_cursors[6];

  struct {
    tc_bool fullscreen;
    tc_bool resizable;
    tc_bool vsync;
  } state;

  void *handle;
} tc_Window;

TIC_API tc_bool tico_window_init(tc_Window *window, const char *title, int width, int height, TIC_WindowFlags_ flags);
TIC_API tc_bool tico_window_deinit(tc_Window *window);


/*******************
 * Internal API
 *******************/

TIC_API tc_bool tico_window_should_close_internal(tc_Window *window);
TIC_API void tico_window_swap_buffers_internal(tc_Window *window);

TIC_API void tico_window_get_pos_internal(tc_Window *window, int *x, int *y);
TIC_API void tico_window_set_pos_internal(tc_Window *window, int x, int y);

TIC_API int tico_window_get_width_internal(tc_Window *window);
TIC_API void tico_window_set_width_internal(tc_Window *window, int width);

TIC_API int tico_window_get_height_internal(tc_Window *window);
TIC_API void tico_window_set_height_internal(tc_Window *window,  int height);

TIC_API void tico_window_get_size_internal(tc_Window *window, int *width, int *height);
TIC_API void tico_window_set_size_internal(tc_Window *window, int width, int height);

TIC_API const char* tico_window_get_title_internal(tc_Window *window);
TIC_API void tico_window_set_title_internal(tc_Window *window, const char *title);

TIC_API tc_bool tico_window_is_fullscreen_internal(tc_Window *window);
TIC_API void tico_window_set_fullscreen_internal(tc_Window *window, tc_bool fullscreen);
TIC_API void tico_window_toggle_fullscreen_internal(tc_Window *window);

TIC_API tc_bool tico_window_is_resizable_internal(tc_Window *window);
TIC_API void tico_window_set_resizable_internal(tc_Window *window, tc_bool resizable);
TIC_API void tico_window_toggle_resizable_internal(tc_Window *window);

TIC_API tc_bool tico_window_get_vsync_internal(tc_Window *window);
TIC_API void tico_window_set_vsync_internal(tc_Window *window, tc_bool vsync);

TIC_API void tico_window_set_mouse_cursor_internal(tc_Window *window, int cursor);

#endif // TICO_WINDOW_H
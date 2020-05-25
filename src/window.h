#ifndef TC_WINDOW_H
#define TC_WINDOW_H

#include "tinycoffee.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef enum {
  TC_WINDOW_DEFAULT = 0,
  TC_WINDOW_FULLSCREEN = 1 << 0,
  TC_WINDOW_RESIZABLE = 1 << 1
} TC_WINDOW_FLAGS_;

typedef struct {
  GLFWwindow *handle;
  tc_int16 width;
  tc_int16 height;
  tc_bool fullscreen;
} tc_window;

TCDEF tc_window tc_create_window(const tc_uint8 *title, tc_int16 width, tc_int16 height, TC_WINDOW_FLAGS_ flags);
TCDEF void tc_destroy_window(tc_window *window);

TCDEF tc_bool tc_window_should_close(tc_window window);
TCDEF void tc_close_window(tc_window window);

TCDEF tc_int16 tc_window_get_width(tc_window window);
TCDEF void tc_window_set_width(tc_window *window, tc_int16 width);

TCDEF tc_int16 tc_window_get_height(tc_window window);
TCDEF void tc_window_set_height(tc_window *window, tc_int16 height);

TCDEF void tc_window_get_size(tc_window window, tc_int16 *width, tc_int16 *height);
TCDEF void tc_window_set_size(tc_window *window, tc_int16 width, tc_int16 height);

#endif

#if defined(TC_WINDOW_IMPLEMENTATION)

TCDEF tc_window tc_create_window(const tc_uint8 *title, tc_int16 width, tc_int16 height, TC_WINDOW_FLAGS_ flags) {
  tc_window window = {0};
  tc_uint8 *ttitle = (tc_uint8*)title;
  if (width < 64) width = 64;
  if (height < 64) height = 64;

  window.handle = glfwCreateWindow(width, height, ttitle, NULL, NULL);
  if (!window.handle) {
    ERROR("WINDOW", "Failed to create GLFW window\n");
    return window;
  }

  window.width = width;
  window.height = height;

  glfwMakeContextCurrent(window.handle);

  return window;
}
TCDEF void tc_destroy_window(tc_window *window) {
  glfwDestroyWindow(window->handle);
}

TCDEF tc_bool tc_window_should_close(tc_window window) {
  return glfwWindowShouldClose(window.handle);
}

TCDEF void tc_close_window(tc_window window) {
  glfwSetWindowShouldClose(window.handle, TC_TRUE);
}

TCDEF tc_int16 tc_window_get_width(tc_window window) { return window.width; }
TCDEF void tc_window_set_width(tc_window *window, tc_int16 width) {
  ASSERT(window != NULL);
  glfwSetWindowSize(window->handle, width, window->height);
  window->width = width;
}

TCDEF tc_int16 tc_window_get_height(tc_window window) { return window.height; }
TCDEF void tc_window_set_height(tc_window *window, tc_int16 height) {
  ASSERT(window != NULL);
  glfwSetWindowSize(window->handle, window->width, height);
  window->height = height;
}

TCDEF void tc_window_get_size(tc_window window, tc_int16 *width, tc_int16 *height) {
  if (width) *width = window.width;
  if (height) *height = window.height;
}
TCDEF void tc_window_set_size(tc_window *window, tc_int16 width, tc_int16 height) {
  ASSERT(window != NULL);
  glfwSetWindowSize(window->handle, width, height);
  window->width = width;
  window->height = height;
}


#endif

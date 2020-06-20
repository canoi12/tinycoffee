#ifndef TICO_WINDOW_H
#define TICO_WINDOW_H

#include "tinycoffee.h"

typedef enum {
  TC_WINDOW_DEFAULT = 0,
  TC_WINDOW_FULLSCREEN = 1 << 1,
  TC_WINDOW_RESIZABLE = 1 << 2,
  TC_WINDOW_VSYNC = 1 << 3,
} TC_WINDOW_FLAGS;

typedef struct {
  GLFWwindow *handle;
  tc_uint8 title[256];
  int x;
  int y;
  unsigned int width;
  unsigned int height;
  tc_bool fullscreen;
  tc_Rect bkpMode;
  tc_bool resizable;
} tc_Window;

TCDEF tc_Window tc_create_window(const tc_uint8 *title, int width, int height, TC_WINDOW_FLAGS flags);
TCDEF void tc_destroy_window(tc_Window *window);

TCDEF void tc_get_window_pos(tc_Window *window, int *x, int *y);
TCDEF void tc_set_window_pos(tc_Window *window, int x, int y);
TCDEF void tc_get_window_size(tc_Window *window, int *width, int *height);
TCDEF void tc_set_window_size(tc_Window *window, int width, int height);

TCDEF tc_uint8* tc_get_window_title(tc_Window *window);
TCDEF void tc_set_window_title(tc_Window *window, const tc_uint8 *title);

TCDEF tc_bool tc_get_window_fullscreen(tc_Window *window);
TCDEF void tc_set_window_fullscreen(tc_Window *window, tc_bool fullscreen);

TCDEF tc_bool tc_get_window_resizable(tc_Window *window);
TCDEF void tc_set_window_resizable(tc_Window *window, tc_bool resizable);

#endif /* TICO_WINDOW_H */

#if defined(TICO_WINDOW_IMPLEMENTATION)

TCDEF tc_Window tc_create_window(const tc_uint8 *title, int width, int height, TC_WINDOW_FLAGS flags) {
  tc_Window window = {0};
  if (!title) sprintf(window.title, "tico %s", TICO_VERSION);
  else strcpy(window.title, title);
  window.width = tc_max(width, 10);
  window.height = tc_max(height, 10);
  window.fullscreen = tc_false;
  window.resizable = tc_true;

  window.handle = glfwCreateWindow(window.width, window.height, window.title, NULL, NULL);
  if (!window.handle) {
    TRACEERR("Failed to create window");
    exit(1);
  }

  tc_get_window_pos(&window, &window.x, &window.y);
  tc_get_window_size(&window, &window.width, &window.height);
//   glfwSwapInterval(1);

  if (flags & TC_WINDOW_FULLSCREEN) {
    tc_set_window_fullscreen(&window, tc_true);
  }
  if (~flags & TC_WINDOW_RESIZABLE) {
    tc_set_window_resizable(&window, tc_false);
  }


  glfwMakeContextCurrent(window.handle);

  glfwSwapInterval(1);
  if (~flags & TC_WINDOW_VSYNC) {
    glfwSwapInterval(0);
  }

  return window;
}

TCDEF void tc_destroy_window(tc_Window *window) {
  glfwDestroyWindow(window->handle);
}

TCDEF void tc_get_window_pos(tc_Window *window, int *x, int *y) {
  glfwGetWindowPos(window->handle, x, y);
}
TCDEF void tc_set_window_pos(tc_Window *window, int x, int y) {
  glfwSetWindowPos(window->handle, x, y);
}
TCDEF void tc_get_window_size(tc_Window *window, int *width, int *height) {
  glfwGetWindowSize(window->handle, width, height);
}
TCDEF void tc_set_window_size(tc_Window *window, int width, int height) {
  glfwSetWindowSize(window->handle, width, height);
}

TCDEF tc_uint8* tc_get_window_title(tc_Window *window) {
  return window->title;
}
TCDEF void tc_set_window_title(tc_Window *window, const tc_uint8 *title) {
  strcpy(window->title, title);
  glfwSetWindowTitle(window->handle, title);
}

TCDEF tc_bool tc_get_window_fullscreen(tc_Window *window) {
  return window->fullscreen;
}
TCDEF void tc_set_window_fullscreen(tc_Window *window, tc_bool fullscreen) {
  if (window->fullscreen == fullscreen) return;

  if (fullscreen) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    tc_get_window_pos(window, &window->bkpMode.x, &window->bkpMode.y);
    tc_get_window_size(window, &window->bkpMode.w, &window->bkpMode.h);
    glfwSetWindowMonitor(window->handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
    window->fullscreen = tc_true;
  } else {
//     TRACELOG(
    glfwSetWindowMonitor(window->handle, NULL, window->bkpMode.x, window->bkpMode.y, window->bkpMode.w, window->bkpMode.h, 0);
    window->fullscreen = tc_false;
  }
}

TCDEF tc_bool tc_get_window_resizable(tc_Window *window) {
  return window->resizable;
}
TCDEF void tc_set_window_resizable(tc_Window *window, tc_bool resizable) {
  if (window->resizable == resizable) return;

  glfwSetWindowAttrib(window->handle, GLFW_RESIZABLE, !window->resizable);
  window->resizable = !window->resizable;
}

#endif /* TICO_WINDOW_IMPLEMENTATION */

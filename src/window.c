#include "tico.h"

tc_bool tico_window_init(tc_Window *window, const char *title, int width, int height, TIC_WindowFlags_ flags) {
  ASSERT(window != NULL);

  memset(window, 0, sizeof(*window));

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  for (int i = 0; i < 6; i++) {
    window->mouse_cursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);  
  }

  window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window->handle) return tc_false;
  window->width = width;
  window->height = height;
  window->state.fullscreen = tc_false;
  window->state.resizable = tc_true;

  tico_window_get_pos_internal(window, &window->x, &window->y);
  // tico_window_get_size_internal(window, &window->x, &window->y);

  glfwMakeContextCurrent((GLFWwindow*)window->handle);

  window->bkpMode.w = width;
  window->bkpMode.h = height;

  if (flags & TIC_WindowFlags_FullScreen) {
    tico_window_set_fullscreen_internal(window, tc_true);
  }
  if (~flags & TIC_WindowFlags_Resizable) {
    tico_window_set_resizable_internal(window, tc_false);
  }

  glfwSwapInterval(1);
  if (~flags & TIC_WindowFlags_VSync) {
    glfwSwapInterval(0);
  }

  LOG("Window initiated");

  return tc_true;
}

tc_bool tico_window_deinit(tc_Window *window) {
  glfwDestroyWindow(window->handle);

  return 1;
}

tc_bool tico_window_should_close_internal(tc_Window* window) {
  return glfwWindowShouldClose(window->handle);
}

void tico_window_swap_buffers_internal(tc_Window* window) {
  glfwSwapBuffers(window->handle);
}

void tico_window_get_pos_internal(tc_Window *window, int *x, int *y) {
  ASSERT(window != NULL);
  glfwGetWindowPos(window->handle, x, y);
}

void tico_window_set_pos_internal(tc_Window *window, int x, int y) {
  ASSERT(window != NULL);
  glfwSetWindowPos(window->handle, x, y);
}

int tico_window_get_width_internal(tc_Window *window) {
  ASSERT(window != NULL);
  return window->width;
}

void tico_window_set_width_internal(tc_Window *window, int width) {
  ASSERT(window != NULL);
  // ASSERT(width >= 32);
  if (width <= 32) return;
  int old_h = tico_window_get_height_internal(window);
  glfwSetWindowSize(window->handle, width, old_h);
  window->width = width;
}

int tico_window_get_height_internal(tc_Window *window)  {
  ASSERT(window != NULL);
  return window->height;
}
void tico_window_set_height_internal(tc_Window *window, int height) {
  ASSERT(window != NULL);
  // ASSERT(height >= 32);
  if (height <= 32) return;
  int old_w = tico_window_get_width_internal(window);
  glfwSetWindowSize(window->handle, old_w, height);
  window->height = height;
}

void tico_window_get_size_internal(tc_Window *window, int *width, int *height) {
  ASSERT(window != NULL);
  if (width) *width = window->width;
  if (height) *height = window->height;
}

void tico_window_set_size_internal(tc_Window *window, int width, int height) {
  ASSERT(window != NULL);
  // ASSERT(width >= 32);
  // ASSERT(height >= 32);
  // if (width <= 32) width = 32;
  // if (height <= 32) return;
  width = tico_max(width, 32);
  height = tico_max(height, 32);
  glfwSetWindowSize(window->handle, width, height);
  window->width = width;
  window->height = height;
}

const char* tico_window_get_title_internal(tc_Window *window) {
  ASSERT(window != NULL);
  return window->title;
}

void tico_window_set_title_internal(tc_Window *window, const char *title) {
  ASSERT(window != NULL);
  if (!title) return;
  glfwSetWindowTitle(window->handle, title);
  strcpy(window->title, title);
}

tc_bool tico_window_is_fullscreen_internal(tc_Window *window) {
  ASSERT(window != NULL);

  return window->state.fullscreen;
}

void tico_window_set_fullscreen_internal(tc_Window *window, tc_bool fullscreen) {
  ASSERT(window != NULL);
  if (window->state.fullscreen == fullscreen) return;

  if (fullscreen) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    tico_window_get_pos_internal(window, &window->bkpMode.x, &window->bkpMode.y);
    tico_window_get_size_internal(window, &window->bkpMode.w, &window->bkpMode.h);
    glfwSetWindowMonitor(window->handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
    window->state.fullscreen = tc_true;
  } else {
//     TRACELOG(
    glfwSetWindowMonitor(window->handle, NULL, window->bkpMode.x, window->bkpMode.y, window->bkpMode.w, window->bkpMode.h, 0);
    window->state.fullscreen = tc_false;
  }
}

void tico_window_toggle_fullscreen_internal(tc_Window *window) {
  tico_window_set_fullscreen_internal(window, !window->state.fullscreen);
}

tc_bool tico_window_is_resizable_internal(tc_Window *window) {
  ASSERT(window != NULL);
  return window->state.resizable;
}

void tico_window_set_resizable_internal(tc_Window *window, tc_bool resizable) {
  ASSERT(window != NULL);
  if (window->state.resizable == resizable) return;
  glfwSetWindowAttrib(window->handle, GLFW_RESIZABLE, resizable);
  window->state.resizable = resizable;
}

void tico_window_toggle_resizable_internal(tc_Window *window) {
  ASSERT(window != NULL);
  tc_bool resizable = window->state.resizable;
  tico_window_set_resizable_internal(window, !resizable);
}

tc_bool tico_window_get_vsync_internal(tc_Window *window) {

  return 1;
}

void tico_window_set_vsync_internal(tc_Window *window, tc_bool vsync) {

}

void tic_window_set_mouse_cursor_internal(tc_Window *window, int cursor) {
  if (cursor < 0 || cursor >= 6) return;
  glfwSetCursor(window->handle, window->mouse_cursors[cursor]);
}
#include "tico.h"

tc_bool tic_window_init(tc_Window *window, const char *title, int width, int height, TIC_WINDOW_FLAGS flags) {
  if (!title) sprintf(window->title, "tico %s", TICO_VERSION);
  else strcpy(window->title, title);
  window->width = tic_max(width, 10);
  window->height = tic_max(height, 10);
  window->state.fullscreen = tc_false;
  window->state.resizable = tc_true;

  window->handle = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);
  if (!window->handle) {
    TRACEERR("Failed to create window");
    exit(1);
  }

  tic_window_get_pos(&window->x, &window->y);
  tic_window_get_size(&window->width, &window->height);
//   glfwSwapInterval(1);

  if (flags & TIC_WINDOW_FULLSCREEN) {
    tic_window_set_fullscreen(tc_true);
  }
  if (~flags & TIC_WINDOW_RESIZABLE) {
    tic_window_set_resizable(tc_false);
  }


  glfwMakeContextCurrent(window->handle);

  glfwSwapInterval(1);
  if (~flags & TIC_WINDOW_VSYNC) {
    glfwSwapInterval(0);
  }

  return tc_true;
}

void tic_window_destroy(tc_Window *window) {
  glfwDestroyWindow(window->handle);
  TRACELOG("Window destroyed");
}

void tic_window_get_pos(int *x, int *y) {
  glfwGetWindowPos(Core.window.handle, x, y);
}
void tic_window_set_pos(int x, int y) {
  glfwSetWindowPos(Core.window.handle, x, y);
  Core.window.x = x;
  Core.window.y = y;
}

void tic_window_get_size(int *width, int *height) {
  glfwGetWindowSize(Core.window.handle, width, height);
}

void tic_window_set_size(int width, int height) {
  glfwSetWindowSize(Core.window.handle, width, height);
  Core.window.width = width;
  Core.window.height = height;
}

int tic_window_get_width() {
  return Core.window.width;
}

void tic_window_set_width(int width) {
  glfwSetWindowSize(Core.window.handle, width, Core.window.height);
  Core.window.width = width;
}

int tic_window_get_height() {
  return Core.window.height;
}

void tic_window_set_height(int height) {
  glfwSetWindowSize(Core.window.handle, Core.window.width, height);
  Core.window.height = height;
}

tc_uint8* tic_window_get_title() {
  return Core.window.title;
}


void tic_window_set_title(const tc_uint8 *title) {
  strcpy(Core.window.title, title);
  glfwSetWindowTitle(Core.window.handle, title);
}

tc_bool tic_window_is_fullscreen() {
  return Core.window.state.fullscreen;
}
void tic_window_set_fullscreen(tc_bool fullscreen) {
  if (Core.window.state.fullscreen == fullscreen) return;
  tc_Window *window = &Core.window;

  if (fullscreen) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    tic_window_get_pos(&window->bkpMode.x, &window->bkpMode.y);
    tic_window_get_size(&window->bkpMode.w, &window->bkpMode.h);
    glfwSetWindowMonitor(window->handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
    window->state.fullscreen = tc_true;
  } else {
//     TRACELOG(
    glfwSetWindowMonitor(window->handle, NULL, window->bkpMode.x, window->bkpMode.y, window->bkpMode.w, window->bkpMode.h, 0);
    window->state.fullscreen = tc_false;
  }
}

void tic_window_toggle_fullscreen() {
  tic_window_set_fullscreen(!Core.window.state.fullscreen);
}

tc_bool tic_window_is_resizable() {
  return Core.window.state.resizable;
}
void tic_window_set_resizable(tc_bool resizable) {
  if (Core.window.state.resizable == resizable) return;

  glfwSetWindowAttrib(Core.window.handle, GLFW_RESIZABLE, !Core.window.state.resizable);
  Core.window.state.resizable = !Core.window.state.resizable;
}


int tic_window_should_close() {
  return glfwWindowShouldClose(Core.window.handle);
}

void tic_window_close() {
  glfwSetWindowShouldClose(Core.window.handle, 1);
}

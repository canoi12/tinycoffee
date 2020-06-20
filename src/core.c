#include "tinycoffee.h"

#define TICO_AUDIO_IMPLEMENTATION
#include "audio.h"

#define TICO_WINDOW_IMPLEMENTATION
#include "window.h"

#define TICO_RENDER_IMPLEMENTATION
#include "render.h"

#define TICO_INPUT_IMPLEMENTATION
#include "input.h"

#define TICO_MATH_IMPLEMENTATION
#include "tcmath.h"

#define HASHMAP_IMPLEMENTATION
#include "external/hashmap.h"

tc_Core Core;

static void tc_window_move_callback(GLFWwindow *window, int x, int y) {
  Core.window.x = x;
  Core.window.y = y;
}

static void tc_window_resize_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  Core.window.width = width;
  Core.window.height = height;
}

static void tc_window_character_callback(GLFWwindow *window, unsigned int codepoint) {
  tc_ui_text_callback(codepoint);
}

static void tc_window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  Core.input.keyState.down[key] = tc_clamp(action, 0, 1);
  tc_ui_key_callback(key, action);
}

static void tc_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  int fbutton = tc_clamp(button, 0, MOUSE_BUTTON_LAST);
  Core.input.mouseState.down[fbutton] = action;
  tc_ui_mouse_btn_callback(button, Core.input.mouseState.x, Core.input.mouseState.y, action);
}

static void tc_mouse_pos_callback(GLFWwindow *window, double posX, double posY)
{
  Core.input.mouseState.x = posX;
  Core.input.mouseState.y = posY;

  tc_ui_mouse_pos_callback(posX, posY);
}

static void tc_window_focus_callback(GLFWwindow *window, int focused)
{
  if (focused == 1)
    tc_start_device();
  else
    tc_stop_device();
}

static void tc_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  tc_ui_mouse_scroll_callback(xoffset, yoffset);
  Core.input.mouseState.scrollX = xoffset * -10;
  Core.input.mouseState.scrollY = yoffset * -10;
}

TCDEF tc_bool tc_init(tc_Config *config) {
  if (!glfwInit()) {
    TRACEERR("Failed to init GLFW");
    exit(-1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  Core.config = *config;
  Core.packed = tc_false;
  if (tc_file_exists("data.pack")) Core.packed = tc_true;

  TRACELOG("packed: %d", Core.packed);

  Core.window = tc_create_window(config->title, config->width, config->height, config->windowFlags);
  // Core.packed = tc_false;

  if (gl3wInit()) {
    TRACEERR("Failed to init gl3w");
    exit(-1);
  }

#ifdef LUA_LANG
  tc_init_lua(&Core.config);
#endif
#ifdef WREN_LANG
  Core.wren = tc_init_wren(&Core.config);
#endif

  // Window callbacks
  glfwSetWindowPosCallback(Core.window.handle, tc_window_move_callback);
  glfwSetWindowSizeCallback(Core.window.handle, tc_window_resize_callback);
  glfwSetKeyCallback(Core.window.handle, tc_window_key_callback);
  glfwSetMouseButtonCallback(Core.window.handle, tc_mouse_button_callback);
  glfwSetCursorPosCallback(Core.window.handle, tc_mouse_pos_callback);
  glfwSetWindowFocusCallback(Core.window.handle, tc_window_focus_callback);
  glfwSetCharCallback(Core.window.handle, tc_window_character_callback);
  glfwSetScrollCallback(Core.window.handle, tc_mouse_scroll_callback);
  TRACELOG("Setup callbacks");

  tc_init_input(&Core.input, 0);
  TRACELOG("Input initiated");

  Core.render = tc_create_render();
  TRACELOG("Render created");

  Core.defaultFont = tc_load_default_font();
  TRACELOG("Default font loaded");

  tc_init_audio();

  glViewport(0, 0, Core.window.width, Core.window.height);
#ifdef WREN_LANG
  tc_wren_game_load(Core.wren);
#endif
#ifdef LUA_LANG
  tc_lua_load();
#endif

  tc_ui_init(Core.defaultFont);

  return tc_true;
}

TCDEF void tc_init_config_json(tc_Config *config) {
  if (!tc_file_exists("config.json")) return;
  cJSON *jsonConfig = tc_json_open("config.json");
  cJSON *window = NULL;
  const char *name = tc_json_get_opt_string(jsonConfig, "name", config->title);
  if (name) {
    strcpy(config->title, name);
  }
  window = tc_json_get_object(jsonConfig, "window");
  if (window) {
    config->width = tc_json_get_opt_number(window, "width", config->width);
    config->height = tc_json_get_opt_number(window, "height", config->height);
    if (!tc_json_get_opt_boolean(window, "resizable", tc_true)) config->windowFlags ^= TC_WINDOW_RESIZABLE;
    if (tc_json_get_opt_boolean(window, "fullscreen", tc_false)) config->windowFlags |= TC_WINDOW_FULLSCREEN;
    if (!tc_json_get_opt_boolean(window, "vsync", tc_true)) config->windowFlags ^= TC_WINDOW_VSYNC;
  }

  tc_json_delete(jsonConfig);
}

TCDEF tc_Config tc_init_config(const tc_uint8 *title, int width, int height, int argc, char ** argv) {
  tc_Config config = {0};
  config.title[0] = '\0';
  if (title) sprintf(config.title, "%s", title);
  else if (!config.title[0]) strcpy((char*)config.title, "tico " TICO_VERSION);
  config.width = tc_max(width, 10);
  config.height = tc_max(height, 10);
  config.windowFlags = 0;
  config.windowFlags |= TC_WINDOW_RESIZABLE;
  config.windowFlags |= TC_WINDOW_VSYNC;
  config.flags = 0;

  if (argc < 2) strcpy(config.path, ".");
  else strcpy(config.path, argv[1]);
  tc_filesystem_set_path(config.path);

  tc_init_config_json(&config);

  return config;
}

TCDEF tc_bool tc_should_close() {
  return glfwWindowShouldClose(Core.window.handle);
}

TCDEF void tc_poll_events() {
  tc_update_input(&Core.input);
  glfwPollEvents();
}

TCDEF void tc_update_timer() {
  Core.timer.currentTime = glfwGetTime();
  Core.timer.delta = Core.timer.currentTime - Core.timer.lastTime;
  Core.timer.lastTime = Core.timer.currentTime;

  float delta = Core.timer.currentTime - Core.timer.fpsLastTime;
  Core.timer.frames++;
  if (delta >= 1) {
    Core.timer.fps = Core.timer.frames;
    Core.timer.frames = 0;
    Core.timer.fpsLastTime = Core.timer.currentTime;
  }
}

TCDEF void tc_update() {
  tc_update_timer();
  tc_poll_events();
}

TCDEF void tc_close() {
  glfwSetWindowShouldClose(Core.window.handle, 1);
}

TCDEF void tc_clear(tc_Color color) {
  glClearColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

TCDEF void tc_swap_buffers() {
  glfwSwapBuffers(Core.window.handle);
}

TCDEF void tc_begin_draw() {
  tc_begin_batch(&Core.render.batch);
  glEnable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(Core.render.state.currentShader.id);
  tc_shader_send_worldview(Core.render.state.currentShader);
  tc_begin_batch(&Core.render.batch);
  tc_ui_begin();
}

TCDEF void tc_end_draw() {
  tc_ui_end();
  tc_flush_batch(&Core.render.batch);
  tc_draw_batch(&Core.render.batch);

  tc_swap_buffers();
}

TCDEF void tc_main_loop() {
  int count = 0;
  while(!tc_should_close()) {
    tc_update();
// #ifdef WREN_LANG
//     tc_wren_game_update(Core.wren);
// #endif

    tc_clear(BLACK);
    tc_begin_draw();
#ifdef LUA_LANG
    tc_lua_step();
#endif
// #ifdef WREN_LANG
//     tc_wren_game_draw(Core.wren);
// #endif

    tc_end_draw();
#ifdef LUA_LANG
//     count++;
//     if (count > 60 * Core.timer.fps)  {tc_lua_gc(LUA_GCCOLLECT, 0); count = 0;}
#endif
  }
}

TCDEF float tc_get_time() {
  return Core.timer.currentTime;
}

TCDEF float tc_get_delta() {
  return Core.timer.delta;
}

TCDEF int tc_get_fps() {
  return Core.timer.fps;
}

TCDEF void tc_terminate() {
  tc_destroy_render(&Core.render);
  tc_destroy_window(&Core.window);
  glfwTerminate();
}

/******************
 * Utils
 ******************/

TCDEF char *tc_replace_char(char *str, tc_uint8 find, tc_uint8 replace) {
  char *currentPos = (char*)strchr((const char*)str, find);
  while(currentPos) {
    *currentPos = replace;
    currentPos = (char*)strchr((const char*)currentPos, find);
  }

  return str;
}

#define MAXUNICODE 0x10FFFF

TCDEF tc_uint8* tc_utf8_codepoint(tc_uint8 *p, int* codepoint) {
  tc_uint8 *n;
  tc_uint8 c = p[0];
  if (c < 0x80) {
    *codepoint = c;
    n = p + 1;
    return n;
  }

  switch (c & 0xf0) {
    case 0xf0: {
      *codepoint = ((p[0] & 0x07) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | ((p[3] & 0x3f));
      n = p + 4;
      break;
    }
    case 0xe0: {
      *codepoint = ((p[0] & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | ((p[2] & 0x3f));
      n = p + 3;
      break;
    }
    case 0xc0:
    case 0xd0: {
      *codepoint = ((p[0] & 0x1f) << 6) | ((p[1] & 0x3f));
      n = p + 2;
      break;
    }
    default:
    {
      *codepoint = -1;
      n = n + 1;
    }
  }

  if (*codepoint > MAXUNICODE) *codepoint = -1;

  return n;
}
TCDEF void tc_utf8_encode(tc_uint8* c, int codepoint) {
  if (codepoint < 0x80) {
    c[0] = (tc_uint8)codepoint;
  } else if (codepoint < 0x800) {
    c[0] = 0xC0 | (codepoint >> 6);
    c[1] = 0x80 | (codepoint & 0x3f);
  } else if (codepoint <= 0xffff) {
    c[0] = 0xe0 | (codepoint >> 12);
    c[1] = 0x80 | ((codepoint >> 6) & 0x3f);
    c[2] = 0x80 | (codepoint & 0x3f);
  } else if (codepoint <= 0x10ffff) {
    c[0] = 0xf0 | (codepoint >> 18);
    c[1] = 0x80 | ((codepoint >> 12) & 0x3f);
    c[2] = 0x80 | ((codepoint >> 6) & 0x3f);
    c[3] = 0x80 | (codepoint & 0x3f);
  }
}

TCDEF int tc_utf8_decode(const tc_uint8 *p) {
  static const int limits[] = {0xFF, 0x7F, 0x7FF, 0xFFFF};
  const tc_uint8 *s = (const tc_uint8 *)p;
  int c = s[0];
  int res = 0;  /* final result */
  if (c < 0x80)  /* ascii? */
    res = c;
  else {
    int count = 0;  /* to count number of continuation bytes */
    while (c & 0x40) {  /* still have continuation bytes? */
      int cc = s[++count];  /* read next byte */
      if ((cc & 0xC0) != 0x80)  /* not a continuation byte? */
        return -1;  /* invalid byte sequence */
      res = (res << 6) | (cc & 0x3F);  /* add lower 6 bits from cont. byte */
      c <<= 1;  /* to test next bit */
    }
    res |= ((c & 0x7F) << (count * 5));  /* add first byte */
    if (count > 3 || res > MAXUNICODE || res <= limits[count])
      return -1;  /* invalid byte sequence */
    s += count;  /* skip continuation bytes read */
  }
  return res;
}

// TCDEF void tc_scissor(int x, int y, int w, int h) {
// //   tc_reset_batch(&Core.render);
// //   GLint view[4];
// //   glGetIntegerv(GL_VIEWPORT, view);
// //   glScissor(x, view[3] - (y+h), w, h);
// }

TCDEF void tc_write_buffer_header(const char *name, const char *text, size_t size) {
  size_t ssize = size * 20 + 512;
  char out[ssize];
  memset(out, 0, ssize);
  sprintf(out, "static const char %s[] = {\n", name);
  for (int i = 0; i < size; i++) {
    if (text[i] == EOF) break;
    char c[10];
    sprintf(c, "%d,", text[i]);
    sprintf(out, "%s%s", out, c);
    if (i != 0 && i % 20 == 0) sprintf(out, "%s\n", out);
  }
  sprintf(out, "%s0\n};", out);
  tc_write_file("out.txt", out, 0, 0);
}

/******************
 * Debug
 ******************/

#define TICO_DEBUG_IMPLEMENTATION
#include "debug.h"

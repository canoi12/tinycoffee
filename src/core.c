#include "tico.h"

#define TICO_UTILS_IMPLEMENTATION
#include "utils.h"

#define TICO_IMGUI_IMPLEMENTATION
#include "tico_imgui.h"

tc_Core Core;

static void tico_window_move_callback(GLFWwindow *window, int x, int y) {
  tc_Core *core = glfwGetWindowUserPointer(window);
  // core->window.x = x;
  // core->window.y = y;
  tico_window_set_pos(x, y);
}

static void tico_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  // tc_Core *core = glfwGetWindowUserPointer(window);
  // core->window.width = width;
  // core->window.height = height;
  // TRACELOG("%d %d", width, height);
  tico_window_set_size(width, height);
}

static void tico_window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  tc_Core *core = glfwGetWindowUserPointer(window);
  // core->input.keyState.down[key] = tico_clamp(action, 0, 1);
  // tico_ui_key_callback(key, action);
  tico_input_update_key(key, action);

  // tico_lua_callback("keypressed");
}

static void tico_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  tc_Core *core = glfwGetWindowUserPointer(window);
  tico_input_update_mouse_button(button, action);
  // int fbutton = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  // core->input.mouseState.down[fbutton] = action;
  // tico_ui_mouse_btn_callback(button, Core.input.mouseState.x, Core.input.mouseState.y, action);
}

static void tico_mouse_pos_callback(GLFWwindow *window, double posX, double posY) {
  tc_Core *core = glfwGetWindowUserPointer(window);
  tico_input_update_mouse_pos(posX, posY);
  // core->input.mouseState.x = posX;
  // core->input.mouseState.y = posY;
  // tic_lua_callback("mousemoved");

  // tic_ui_mouse_pos_callback(posX, posY);
}

static void tico_window_focus_callback(GLFWwindow *window, int focused)
{
  // if (focused == 1)
  //   tico_audio_start_device();
  // else
  //   tico_audio_stop_device();
}

static void tico_joystick_callback(int jid, int event) {
  if (event == GLFW_CONNECTED) {
    Core.input.joystickState[jid].active = tc_true;
    const char *name = glfwGetGamepadName(jid);
    TRACELOG("joystick '%s' connected", name);
  } else if (event == GLFW_DISCONNECTED) {
    Core.input.joystickState[jid].active = tc_true;
    const char *name = glfwGetGamepadName(jid);
    TRACELOG("joystick '%s' disconnected", name);
  }
}

static void tico_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  // tico_ui_mouse_scroll_callback(xoffset, yoffset);
  // Core.input.mouseState.scroll_delta.x = (xoffset * -10) - Core.input.mouseState.scroll.x;
  // Core.input.mouseState.scroll_delta.y = (yoffset * -10) - Core.input.mouseState.scroll.y;
  // tc_Core *core = glfwGetWindowUserPointer(window);
  // core->input.mouseState.scroll.x = xoffset * -10;
  // core->input.mouseState.scroll.y = yoffset * -10;
  // core->input.mouseState.scroll_delta.x += xoffset * -10;
  // core->input.mouseState.scroll_delta.y += yoffset * -10;
  tico_input_update_mouse_scroll(xoffset, yoffset);

  // TRACELOG("%f %f", Core.input.mouseState.scroll_delta.x, Core.input.mouseState.scroll_delta.y);


  // tico_lua_callback("mousescroll");
}

/*=======================
 * Core
 *=======================*/

tc_Config tico_config_load(const char *filename, int argc, char ** argv) {
  tc_Config config = tico_config_init(NULL, 640, 380, argc, argv);
  if (!tico_filesystem_file_exists(filename)) return config;
  cJSON *jsonConfig = tico_json_open(filename);
  cJSON *window = NULL;

  // TRACELOG("%s", filename);

  const char *name = tico_json_get_opt_string(jsonConfig, "name", (char*)config.title);
  if (name) strcpy(config.title, name);

  window = tico_json_get_object(jsonConfig, "window");
  if (window) {
    config.width = tico_json_get_opt_number(window, "width", config.width);
    config.height = tico_json_get_opt_number(window, "height", config.height);
    if (!tico_json_get_opt_boolean(window, "resizable", tc_true)) config.window_flags ^= TIC_WindowFlags_Resizable;
    if (tico_json_get_opt_boolean(window, "fullscreen", tc_false)) config.window_flags |= TIC_WindowFlags_FullScreen;
    if (!tico_json_get_opt_boolean(window, "vsync", tc_true)) config.window_flags ^= TIC_WindowFlags_VSync;
    // TRACELOG("%d", config.window_flags & TIC_WindowFlags_Resizable);
  }

  cJSON *plugins = NULL;
  plugins = tico_json_get_array(jsonConfig, "plugins");
  if (plugins) {
    cJSON *plugin = NULL;
    cJSON_ArrayForEach(plugin, plugins) {
      char *pname = malloc(strlen(plugin->valuestring));
      strcpy(pname, plugin->valuestring);
      vec_push(&config.plugins, pname);
    }
  }

  // int i = 0;
  // char *pname = vec_first(&config.plugins);
  // TRACELOG("%s", pname);

  tico_json_delete(jsonConfig);

  return config;
}

tc_Config tico_config_init(const char *title, int width, int height, int argc, char ** argv) {
  tc_Config config = {0};
  tico_filesystem_init(&Core.fs);
  strcpy(config.title, "tico "TICO_VERSION);
  if (title) strcpy(config.title, title);
  config.width = tico_max(width, 64);
  config.height = tico_max(height, 64);
  config.window_flags = TIC_WindowFlags_Resizable | TIC_WindowFlags_VSync;
  config.flags = 0;
  // list_init(&config.plugins);
  vec_init(&config.plugins);

  config.argc = argc;
  config.argv = argv;

  config.flags = 0;

  char path[128];
  if (argc < 2) strcpy(path, ".");
  else strcpy(path, argv[1]);
  tico_filesystem_set_path(path);

  return config;
}

int tico_init(tc_Config *config) {
  tc_Config conf = {0};
  if (config) memcpy(&conf, config, sizeof(*config));
  else conf = tico_config_init(NULL, 640, 380, 0, NULL);

  if (!glfwInit()) {
    TRACEERR("Failed to init GLFW");
    exit(1);
  }

  // memset(&Core, 0, sizeof(tc_Core));
  if (!tico_window_init(&Core.window, conf.title, conf.width, conf.height, conf.window_flags)) {
    TRACEERR("Failed to init tico Window");
    exit(1);
  }
  glfwSetWindowUserPointer(Core.window.handle, &Core);
  // glfwMakeContextCurrent(Core.window.handle);

  GLFWwindow *window = Core.window.handle;
  glfwSetWindowPosCallback(window, tico_window_move_callback);
  glfwSetFramebufferSizeCallback(window, tico_framebuffer_size_callback);
  glfwSetKeyCallback(window, tico_window_key_callback);
  glfwSetMouseButtonCallback(window, tico_mouse_button_callback);
  glfwSetCursorPosCallback(window, tico_mouse_pos_callback);
  glfwSetWindowFocusCallback(window, tico_window_focus_callback);
  glfwSetJoystickCallback(tico_joystick_callback);
  glfwSetScrollCallback(window, tico_mouse_scroll_callback);

  #if !defined(TICO_NO_RENDER_MODULE)
  if (gl3wInit()) {
    TRACEERR("Failed to init gl3w");
    exit(1);
  }
  #endif

  // tico_timer_init(&Core.timer);

  #if !defined(TICO_NO_GRAPHICS_MODULE)
  tico_graphics_init(&Core.graphics);
  #endif

  #if !defined(TICO_NO_INPUT_MODULE)
  tico_input_init(&Core.input, 0);
  #endif
  tico_audio_init();

  tico_imgui_init(&Core.window);

  #if !defined(TICO_NO_PLUGINS)
  tico_plugin_module_init_internal(&Core.plugins);
  char *key;
  int i;
  vec_foreach(&config->plugins, key, i) {
    tico_plugin_enable_plugin(key);
  }
  vec_deinit(&config->plugins);
  #endif

  return 1;
}

void tico_terminate() {
  tico_audio_terminate();
  tico_input_terminate(&Core.input);
  tico_graphics_terminate(&Core.graphics);
  tico_window_deinit(&Core.window);
  glfwTerminate();
}

void tico_poll_events() {
  glfwPollEvents();
}

int tico_update() {
  tico_timer_update_internal(&Core.timer);
  tico_input_update(&Core.input);
  tico_poll_events();
  tico_imgui_update();

  tico_plugin_module_update_internal(&Core.plugins);

  return 1;
}

int tico_begin_draw() {
  tico_graphics_begin_internal(&Core.graphics);

  tico_plugin_module_begin_draw_internal(&Core.plugins);
  return 1;
}
int tico_end_draw() {
  tico_plugin_module_end_draw_internal(&Core.plugins);
  tico_graphics_end_internal(&Core.graphics);
  tico_imgui_render();
  tico_window_swap_buffers();
  return 1;
}

int tico_main_loop() {
  int clicked = 0;
  tico_plugin_module_load_internal(&Core.plugins);
  while(!tico_window_should_close()) {
    tico_update();

    tico_begin_draw();

    tico_plugin_module_draw_internal(&Core.plugins);

    tico_end_draw();
  }

  return 1;
}

/*=======================
 * Debug
 *=======================*/

#include <time.h>
#include <stdarg.h>

void tic_log(int type, const char *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  char err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR: ");
  char buffer[1024];
  char bufmsg[512];

  tm_now = localtime(&t);
  char buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s", buf, err);
  va_start(args, fmt);
  vfprintf(stderr, (const char*)fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void tic_tracelog(int type, const char *file, const char *function, int line, const char *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  char err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR in ");
  char buffer[1024];
  char bufmsg[512];

  tm_now = localtime(&t);
  char buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  // sprintf(buffer, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  va_start(args, fmt);
  // vsprintf(bufmsg, (const char*)fmt, args);
  vfprintf(stderr, (const char*)fmt, args);
  // fprintf(stderr, "%s", bufmsg);
  va_end(args);
  fprintf(stderr, "\n");
  // tico_input_ok_internal
  // strcat(buffer, bufmsg);
  // tc_editor_write_log(buffer);
}


/*===========================*
 #                           #
 #        PUBLIC API         #
 #                           #
 *===========================*/

/*==============*
 *    Timer     *
 *==============*/

void PUBLIC_API(tico_timer_update) {
  INTERNAL_API(tico_timer_update, &Core.timer);
}
float PUBLIC_API(tico_timer_get_time) {
  return INTERNAL_API(tico_timer_get_time, &Core.timer);
}
float PUBLIC_API(tico_timer_get_delta) {
  return INTERNAL_API(tico_timer_get_delta, &Core.timer);
}
int PUBLIC_API(tico_timer_get_fps) {
  return INTERNAL_API(tico_timer_get_fps, &Core.timer);
}
void PUBLIC_API(tico_timer_wait, float ms) {
  INTERNAL_API(tico_timer_wait, &Core.timer, ms);
}

/*==============*
 *  Filesystem  *
 *==============*/

void PUBLIC_API(tico_filesystem_set_packed, tc_bool packed) {
  INTERNAL_API(tico_filesystem_set_packed, &Core.fs, packed);
}
tc_bool PUBLIC_API(tico_filesystem_is_packed) {
  return INTERNAL_API(tico_filesystem_is_packed, &Core.fs);
}

void PUBLIC_API(tico_filesystem_set_path, const char *path) {
  INTERNAL_API(tico_filesystem_set_path, &Core.fs, path);
}
unsigned char* PUBLIC_API(tico_filesystem_get_path) {
  return INTERNAL_API(tico_filesystem_get_path, &Core.fs);
}
void PUBLIC_API(tico_filesystem_resolve_path, char *outName, const char *filename) {
  INTERNAL_API(tico_filesystem_resolve_path, &Core.fs, outName, filename);
}
unsigned char* PUBLIC_API(tico_filesystem_get_exe_path) {
  return INTERNAL_API(tico_filesystem_get_exe_path, &Core.fs);
}

unsigned char* PUBLIC_API(tico_filesystem_read_internal_file, const char *filename, size_t *outSize) {
  return INTERNAL_API(tico_filesystem_read_internal_file, &Core.fs, filename, outSize);
}
unsigned char* PUBLIC_API(tico_filesystem_read_external_file, const char *filename, size_t *outSize) {
  return INTERNAL_API(tico_filesystem_read_external_file, &Core.fs, filename, outSize);
}

void PUBLIC_API(tico_filesystem_write_internal_file, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode) {
  INTERNAL_API(tico_filesystem_write_internal_file, &Core.fs, filename, text, size, mode);
}
void PUBLIC_API(tico_filesystem_write_external_file, const char *filename, const char* text, size_t size, TIC_WRITE_MODE_ mode) {
  INTERNAL_API(tico_filesystem_write_external_file, &Core.fs, filename, text, size, mode);
}

tc_bool PUBLIC_API(tico_filesystem_internal_file_exists, const char *filename) {
  return INTERNAL_API(tico_filesystem_internal_file_exists, &Core.fs, filename);
}
tc_bool PUBLIC_API(tico_filesystem_external_file_exists, const char *filename) {
  return INTERNAL_API(tico_filesystem_external_file_exists, &Core.fs, filename);
}

unsigned char* PUBLIC_API(tico_filesystem_read_file, const char *filename, size_t *outSize) {
  return INTERNAL_API(tico_filesystem_read_file, &Core.fs, filename, outSize);
}
void PUBLIC_API(tico_filesystem_write_file, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode) {
  INTERNAL_API(tico_filesystem_write_file, &Core.fs, filename, text, size, mode);
}
void PUBLIC_API(tico_filesystem_delete_file, const char *filename) {
  INTERNAL_API(tico_filesystem_delete_file, &Core.fs, filename);
}
tc_bool PUBLIC_API(tico_filesystem_file_exists, const char *filename) {
  return INTERNAL_API(tico_filesystem_file_exists, &Core.fs, filename);
}
void PUBLIC_API(tico_filesystem_mkdir, const char *path) {
  INTERNAL_API(tico_filesystem_mkdir, &Core.fs, path);
}
void PUBLIC_API(tico_filesystem_rmdir, const char *path) {
  INTERNAL_API(tico_filesystem_rmdir, &Core.fs, path);
}

/*==============*
 *   Graphics   *
 *==============*/

void PUBLIC_API(tico_graphics_draw, tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color) {
  // tico_graphics_draw_internal(&Core.graphics, texture, dest, source, color);
  INTERNAL_API(tico_graphics_draw, &Core.graphics, texture, dest, source, color);
}

void PUBLIC_API(tico_graphics_draw_ex, tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color) {
  // tico_graphics_draw_ex_internal(&Core.graphics, texture, dest, source, angle, origin, color);
  INTERNAL_API(tico_graphics_draw_ex, &Core.graphics, texture, dest, source, angle, origin, color);
}

void PUBLIC_API(tico_graphics_push, void) {
  // tico_graphics_push_internal(&Core.graphics);
  INTERNAL_API(tico_graphics_push, &Core.graphics);
}
void PUBLIC_API(tico_graphics_pop, void) {
  // tico_graphics_pop_internal(&Core.graphics);
  INTERNAL_API(tico_graphics_pop, &Core.graphics);
}

void PUBLIC_API(tico_graphics_origin, void) {
  // tico_graphics_origin_internal(&Core.graphics);
  INTERNAL_API(tico_graphics_origin, &Core.graphics);
}
void PUBLIC_API(tico_graphics_translate, float x, float y) {
  // tico_graphics_translate_internal(&Core.graphics, x, y);
  INTERNAL_API(tico_graphics_translate, &Core.graphics, x, y);
}
void PUBLIC_API(tico_graphics_rotate, float angle) {
  INTERNAL_API(tico_graphics_rotate, &Core.graphics, angle);
}
void PUBLIC_API(tico_graphics_scale, float x, float y) {
  INTERNAL_API(tico_graphics_scale, &Core.graphics, x, y);
}

void PUBLIC_API(tico_graphics_scissor, int x, int y, int w, int h) {
  INTERNAL_API(tico_graphics_scissor, &Core.graphics, x, y, w, h);
}

void PUBLIC_API(tico_graphics_send, const char *name, void *data, TIC_SHADER_UNIFORM_ type) {
  INTERNAL_API(tico_graphics_send, &Core.graphics, name, data, type);
}
void PUBLIC_API(tico_graphics_send_count, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type) {
  INTERNAL_API(tico_graphics_send_count, &Core.graphics, name, count, data, type);
}

int PUBLIC_API(tico_graphics_default_vertex_shader, void) {
  return INTERNAL_API(tico_graphics_default_vertex_shader, &Core.graphics);
}
int PUBLIC_API(tico_graphics_default_fragment_shader, void) {
  return INTERNAL_API(tico_graphics_default_fragment_shader, &Core.graphics);
}
tc_Shader PUBLIC_API(tico_graphics_default_shader, void) {
  return INTERNAL_API(tico_graphics_default_shader, &Core.graphics);
}

void PUBLIC_API(tico_graphics_push_shader, tc_Shader shader) {
  INTERNAL_API(tico_graphics_push_shader, &Core.graphics, shader);
}
void PUBLIC_API(tico_graphics_pop_shader, void) {
  INTERNAL_API(tico_graphics_pop_shader, &Core.graphics);
}
void PUBLIC_API(tico_graphics_push_canvas, tc_Canvas canvas) {
  INTERNAL_API(tico_graphics_push_canvas, &Core.graphics, canvas);
}
void PUBLIC_API(tico_graphics_pop_canvas, void) {
  INTERNAL_API(tico_graphics_pop_canvas, &Core.graphics);
}
tc_Canvas PUBLIC_API(tico_graphics_top_canvas, void) {
  return INTERNAL_API(tico_graphics_top_canvas, &Core.graphics);
}

void PUBLIC_API(tico_graphics_push_matrix, tc_Matrix matrix) {}
void PUBLIC_API(tico_graphics_pop_matrix, void) {}

/*******************
 * Shapes
 *******************/

void PUBLIC_API(tico_graphics_draw_rectangle, float x, float y, int width, int height, tc_Color color) {
  INTERNAL_API(tico_graphics_draw_rectangle, &Core.graphics, x, y, width, height, color);
}
void PUBLIC_API(tico_graphics_fill_rectangle, float x, float y, float width, float height, tc_Color color) {
  INTERNAL_API(tico_graphics_fill_rectangle, &Core.graphics, x, y, width, height, color);
}
void PUBLIC_API(tico_graphics_draw_rect, tc_Rect rect, tc_Color color) {
}
void PUBLIC_API(tico_graphics_fill_rect, tc_Rect rect, tc_Color color);

void PUBLIC_API(tico_graphics_draw_circle, int x, int y, float radius, tc_Color color) {
  INTERNAL_API(tico_graphics_draw_circle, &Core.graphics, x, y, radius, color);
}
void PUBLIC_API(tico_graphics_fill_circle, int x, int y, float radius, tc_Color color) {
  INTERNAL_API(tico_graphics_fill_circle, &Core.graphics, x, y, radius, color);
}
void PUBLIC_API(tico_graphics_draw_circle_ex, int x, int y, float radius, int sides, tc_Color color) {
  INTERNAL_API(tico_graphics_draw_circle_ex, &Core.graphics, x, y, radius, sides, color);
}
void PUBLIC_API(tico_graphics_fill_circle_ex, int x, int y, float radius, int sides, tc_Color color) {
  INTERNAL_API(tico_graphics_fill_circle_ex, &Core.graphics, x, y, radius, sides, color);
}

void PUBLIC_API(tico_graphics_draw_triangle, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  INTERNAL_API(tico_graphics_draw_triangle, &Core.graphics, x0, y0, x1, y1, x2, y2, color);
}
void PUBLIC_API(tico_graphics_fill_triangle, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  INTERNAL_API(tico_graphics_fill_triangle, &Core.graphics, x0, y0, x1, y1, x2, y2, color);
}

void PUBLIC_API(tico_graphics_draw_line, int x0, int y0, int x1, int y1, tc_Color color) {
  INTERNAL_API(tico_graphics_draw_line, &Core.graphics, x0, y0, x1, y1, color);
}

/********************
 * Text
 ********************/

void tico_graphics_draw_text(const char *text, int x, int y, tc_Color color) {
  tico_graphics_draw_text_internal(&Core.graphics, text, x, y, color);
}
void tico_graphics_draw_text_scale(const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tico_graphics_draw_text_scale_internal(&Core.graphics, text, x, y, sx, sy, color);
}
void tico_graphics_draw_text_ex(const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  tico_graphics_draw_text_ex_internal(&Core.graphics, text, x, y, angle, sx, sy, cx, cy, color);
}

void tico_graphics_draw_text_font(tc_Font font, const char *text, int x, int y, tc_Color color) {
  tico_graphics_draw_text_font_internal(&Core.graphics, font, text, x, y, color);
}
void tico_graphics_draw_text_font_scale(tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tico_graphics_draw_text_font_scale_internal(&Core.graphics, font, text, x, y, sx, sy, color);
}
void tico_graphics_draw_text_font_ex(tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  tico_graphics_draw_text_font_ex_internal(&Core.graphics, font, text, x, y, angle, sx, sy, cx, cy, color);
}

void tico_graphics_draw_text_font_scale_width(tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color) {
  tico_graphics_draw_text_font_scale_width_internal(&Core.graphics, font, text, x, y, width, sx, sy, color);
}


/*==================*
 *      Input       *
 *==================*/

void PUBLIC_API(tico_input_update_mouse_scroll, float x, float y) {
  INTERNAL_API(tico_input_update_mouse_scroll, &Core.input, x, y);
}
void PUBLIC_API(tico_input_update_key, TIC_KEY_ key, int action) {
  INTERNAL_API(tico_input_update_key, &Core.input, key, action);
}
void PUBLIC_API(tico_input_update_mouse_pos, int x, int y) {
  INTERNAL_API(tico_input_update_mouse_pos, &Core.input, x, y);
}
void PUBLIC_API(tico_input_update_joy_button, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button, int action) {}
void PUBLIC_API(tico_input_update_mouse_button, TIC_MOUSEBUTTON_ button, int action) {
  INTERNAL_API(tico_input_update_mouse_button, &Core.input, button, action);
}

int tico_input_get_key_code(const char *name) {
  return tico_input_get_key_code_internal(&Core.input, name);
}
int tico_input_get_joy_btncode(const char *name) {
  return tico_input_get_joy_btncode_internal(&Core.input, name);
}
int tico_input_get_joy_axiscode(const char *name) {
  return tico_input_get_joy_axiscode_internal(&Core.input, name);
}
int tico_input_get_mouse_code(const char *name) {
  return tico_input_get_mouse_code_internal(&Core.input, name);
}

tc_bool tico_input_key_down(TIC_KEY_ key) {
  return tico_input_key_down_internal(&Core.input, key);
}
tc_bool tico_input_key_pressed(TIC_KEY_ key) {
  return tico_input_key_pressed_internal(&Core.input, key);
}
tc_bool tico_input_key_up(TIC_KEY_ key) {
  return tico_input_key_up_internal(&Core.input, key);
}
tc_bool tico_input_key_released(TIC_KEY_ key) {
  return tico_input_key_released_internal(&Core.input, key);
}

tc_bool tico_input_mouse_down(TIC_MOUSEBUTTON_ button) {
  return tico_input_mouse_down_internal(&Core.input, button);
}
tc_bool tico_input_mouse_pressed(TIC_MOUSEBUTTON_ button) {
  return tico_input_mouse_pressed_internal(&Core.input, button);
}
tc_bool tico_input_mouse_up(TIC_MOUSEBUTTON_ button) {
  return tico_input_mouse_up_internal(&Core.input, button);
}
tc_bool tico_input_mouse_released(TIC_MOUSEBUTTON_ button) {
  return tico_input_mouse_released_internal(&Core.input, button);
}
void tico_input_mouse_set_cursor(int cursor) {
}

tc_bool tico_input_joy_down(TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  return tico_input_joy_down_internal(&Core.input, jid, button);
}
tc_bool tico_input_joy_up(TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  return tico_input_joy_up_internal(&Core.input, jid, button);
}
tc_bool tico_input_joy_pressed(TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  return tico_input_joy_pressed_internal(&Core.input, jid, button);
}
tc_bool tico_input_joy_released(TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  return tico_input_joy_released_internal(&Core.input, jid, button);
}

void tico_input_get_mouse_pos(int *x, int *y) {
  tico_input_get_mouse_pos_internal(&Core.input, x, y);
}
void tico_input_fix_mouse() {
  tico_input_fix_mouse_internal(&Core.input);
}
void tico_input_release_mouse() {
  tico_input_release_mouse_internal(&Core.input);
}
void tico_input_get_mouse_delta(int *x, int *y) {
  tico_input_get_mouse_delta_internal(&Core.input, x, y);
}
void tico_input_get_mouse_scroll(float *x, float *y) {
  tico_input_get_mouse_scroll_internal(&Core.input, x, y);
}
void tico_input_get_mouse_scroll_delta(float *x, float *y) {
  tico_input_get_mouse_scroll_delta_internal(&Core.input, x, y);
}


/*==============*
 *    Window    *
 *==============*/

int tico_window_should_close() {
  return tico_window_should_close_internal(&Core.window);
}

void tico_window_swap_buffers() {
  tico_window_swap_buffers_internal(&Core.window);
}

void tico_window_get_pos(int *x, int *y) {
  tico_window_get_pos_internal(&Core.window, x, y);
}

void tico_window_set_pos(int x, int y) {
  tico_window_set_pos_internal(&Core.window, x, y);
}

int tico_window_get_width() {
  return tico_window_get_width_internal(&Core.window);
}

void tico_window_set_width(int width) {
  tico_window_set_width_internal(&Core.window, width);
}

int tico_window_get_height() {
  return tico_window_get_height_internal(&Core.window);
}
void tico_window_set_height(int height) {
  tico_window_set_height_internal(&Core.window, height);
}

void tico_window_get_size(int *width, int *height) {
  tico_window_get_size_internal(&Core.window, width, height);
}
void tico_window_set_size(int width, int height) {
  tico_window_set_size_internal(&Core.window, width, height);
}

const char* tico_window_get_title() {
  return tico_window_get_title_internal(&Core.window);
}
void tico_window_set_title(const char *title) {
  tico_window_set_title_internal(&Core.window, title);
}

tc_bool tico_window_is_fullscreen() {
  return tico_window_is_fullscreen_internal(&Core.window);
}
void tico_window_set_fullscreen(tc_bool fullscreen) {
  tico_window_set_fullscreen_internal(&Core.window, fullscreen);
}
void tico_window_toggle_fullscreen() {
  tico_window_toggle_fullscreen_internal(&Core.window);
}

tc_bool tico_window_is_resizable() {
  return tico_window_is_resizable_internal(&Core.window);
}
void tico_window_set_resizable(tc_bool resizable) {
  tico_window_set_resizable_internal(&Core.window, resizable);
}
void tico_window_toggle_resizable() {
  tico_window_toggle_resizable_internal(&Core.window);
}

tc_bool tico_window_get_vsync() {
  return tico_window_get_vsync_internal(&Core.window);
}
void tico_window_set_vsync(tc_bool vsync) {
  tico_window_set_vsync_internal(&Core.window, vsync);
}

/*===============*
 *    Plugins    *
 *===============*/

void PUBLIC_API(tico_plugin_add_plugin, const char *name, tc_Plugin plugin) {
  INTERNAL_API(tico_plugin_add_plugin, &Core.plugins, name, plugin);
}
void PUBLIC_API(tico_plugin_enable_plugin, const char *name) {
  INTERNAL_API(tico_plugin_enable_plugin, &Core.plugins, name);
}
void PUBLIC_API(tico_plugin_disable_plugin, const char *name) {
  INTERNAL_API(tico_plugin_disable_plugin, &Core.plugins, name);
}
tc_bool PUBLIC_API(tico_plugin_is_active, const char *name) {
  return INTERNAL_API(tico_plugin_is_active, &Core.plugins, name);
}
tc_Plugin* PUBLIC_API(tico_plugin_get, const char *name) {
  return INTERNAL_API(tico_plugin_get, &Core.plugins, name);
}

int PUBLIC_API(tico_plugin_module_init) {
  return INTERNAL_API(tico_plugin_module_init, &Core.plugins);
}
void PUBLIC_API(tico_plugin_module_terminate) {
  INTERNAL_API(tico_plugin_module_terminate, &Core.plugins);
}
void PUBLIC_API(tico_plugin_module_update) {
  INTERNAL_API(tico_plugin_module_init, &Core.plugins);
}
void PUBLIC_API(tico_plugin_module_begin_draw) {
  INTERNAL_API(tico_plugin_module_begin_draw, &Core.plugins);
}
void PUBLIC_API(tico_plugin_module_draw) {
  INTERNAL_API(tico_plugin_module_draw, &Core.plugins);
}
void PUBLIC_API(tico_plugin_module_end_draw) {
  INTERNAL_API(tico_plugin_module_end_draw, &Core.plugins);
}
#include "tico.h"

#define HASHMAP_IMPLEMENTATION
#include "external/hashmap.h"

tc_Core Core;

static void tic_window_move_callback(GLFWwindow *window, int x, int y) {
  Core.window.x = x;
  Core.window.y = y;
//   tic_lua_callback("moved");
}

static void tic_window_resize_callback(GLFWwindow *window, int width, int height) {
  // glViewport(0, 0, width, height);
}

static void tic_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  Core.window.width = width;
  Core.window.height = height;
//   tic_lua_callback("resized");
}

static void tic_window_character_callback(GLFWwindow *window, unsigned int codepoint) {
  // tic_ui_text_callback(codepoint);
//   tic_lua_callback("textinput");
}

static void tic_window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  Core.input.keyState.down[key] = tic_clamp(action, 0, 1);
  // tic_ui_key_callback(key, action);
}

static void tic_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  int fbutton = tic_clamp(button, 0, MOUSE_BUTTON_LAST);
  Core.input.mouseState.down[fbutton] = action;
  // tic_ui_mouse_btn_callback(button, Core.input.mouseState.x, Core.input.mouseState.y, action);
}

static void tic_mouse_pos_callback(GLFWwindow *window, double posX, double posY)
{
  Core.input.mouseState.x = posX;
  Core.input.mouseState.y = posY;
//   tic_lua_callback("mousemoved");

  // tic_ui_mouse_pos_callback(posX, posY);
}

static void tic_window_focus_callback(GLFWwindow *window, int focused)
{
  if (focused == 1)
    tic_audio_start_device();
  else
    tic_audio_stop_device();
}

static void tic_joystick_callback(int jid, int event) {
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

static void tic_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  // tic_ui_mouse_scroll_callback(xoffset, yoffset);
  Core.input.mouseState.scrollX = xoffset * -10;
  Core.input.mouseState.scrollY = yoffset * -10;

//   tic_lua_callback("mousescroll");
}

tc_bool tic_init(tc_Config *config) {
  if (!glfwInit()) {
    TRACEERR("Failed to init GLFW");
    exit(-1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  Core.config = *config;
  Core.state.mode = config->mode;
  if (tic_filesystem_file_exists("data.pack")) tic_filesystem_set_packed(tc_true);

  TRACELOG("packed: %d", Core.fs.packed);

  tic_window_init(&Core.window, config->title, config->width, config->height, config->windowFlags);
  tic_resources_init(&Core.resources);
  // Core.packed = tc_false;

  if (gl3wInit()) {
    TRACEERR("Failed to init gl3w");
    exit(-1);
  }

  // Window callbacks
  glfwSetWindowPosCallback(Core.window.handle, tic_window_move_callback);
  glfwSetWindowSizeCallback(Core.window.handle, tic_window_resize_callback);
  glfwSetKeyCallback(Core.window.handle, tic_window_key_callback);
  glfwSetMouseButtonCallback(Core.window.handle, tic_mouse_button_callback);
  glfwSetCursorPosCallback(Core.window.handle, tic_mouse_pos_callback);
  glfwSetWindowFocusCallback(Core.window.handle, tic_window_focus_callback);
  glfwSetCharCallback(Core.window.handle, tic_window_character_callback);
  glfwSetScrollCallback(Core.window.handle, tic_mouse_scroll_callback);
  glfwSetJoystickCallback(tic_joystick_callback);
  glfwSetFramebufferSizeCallback(Core.window.handle, tic_framebuffer_size_callback);
  TRACELOG("Setup callbacks");

  if (config->lang == TIC_LUA_GAME) tic_lua_init(&Core.config);
#ifdef LUA_LANG
#endif
#ifdef WREN_LANG
  if (config->lang == TIC_WREN_GAME) tic_wren_init();
#endif

  tic_input_init(&Core.input, 0);

  tic_render_init(&Core.render);
  TRACELOG("Render created");

  Core.defaultFont = tic_font_load_default();
  TRACELOG("Default font loaded");

  tic_audio_init();

  // glViewport(0, 0, Core.window.width, Core.window.height);
//   tic_lua_load();

  // tic_ui_init(Core.defaultFont);

  return tc_true;
}

void tic_init_config_json(tc_Config *config) {
  if (!tic_filesystem_file_exists("config.json")) return;
  cJSON *jsonConfig = tic_json_open("config.json");
  cJSON *window = NULL;
  const char *name = tic_json_get_opt_string(jsonConfig, "name", config->title);
  if (name) {
    strcpy(config->title, name);
  }
  const char *lang = tic_json_get_opt_string(jsonConfig, "lang", "");
  if (!strcmp(lang, "wren")) {
    config->lang = TIC_WREN_GAME;
  }
  window = tic_json_get_object(jsonConfig, "window");
  if (window) {
    config->width = tic_json_get_opt_number(window, "width", config->width);
    config->height = tic_json_get_opt_number(window, "height", config->height);
    if (!tic_json_get_opt_boolean(window, "resizable", tc_true)) config->windowFlags ^= TIC_WINDOW_RESIZABLE;
    if (tic_json_get_opt_boolean(window, "fullscreen", tc_false)) config->windowFlags |= TIC_WINDOW_FULLSCREEN;
    if (!tic_json_get_opt_boolean(window, "vsync", tc_true)) config->windowFlags ^= TIC_WINDOW_VSYNC;
  }

  tic_json_delete(jsonConfig);
}

tc_Config tic_config_init(const char *title, int width, int height, int argc, char ** argv) {
  tic_filesystem_init(&Core.fs);
  tc_Config config = {0};
  config.title[0] = '\0';
  if (title) sprintf(config.title, "%s", title);
  else if (!config.title[0]) strcpy((char*)config.title, "tico " TICO_VERSION);
  config.width = tic_max(width, 10);
  config.height = tic_max(height, 10);
  config.windowFlags = 0;
  config.windowFlags |= TIC_WINDOW_RESIZABLE;
  config.windowFlags |= TIC_WINDOW_VSYNC;
  config.flags = 0;
  config.mode = TIC_MODE_GAME;
  config.argc = argc;
  config.argv = argv;
  config.lang = TIC_LUA_GAME;


  if (argc < 2) strcpy(config.path, ".");
  else strcpy(config.path, argv[1]);
  tic_filesystem_set_path(config.path);

  tic_init_config_json(&config);

  return config;
}

void tic_update() {
  tic_timer_update();
  tic_poll_events();
}
void tic_poll_events() {
	tic_input_update(&Core.input);
  glfwPollEvents();
}

static char *errMsg;

static int tic_error_load() {
}

static int tic_error_step() {
  tic_graphics_draw_text_scale("Error", 32, 32, 2, 2, WHITE);
  // tic_graphics_draw_text(errMsg, 32, 64, WHITE);
#ifdef WREN_LANG
  int y = 64;
  tc_WrenTraceback *trace = Core.wren.trace;
  tic_graphics_draw_text(trace->message, 32, y, WHITE);

  y += 24;
  tic_graphics_draw_text("Traceback", 32, y, WHITE);

  y += 24;
  trace = trace->next;
  while(trace->next) {
    tic_graphics_draw_text(trace->message, 32, y, WHITE);
    y += 16;
    trace = trace->next;
  }
#endif
}

void tic_error(char *message) {
  // TRACELOG("%s", message);
#ifdef WREN_LANG
  if (!Core.wren.trace) {
    Core.wren.trace = malloc(sizeof(tc_WrenTraceback));
    strcpy(Core.wren.trace->message, message);
    Core.wren.trace->next = NULL;
  } else {
    tc_WrenTraceback *trace = Core.wren.trace;
    while(trace->next) trace = trace->next;

    tc_WrenTraceback *next = malloc(sizeof(tc_WrenTraceback));

    strcpy(next->message, message);
    next->next = NULL;
    trace->next = next;
  }
#endif
//   TRACEERR("%s", message);
  Core.state.load = tic_error_load;
  Core.state.step = tic_error_step;
}

void tic_begin_draw() {
  tc_Canvas canvas;
  canvas.id = 0;
  canvas.width = Core.window.width;
  canvas.height = Core.window.height;
	tic_batch_begin(&Core.render.batch);
  glEnable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
  // glUseProgram(Core.render.state.currentShader.program);
  // tic_shader_send_world(Core.render.state.currentShader);
  tic_batch_begin(&Core.render.batch);
//   tc_attach_canvas(Core.render.state.defaultCanvas);
  // tic_ui_begin();
  tic_shader_attach(Core.render.state.defaultShader);
  tic_canvas_attach(canvas);
}

void tic_end_draw() {
  tic_canvas_detach();
  tic_shader_detach();
  // tic_canvas_disable();
  // tic_ui_end();
  tic_batch_flush(&Core.render.batch);
  tic_batch_draw(&Core.render.batch);

  tic_swap_buffers();
}

void tic_main_loop() {
  Core.state.load();
  while(!tic_window_should_close()) {
    tic_update();

    tic_graphics_clear(BLACK);
    tic_begin_draw();

    Core.state.step(tic_timer_get_delta());

    tic_end_draw();
  }
}

void tic_swap_buffers() {
	glfwSwapBuffers(Core.window.handle);
}

void tic_close() {
  glfwSetWindowShouldClose(Core.window.handle, 1);
}

void tic_terminate() {
	tic_input_destroy(&Core.input);
	tic_render_destroy(&Core.render);
  tic_window_destroy(&Core.window);
  tic_audio_terminate();
  tic_resources_destroy(&Core.resources);
  // lua_close(Core.lua.L);
//   TRACELOG("Lua close");
  glfwTerminate();
  TRACELOG("Exiting tico");
}

/****************
 * Timer
 ****************/

void tic_timer_update() {
  Core.timer.currentTime = glfwGetTime();
  Core.timer.delta = Core.timer.currentTime - Core.timer.lastTime;
  Core.timer.lastTime = Core.timer.currentTime;

  float delta = Core.timer.currentTime - Core.timer.fpsLastTime;
  Core.timer.frames++;
  if (delta >= 1) {
    Core.timer.fps = Core.timer.frames;
    Core.timer.frames = 0;
    Core.timer.fpsLastTime = Core.timer.currentTime;
    // tic_timer_wait(delta);
  }
}

float tic_timer_get_time() {
  return Core.timer.currentTime;
}

float tic_timer_get_delta() {
  return Core.timer.delta;
}

int tic_timer_get_fps() {
  return Core.timer.fps;
}

void tic_timer_wait(float ms) {
	#if defined(_WIN32)
		Sleep((unsigned int) ms);
	#elif defined(__linux__)
		struct timespec req = {0};
		time_t sec = (int)(ms/1000.f);
		ms -= (sec * 1000);
		req.tv_sec = sec;
		req.tv_nsec = ms * 1000000L;

		while(nanosleep(&req, &req) == -1) continue;
	#elif defined(__APPLE__)
			usleep(ms * 1000.f);
	#endif
}

/*=======================
 * Utils
 *=======================*/

char *tic_replace_char(char *str, tc_uint8 find, tc_uint8 replace) {
  char *currentPos = (char*)strchr((const char*)str, find);
  while(currentPos) {
    *currentPos = replace;
    currentPos = (char*)strchr((const char*)currentPos, find);
  }

  return str;
}

#define MAXUNICODE 0x10FFFF

tc_uint8* tic_utf8_codepoint(tc_uint8 *p, int* codepoint) {
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
void tic_utf8_encode(tc_uint8* c, int codepoint) {
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

int tic_utf8_decode(const tc_uint8 *p) {
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

// void tc_scissor(int x, int y, int w, int h) {
// //   tc_reset_batch(&Core.render);
// //   GLint view[4];
// //   glGetIntegerv(GL_VIEWPORT, view);
// //   glScissor(x, view[3] - (y+h), w, h);
// }

void tic_write_buffer_header(const char *name, const char *text, size_t size) {
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
  tic_filesystem_write_file("out.txt", out, 0, 0);
}

void tic_set_clipboard(const char *text) {
  glfwSetClipboardString(Core.window.handle, text);
}

const char* tic_get_clipboard() {
  return glfwGetClipboardString(Core.window.handle);
}

/*=======================
 * Debug
 *=======================*/

#include <time.h>
#include <stdarg.h>

TIC_API void tic_log(int type, const char *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  tc_uint8 err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR: ");
  tc_uint8 buffer[1024];
  tc_uint8 bufmsg[512];

  tm_now = localtime(&t);
  tc_uint8 buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s", buf, err);
  va_start(args, fmt);
  vfprintf(stderr, (const char*)fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

TIC_API void tic_tracelog(int type, const char *file, const char *function, int line, const char *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  tc_uint8 err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR in ");
  tc_uint8 buffer[1024];
  tc_uint8 bufmsg[512];

  tm_now = localtime(&t);
  tc_uint8 buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  // sprintf(buffer, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  va_start(args, fmt);
  // vsprintf(bufmsg, (const char*)fmt, args);
  vfprintf(stderr, (const char*)fmt, args);
  // fprintf(stderr, "%s", bufmsg);
  va_end(args);
  fprintf(stderr, "\n");
  // strcat(buffer, bufmsg);
  // tc_editor_write_log(buffer);
}

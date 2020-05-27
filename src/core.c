#include "tinycoffee.h"

#define TC_MATH_IMPLEMENTATION
#include "tcmath.h"

#define TC_AUDIO_IMPLEMENTATION
#include "audio.h"

#define TC_WINDOW_IMPLEMENTATION
#include "window.h"

#define TC_RENDER_IMPLEMENTATION
#include "render.h"

#define TC_FONT_IMPLEMENTATION
#include "font.h"

#define TC_FILESYSTEM_IMPLEMENTATION
#include "fs.h"

#define TC_INPUT_IMPLEMENTATION
#include "input.h"

tc_core CORE;

static void tc_window_resize_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  CORE.window.width = width;
  CORE.window.height = height;
}

static void tc_window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	CORE.input.keyboardState.keyDown[key] = clamp(action, 0, 1);
}

static void tc_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	int fbutton = clamp(button, 0, MOUSE_BUTTON_LAST);
	CORE.input.mouseState.buttonDown[fbutton] = action;
}

static void tc_mouse_pos_callback(GLFWwindow *window, double posX, double posY)
{
	CORE.input.mouseState.x = posX;
	CORE.input.mouseState.y = posY;
}

static void tc_window_focus_callback(GLFWwindow *window, int focused)
{
	if (focused == 1)
		tc_start_device();
	else
		tc_stop_device();
}

TCDEF tc_bool tc_config_init (tc_config *config, const tc_uint8 *title, tc_uint16 width, tc_uint16 height) {
  if (title) sprintf((char *)config->title, "%s", title);
  else sprintf((char *)config->title, "tiny coffee");
  config->width = width;
  config->height = height;
  config->windowFlags = 0;
  config->packed = 0;
}

TCDEF tc_bool tc_init(tc_config *config) {
    //Init wren lang
#ifdef WREN_LANG
  CORE.wren = tc_init_wren(config);
#endif
#ifdef LUA_LANG
  CORE.lua = tc_init_lua();
#endif

  if (!glfwInit()) {
    ERROR("Failed to init GLFW");
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  CORE.window = tc_create_window(config->title, config->width, config->height, config->windowFlags);

  if (gl3wInit()) {
    ERROR("Failed to init gl3w");
    return -1;
  }

	// Window callbacks
  glfwSetWindowSizeCallback(CORE.window.handle, tc_window_resize_callback);
	glfwSetKeyCallback(CORE.window.handle, tc_window_key_callback);
	glfwSetMouseButtonCallback(CORE.window.handle, tc_mouse_button_callback);
	glfwSetCursorPosCallback(CORE.window.handle, tc_mouse_pos_callback);
	glfwSetWindowFocusCallback(CORE.window.handle, tc_window_focus_callback);

  TRACELOG("Setup callbacks");

  // Load render
  tc_uint16 vertexShader, fragmentShader;
  tc_load_default_shader(&vertexShader, &fragmentShader);
  // CORE.render = (tc_render){0};
  CORE.render = tc_create_render(vertexShader, fragmentShader);

  // Init audio system
  tc_init_audio();

  CORE.packed = TC_FALSE;
  if (tc_fs_file_exists("data.pack")) CORE.packed = TC_TRUE;

  TRACELOG("packed: %d", CORE.packed);

  // Init font lib
  // tc_init_font_lib();
  CORE.defaultFont = tc_load_default_font();

#ifdef WREN_LANG
  tc_wren_game_load(CORE.wren);
#endif


  glViewport(0, 0, CORE.window.width, CORE.window.height);
//   glLineWidth(2);
  TRACELOG("tico initiated");
  return TC_TRUE;
}

TCDEF void tc_terminate() {
  tc_destroy_render(&CORE.render);
  tc_destroy_window(&CORE.window);
  glfwTerminate();
}

TCDEF void tc_poll_events() {
	tc_input_poll(&CORE.input);
	tc_update_timer();
	glfwPollEvents();
}

TCDEF void tc_main_loop() {
  while(!tc_should_close()) {
    tc_poll_events();
    tc_scripting_wren_update();

    tc_begin_draw();
    tc_scripting_wren_draw();
    tc_end_draw();
  }
}

TCDEF void tc_clear(tc_color color) {
	glClearColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

TCDEF void tc_update_timer() {
	CORE.timer.currentTime = glfwGetTime();
	CORE.timer.delta = CORE.timer.currentTime - CORE.timer.lastTime;
	CORE.timer.lastTime = CORE.timer.currentTime;

	float delta = CORE.timer.currentTime - CORE.timer.fpsLastTime;
	CORE.timer.frames++;
	if (delta >= 1) {
		CORE.timer.fps = CORE.timer.frames;
		CORE.timer.frames = 0;
		CORE.timer.fpsLastTime = CORE.timer.currentTime;
	}
}

TCDEF void tc_swap_buffers() {
  glfwSwapBuffers(CORE.window.handle);
}

TCDEF tc_bool tc_should_close() {
  return tc_window_should_close(CORE.window);
}

TCDEF void tc_close() {
  tc_close_window(CORE.window);
}

TCDEF void tc_begin_draw() {
  tc_begin_batch(&CORE.render);
  glUseProgram(CORE.render.state.defaultShader.id);
  tc_shader_send_worldview(CORE.render.state.currentShader);
//   glEnable(GL_SCISSOR_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

TCDEF void tc_end_draw() {
  tc_end_batch(&CORE.render);
  tc_flush_batch(&CORE.render);

  tc_swap_buffers();
}

TCDEF tc_render tc_get_render() {
  return CORE.render;
}

TCDEF tc_window tc_get_window() {
  return CORE.window;
}

/* Textures */
TCDEF tc_texture tc_create_texture(void *data, tc_int16 width, tc_int16 height) {
  tc_texture tex = {0};
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  tex.width = width;
  tex.height = height;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return tex;
}
TCDEF tc_texture tc_load_texture(const tc_uint8 *filename) {
  size_t size;
  tc_uint8 *buffer = (tc_uint8*)tc_read_file(filename, &size);
  tc_texture texture = tc_load_texture_from_memory(buffer, size);
  if (texture.id == 0) {
    ERROR("Failed to load image '%s'", filename);
  }
  free(buffer);
  return texture;
}

TCDEF tc_texture tc_load_texture_internal(const tc_uint8 *filename) {
  size_t size;
  tc_uint8 *buffer = (tc_uint8*)tc_fs_read_file_from_zip("data.bin", filename, &size);

  tc_texture texture = tc_load_texture_from_memory(buffer, size);
  if (texture.id == 0) {
    ERROR("Failed to load image '%s'", filename);
  }
  free(buffer);
  return texture;
}

TCDEF tc_texture tc_load_texture_external(const tc_uint8 *filename) {
  size_t size;
  tc_uint8 *buffer = (tc_uint8*)tc_fs_read_file(filename, &size, 1);

  tc_texture texture = tc_load_texture_from_memory(buffer, size);
  if (texture.id == 0) {
    ERROR("Failed to load image '%s'", filename);
  }
  free(buffer);
  return texture;
}

TCDEF tc_texture tc_load_texture_from_memory(const tc_uint8 *buffer, size_t bufferSize) {
  tc_texture tex = {0};

  int width, height, channels;

  tc_uint8 *data = stbi_load_from_memory(buffer, bufferSize, &width, &height, &channels, 0);
  if (!data) {
//     ERROR("TEXTURE", "Failed to load image '%s'\n", filename);
    return tex;
  }
  GLenum format;
  switch (channels) {
    case 1:
      format = GL_RED;
      break;
    case 2:
      format = GL_RG;
      break;
    case 3:
      format = GL_RGB;
      break;
    default:
      format = GL_RGBA;
  }

  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  tex.width = width;
  tex.height = height;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, GL_FALSE, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return tex;
}

TCDEF void tc_delete_texture(tc_texture *tex) {
	glDeleteBuffers(1, &tex->id);
}

/** draw texture **/
TCDEF void tc_draw_texture(tc_texture tex, tc_int32 x, tc_int32 y, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  tc_render_draw_quad(&CORE.render, tex.id, (tc_rectangle){0, 0, tex.width, tex.height}, x, y, tex.width, tex.height, color);
}
TCDEF void tc_draw_texture_scale(tc_texture tex, tc_int32 x, tc_int32 y, float scaleX, float scaleY, tc_color color) {
	tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_render_draw_quad_scale(&CORE.render, tex.id, (tc_rectangle){0, 0, tex.width, tex.height}, x, y, tex.width, tex.height, scaleX, scaleY, color);
}
TCDEF void tc_draw_texture_ex(tc_texture tex, tc_int32 x, tc_int32 y, float angle, float sx, float sy, tc_int32 cx, tc_int32 cy, tc_color color) {
	tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_render_draw_quad_ex(&CORE.render, tex.id, (tc_rectangle){0, 0, tex.width, tex.height}, x, y, tex.width, tex.height, angle, sx, sy, cx, sy, color);
}

/** draw texture part **/
TCDEF void tc_draw_texture_part(tc_texture tex, tc_rectangle rect, tc_int32 x, tc_int32 y, tc_color color) {
	tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_render_draw_quad(&CORE.render, tex.id, rect, x, y, tex.width, tex.height, color);
}

TCDEF void tc_draw_texture_part_scale(tc_texture tex, tc_rectangle rect, tc_int32 x, tc_int32 y, float sx, float sy, tc_color color) {
	tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_render_draw_quad_scale(&CORE.render, tex.id, rect, x, y, tex.width, tex.height, sx, sy, color);
}

TCDEF void tc_draw_texture_part_ex(tc_texture tex, tc_rectangle rect, tc_int32 x, tc_int32 y, float angle, float sx, float sy, tc_int32 cx, tc_int32 cy, tc_color color) {
	tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_render_draw_quad_ex(&CORE.render, tex.id, rect, x, y, tex.width, tex.height, angle, sx, sy, cx, cy, color);
}

/* draw primitives */
TCDEF void tc_draw_rectangle(tc_int32 x, tc_int32 y, tc_int32 width, tc_int32 height, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_LINES);
  tc_render_draw_quad(&CORE.render, CORE.render.state.defaultTextureId, (tc_rectangle){0, 0, width, height}, x, y, width, height, color);
}
TCDEF void tc_fill_rectangle(tc_int32 x, tc_int32 y, tc_int32 width, tc_int32 height, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  tc_render_draw_quad(&CORE.render, CORE.render.state.defaultTextureId, (tc_rectangle){0, 0, width, height}, x, y, width, height, color);
}

TCDEF void tc_draw_circle(tc_int32 x, tc_int32 y, float radius, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_LINES);
  tc_render_draw_circle(&CORE.render, CORE.render.state.defaultTextureId, x, y, radius, color);
}
TCDEF void tc_fill_circle(tc_int32 x, tc_int32 y, float radius, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  tc_render_draw_circle(&CORE.render, CORE.render.state.defaultTextureId, x, y, radius, color);
}

TCDEF void tc_draw_triangle(tc_int32 x0, tc_int32 y0, tc_int32 x1, tc_int32 y1, tc_int32 x2, tc_int32 y2, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_LINES);
  tc_render_draw_triangle(&CORE.render, CORE.render.state.defaultTextureId, x0, y0, x1, y1, x2, y2, color);
}
TCDEF void tc_fill_triangle(tc_int32 x0, tc_int32 y0, tc_int32 x1, tc_int32 y1, tc_int32 x2, tc_int32 y2, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  tc_render_draw_triangle(&CORE.render, CORE.render.state.defaultTextureId, x0, y0, x1, y1, x2, y2, color);
}

TCDEF void tc_draw_text(const tc_uint8 *text, tc_int32 x, tc_int32 y, tc_color color) {
  tc_draw_text_font(CORE.defaultFont, text, x, y, color);
}

TCDEF void tc_draw_text_scale(const tc_uint8 *text, tc_int32 x, tc_int32 y, float sx, float sy, tc_color color) {
  tc_draw_text_font_scale(CORE.defaultFont, text, x, y, sx, sy, color);
}

TCDEF void tc_draw_text_ex(const tc_uint8 *text, tc_int32 x, tc_int32 y, float angle, float sx, float sy, tc_int32 cx, tc_int32 cy, tc_color color) {
	// tc_render_draw_quad_ex(&CORE.render, CORE.defaultFont.texture.id, )
}

TCDEF void tc_draw_text_font(tc_font font, const tc_uint8 *text, tc_int32 x, tc_int32 y, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  float x0 = 0;
  float y0 = 0;
  tc_uint8 *p = (tc_uint8*)text;
	while (*p) {
		vec2 pos;
		tc_rectangle rect;
		tc_int32 codepoint;
		p = tc_utf8_codepoint(p, &codepoint);
		tc_font_get_rect(font, codepoint, &x0, &y0, &pos, &rect);
		tc_render_draw_quad(&CORE.render, font.texture.id, rect, x + pos.x, y + pos.y, font.texture.width, font.texture.height, color);
	}
}

TCDEF void tc_draw_text_font_scale(tc_font font, const tc_uint8 *text, tc_int32 x, tc_int32 y, float sx, float sy, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
  float x0 = 0;
  float y0 = 0;
  tc_uint8 *p = (tc_uint8*)text;
	while (*p) {
		vec2 pos;
		tc_rectangle rect;
		tc_int32 codepoint;
		p = tc_utf8_codepoint(p, &codepoint);
		tc_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy);
		tc_render_draw_quad_scale(&CORE.render, font.texture.id, rect, x + pos.x, y + pos.y, font.texture.width, font.texture.height, sx, sy, color);
	}
}


/* Canvas */
TCDEF tc_canvas tc_create_canvas(tc_int16 width, tc_int16 height) {
	tc_canvas canvas = {0};
	glGenFramebuffers(1, &canvas.id);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);

	canvas.tex = tc_create_texture(NULL, width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas.tex.id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return canvas;
}
TCDEF void tc_delete_canvas(tc_canvas *canvas) {
	tc_delete_texture(&canvas->tex);
	glDeleteFramebuffers(1, &canvas->id);
}


TCDEF void tc_set_canvas(tc_canvas canvas) {
	tc_reset_batch(&CORE.render);
	glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
	glViewport(0, 0, canvas.tex.width, canvas.tex.height);
	CORE.render.state.currentCanvas = canvas;
	tc_shader_send_worldview(CORE.render.state.currentShader);
}
TCDEF void tc_unset_canvas(tc_canvas canvas) {
	tc_reset_batch(&CORE.render);
	glViewport(0, 0, CORE.window.width, CORE.window.height);
	CORE.render.state.currentCanvas = (tc_canvas){0};
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	tc_shader_send_worldview(CORE.render.state.currentShader);
}

TCDEF void tc_draw_canvas(tc_canvas canvas, tc_int32 x, tc_int32 y, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_rectangle rect = {0, 0, canvas.tex.width, canvas.tex.height};
	tc_render_draw_quad(&CORE.render, canvas.tex.id, rect, x, y, canvas.tex.width, -canvas.tex.height, color);
}
TCDEF void tc_draw_canvas_scale(tc_canvas canvas, tc_int32 x, tc_int32 y, float sx, float sy, tc_color color) {
  tc_render_draw_mode(&CORE.render, TC_TRIANGLES);
	tc_rectangle rect = {0, 0, canvas.tex.width, canvas.tex.height};
	tc_render_draw_quad_scale(&CORE.render, canvas.tex.id, rect, x, y, canvas.tex.width, -canvas.tex.height, sx, sy, color);
}

/* Shader */
TCDEF tc_shader tc_create_shader(const tc_uint8 *vertexSource, const tc_uint8 *fragmentSource) {
	tc_shader shader = {0};
	unsigned int vertexShader = tc_compile_shader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = tc_compile_shader(fragmentSource, GL_FRAGMENT_SHADER);

	shader.id = tc_load_shader_program(vertexShader, fragmentShader);

	return shader;
}

TCDEF tc_uint16 tc_compile_shader(const tc_uint8 *source, tc_uint16 type) {
	unsigned int shader = glCreateShader(type);

	const tc_uint8 *shaderType = (tc_uint8*)(type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

	glShaderSource(shader, 1, (const GLchar* const*)&source, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
    ERROR("%s::Compilation failed: %s", shaderType, infoLog);
	}

	return shader;
}

TCDEF tc_uint16 tc_load_shader_program(tc_uint16 vertexShader, tc_uint16 fragmentShader) {
	int success;
	char infoLog[512];

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    ERROR("Linking failed: %s", infoLog);
	}

	return shaderProgram;
}

TCDEF void tc_set_shader(tc_shader shader) {
	tc_reset_batch(&CORE.render);
	glUseProgram(shader.id);
	CORE.render.state.currentShader = shader;
	tc_shader_send_worldview(shader);
}

TCDEF void tc_unset_shader(void) {
	tc_reset_batch(&CORE.render);
	glUseProgram(CORE.render.state.defaultShader.id);
	CORE.render.state.currentShader = CORE.render.state.defaultShader;
	tc_shader_send_worldview(CORE.render.state.currentShader);
}

TCDEF tc_shader tc_load_default_shader(tc_uint16 *vertexShader, tc_uint16 *fragmentShader) {
	const tc_uint8 *vertexSource = (const tc_uint8*)"#version 330\n"
														 "layout (location = 0) in vec2 in_Pos;\n"
														 "layout (location = 1) in vec4 in_Color;\n"
														 "layout (location = 2) in vec2 in_Texcoord;\n"
														 "varying vec4 v_Color;\n"
														 "varying vec2 v_Texcoord;\n"
														 "uniform mat4 world;\n"
														 "uniform mat4 view;\n"
														 "uniform mat4 model;\n"
														 "void main()\n"
														 "{\n"
														 "	gl_Position = world * view * model *  vec4(in_Pos, 0.0, 1.0);\n"
														 "	//gl_Position = vec4(in_Pos, 0.0, 1.0);\n"
														 "	v_Color = in_Color;\n"
														 "	v_Texcoord = in_Texcoord;\n"
														 "}\n";

	const tc_uint8 *fragmentSource = (const tc_uint8*)"#version 330\n"
															 "out vec4 FragColor;\n"
															 "varying vec4 v_Color;\n"
															 "varying vec2 v_Texcoord;\n"
															 "uniform sampler2D gm_BaseTexture;\n"
															 "void main()\n"
															 "{\n"
															 "	FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
															 "}\n";

  tc_shader shader = {0};
	*vertexShader = tc_compile_shader(vertexSource, GL_VERTEX_SHADER);
	*fragmentShader = tc_compile_shader(fragmentSource, GL_FRAGMENT_SHADER);

	// shader.id = tc_load_shader_program(*vertexShader, *fragmentShader);
  TRACELOG("Default shader created");
	return shader;
}

TCDEF void tc_shader_send_worldview(tc_shader shader) {
	matrix world;
	matrix mview;
	matrix_identity(&mview);
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, view);
	matrix_ortho(&world, 0, view[2], view[3], 0, 0, 1);

	tc_shader_send_uniform(shader, (tc_uint8*)"world", world.data, TC_UNIFORM_MATRIX);
	tc_shader_send_uniform(shader, (tc_uint8*)"view", mview.data, TC_UNIFORM_MATRIX);
}

TCDEF void tc_shader_send_uniform(tc_shader shader, const tc_uint8 *name, void *value, TC_SHADER_UNIFORM_ type) {
	GLuint uniform = glGetUniformLocation(shader.id, (const GLchar*)name);
	float *val = (float *)value;
	switch (type)
	{
	case TC_UNIFORM_FLOAT:
		glUniform1fv(uniform, 1, val);
		break;
	case TC_UNIFORM_VEC2:
		glUniform2fv(uniform, 1, val);
		break;
	case TC_UNIFORM_VEC3:
		glUniform3fv(uniform, 1, val);
		break;
	case TC_UNIFORM_MATRIX:
		glUniformMatrix4fv(uniform, 1, GL_FALSE, val);
		break;
  default:
    printf("ok\n");
	}
}

/* Input */

TCDEF tc_bool tc_is_key_down(TC_KEYBOARD_KEY_ key) {
	return tc_input_key_down(CORE.input, key);
}

TCDEF tc_bool tc_is_key_up(TC_KEYBOARD_KEY_ key) {
	return tc_input_key_up(CORE.input, key);
}

TCDEF tc_bool tc_is_key_pressed(TC_KEYBOARD_KEY_ key) {

	return tc_input_key_pressed(CORE.input, key);
}

TCDEF tc_bool tc_is_key_released(TC_KEYBOARD_KEY_ key) {
	return tc_input_key_released(CORE.input, key);
}

TCDEF tc_bool tc_is_mouse_down(TC_MOUSE_BUTTON_ button) {
	return tc_input_mouse_down(CORE.input, button);
}

TCDEF tc_bool tc_is_mouse_up(TC_MOUSE_BUTTON_ button) {
	return tc_input_mouse_up(CORE.input, button);
}

TCDEF tc_bool tc_is_mouse_pressed(TC_MOUSE_BUTTON_ button) {
	return tc_input_mouse_pressed(CORE.input, button);
}

TCDEF tc_bool tc_is_mouse_released(TC_MOUSE_BUTTON_ button) {
	return tc_input_mouse_released(CORE.input, button);
}

// /* Wren */
// #ifdef WREN_LANG
// #define TC_WREN_IMPLEMENTATION
// #include "tcwren.h"
// #endif

/* Timer */
TCDEF float tc_get_delta() {
	return CORE.timer.delta;
}

TCDEF tc_int16 tc_get_fps() {
	return CORE.timer.fps;
}

TCDEF float tc_get_time() {
	return glfwGetTime();
}

/* Filesystem */

TCDEF unsigned char *tc_read_file(const tc_uint8 *filename, size_t *outSize) {
  if (CORE.packed) {
    return (unsigned char*)tc_fs_read_file_from_zip((const tc_uint8*)"data.pack", filename, outSize);
  } else {
    return (unsigned char*)tc_fs_read_file(filename, outSize, TC_TRUE);
  }
  return NULL;
}

TCDEF void tc_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode) {
  if (CORE.packed) {
    tc_fs_write_file_to_zip((const tc_uint8*)"data.pack", filename, text, size, mode);
  } else {
    tc_fs_write_file(filename, text, size, mode);
  }
  return;
}

TCDEF tc_bool tc_file_exists(const tc_uint8 *filename) {
  if (CORE.packed) {
    return tc_fs_file_exists_in_zip("data.pack", filename);
  } else {
    return tc_fs_file_exists(filename);
  }
}
TCDEF void tc_mkdir(const tc_uint8 *path);
TCDEF void tc_rmdir(const tc_uint8 *path);

/* Scripting */
TCDEF void tc_scripting_wren_update() {
	#ifdef WREN_LANG
	tc_wren_game_update(CORE.wren);
	#endif
}

TCDEF void tc_scripting_wren_draw() {
	#ifdef WREN_LANG
	tc_wren_game_draw(CORE.wren);
	#endif
}

TCDEF void tc_scripting_lua_update() {
  #ifdef LUA_LANG
  tc_lua_update(CORE.lua);
  #endif
}

/* Utils */
TCDEF tc_int8 *tc_replace_char(tc_int8 *str, tc_uint8 find, tc_uint8 replace) {
  tc_int8 *currentPos = (tc_int8*)strchr((const char*)str, find);
  while(currentPos) {
    *currentPos = replace;
    currentPos = (tc_int8*)strchr((const char*)currentPos, find);
  }

  return str;
}

#define MAXUNICODE 0x10FFFF

TCDEF tc_uint8* tc_utf8_codepoint(tc_uint8 *p, tc_int32* codepoint) {
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

TCDEF tc_uint16 tc_utf8_decode(const tc_uint8 *p) {
  static const tc_uint32 limits[] = {0xFF, 0x7F, 0x7FF, 0xFFFF};
  const tc_uint8 *s = (const tc_uint8 *)p;
  tc_uint32 c = s[0];
  tc_uint32 res = 0;  /* final result */
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

/* Log */

TCDEF void tc_log(int type, const tc_uint8 *file, const tc_uint8 *function, tc_uint16 line, const tc_uint8 *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  tc_uint8 err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR in ");

  tm_now = localtime(&t);
  tc_uint8 buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  va_start(args, fmt);
  vfprintf(stderr, (const char*)fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

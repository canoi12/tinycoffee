#include "tico.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

int tico_graphics_init(tc_Graphics* graphics) {
	stack_arr_init(&graphics->canvas_stack, CANVAS_STACK_SIZE);
	stack_arr_init(&graphics->shader_stack, SHADER_STACK_SIZE);
	stack_arr_init(&graphics->matrix_stack, MATRIX_STACK_SIZE);

  vec_init(&graphics->canvas_vec);

	if (!tico_render_init(&graphics->render)) return 0;

	graphics->default_font = tico_font_load_default();
	graphics->default_shader = tico_shader_load_default(&graphics->default_vertex, &graphics->default_fragment);
  graphics->default_texture = tico_texture_create(WHITE.data, 1, 1, GL_RGBA);

	LOG("Graphics Module initiated");
	return 1;
}

void tico_graphics_terminate(tc_Graphics *graphics) {
	// stack_arr_destroy(&graphics->canvas_stack);
	// stack_arr_destroy(&graphics->shader_stack);
	// stack_arr_destroy(&graphics->matrix_stack);
	tico_render_terminate(&graphics->render);
	LOG("Graphics Module terminated");
}

TIC_API void INTERNAL_API(tico_graphics_begin, tc_Graphics* graphics) {
  int width, height;
  tico_window_get_size(&width, &height);

  tc_Canvas canvas = {0};
  canvas.id = 0;
  canvas.width = width;
  canvas.height = height;

  tico_render_begin(&graphics->render);
  glEnable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);

  glScissor(0, 0, width, height);

  glViewport(0, 0, width, height);
  tico_shader_attach(tico_graphics_default_shader());
  tico_canvas_attach(canvas);
  tico_graphics_push();
  tico_graphics_origin();

  tico_graphics_clear(BG);
}
TIC_API void INTERNAL_API(tico_graphics_end, tc_Graphics *graphics) {
  tico_graphics_pop();
  tico_canvas_detach();
  tico_shader_detach();

  tico_render_flush(&graphics->render);
  tico_render_draw(&graphics->render);
}

void tico_graphics_draw_internal(tc_Graphics *graphics, tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color) {
	tico_render_set_texture(&graphics->render, texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tico_render_reset_if_full(&graphics->render, 4);
  tico_render_add_rect(&graphics->render, dest, source, color);
}

void tico_graphics_draw_ex_internal(tc_Graphics *graphics, tc_Texture tex, tc_Rectf dest, tc_Rectf src, float angle, tc_Vec2 origin, tc_Color color) {
  tico_render_set_texture(&graphics->render, tex);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tico_render_reset_if_full(&graphics->render, 4);
  tc_Rectf fdest = tico_rectf(dest.x, dest.y, dest.w, dest.h);
  tico_render_add_rect_ex(&graphics->render, dest, src, angle, origin, color);
}

void tico_graphics_clear(tc_Color color) {
	tc_Rectf c;
	for (int i = 0; i < 4; i++) c.data[i] = (float)color.data[i] / 255.f;
	glClearColor(c.x, c.y, c.w, c.h);

	glClear(GL_COLOR_BUFFER_BIT);
}

void tico_graphics_push_internal(tc_Graphics *graphics) {
  // tico_batch_next_drawcall(&Core.render.batch, tc_false);
  tc_Matrix matrix = stack_arr_top(&graphics->matrix_stack);
  // tico_batch_set_transform(&Core.render.batch, matrix);
  tico_render_set_transform(&graphics->render, matrix);
  stack_arr_push(&graphics->matrix_stack, matrix);
  matrix = stack_arr_top(&graphics->matrix_stack);
}

void tico_graphics_pop_internal(tc_Graphics *graphics) {
  stack_arr_pop(&graphics->matrix_stack);
  tc_Matrix i = stack_arr_top(&graphics->matrix_stack);
  tico_render_set_transform(&graphics->render, i);
  // Core.render.state.camera = NULL;
}

void tico_graphics_origin_internal(tc_Graphics *graphics) {
  tc_Matrix matrix = stack_arr_top(&graphics->matrix_stack);
  tico_matrix_identity(&matrix);
  stack_arr_set(&graphics->matrix_stack, matrix);
  tico_render_set_transform(&graphics->render, matrix);
}

void tico_graphics_translate_internal(tc_Graphics *graphics, float x, float y) {
  float yy = y + 0.1f;
  tc_Matrix matrix = stack_arr_top(&graphics->matrix_stack);
  tico_matrix_translate_in_place(&matrix, x, yy, 0);
  stack_arr_set(&graphics->matrix_stack, matrix);
  tico_render_set_transform(&graphics->render, matrix);
}

void tico_graphics_scale_internal(tc_Graphics *graphics, float x, float y) {
  tc_Matrix matrix = stack_arr_top(&graphics->matrix_stack);
  tico_matrix_scale_aniso(&matrix, matrix, x, y, 1);
  stack_arr_set(&graphics->matrix_stack, matrix);
  tico_render_set_transform(&graphics->render, matrix);
}

void tico_graphics_rotate_internal(tc_Graphics *graphics, float angle) {
  tc_Matrix matrix = stack_arr_top(&graphics->matrix_stack);
  tico_matrix_rotate_z(&matrix, matrix, angle);
  stack_arr_set(&graphics->matrix_stack, matrix);
  tico_render_set_transform(&graphics->render, matrix);
}



void tico_graphics_scissor_internal(tc_Graphics *graphics, int x, int y, int w, int h) {
  tico_render_set_clip(&graphics->render, tico_rect(x, y, w, h));
  // TRACELOG("%d %d", w, h);
}

void tico_graphics_send_internal(tc_Graphics *graphics, const char *name, void *data, TIC_SHADER_UNIFORM_ type) {
	tc_Shader shader = stack_arr_top(&graphics->shader_stack);
	tico_shader_send(shader, name, data, type);
}
void tico_graphics_send_count_internal(tc_Graphics *graphics, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type) {
	tc_Shader shader = stack_arr_top(&graphics->shader_stack);
	tico_shader_send_count(shader, name, count, data, type);
}

int tico_graphics_default_vertex_shader_internal(tc_Graphics *graphics) {
	return graphics->default_vertex;
}

int tico_graphics_default_fragment_shader_internal(tc_Graphics *graphics) {
	return graphics->default_fragment;
}

tc_Shader tico_graphics_default_shader_internal(tc_Graphics *graphics) {
	 return graphics->default_shader;
}

void tico_graphics_push_shader_internal(tc_Graphics *graphics, tc_Shader shader) {
	stack_arr_push(&graphics->shader_stack, shader);
  tico_render_draw_reset(&graphics->render);
  glUseProgram(shader.program);
  // Core.render.state.currentShader = shader;
  tico_shader_send_world(shader);
}
void tico_graphics_pop_shader_internal(tc_Graphics *graphics) {
	stack_arr_pop(&graphics->shader_stack);
  tc_Shader shader = stack_arr_top(&graphics->shader_stack);
  // tc_Shader shader = *sv;

  tico_render_draw_reset(&graphics->render);
  glUseProgram(shader.program);
  // Core.render.state.currentShader = shader;
  tico_shader_send_world(shader);
}
void tico_graphics_push_canvas_internal(tc_Graphics *graphics, tc_Canvas canvas) {
	stack_arr_push(&graphics->canvas_stack, canvas);
  // vec_push(&graphics->canvas_vec, canvas);

  if (!tico_render_is_empty(&graphics->render)) tico_render_draw_reset(&graphics->render);
  tico_render_set_clip(&graphics->render, tico_rect(0, 0, canvas.width, canvas.height));
  // TRACELOG("in: %d %d %d", canvas.id, canvas.width, canvas.height);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
  glViewport(0, 0, canvas.width, canvas.height);
  // tic_shader_send_world(Core.render.state.currentShader);
  tc_Shader top_shader = stack_arr_top(&graphics->shader_stack);
  tico_shader_send_world(top_shader);
  // Core.render.state.currentCanvas = canvas;
  // graphics->curr
}
void tico_graphics_pop_canvas_internal(tc_Graphics *graphics) {
	stack_arr_pop(&graphics->canvas_stack);
  // vec_pop(&graphics->canvas_vec);
  tc_Canvas canvas = stack_arr_top(&graphics->canvas_stack);
  // tc_Canvas canvas = vec_last(&graphics->canvas_vec);
  // TRACEERR("%d", graphics->canvas_stack.top);

  if (!tico_render_is_empty(&graphics->render)) tico_render_draw_reset(&graphics->render);
  tico_render_set_clip(&graphics->render, tico_rect(0, 0, canvas.width, canvas.height));
  // TRACELOG("out: %d %d %d", canvas.id, canvas.width, canvas.height);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
  glViewport(0, 0, canvas.width, canvas.height);
  tc_Shader shader = stack_arr_top(&graphics->shader_stack);
  tico_shader_send_world(shader);
  // Core.render.state.currentCanvas = canvas;
}
tc_Canvas INTERNAL_API(tico_graphics_top_canvas, tc_Graphics *graphics) {
  ASSERT(graphics != NULL);
  tc_Canvas canvas = stack_arr_top(&graphics->canvas_stack);

  return canvas;
}

void tico_graphics_push_matrix_internal(tc_Graphics *graphics, tc_Matrix matrix) {}
void tico_graphics_pop_matrix_internal(tc_Graphics *graphics) {}

// void tico_graphics_clear(tc_Color color) {
//   glClearColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
//   glClear(GL_COLOR_BUFFER_BIT);
// }

/**********************
 * Shapes
 **********************/

void tico_graphics_draw_rectangle_internal(tc_Graphics *graphics, float x, float y, int width, int height, tc_Color color) {
  // tico_render_set_texture(&graphics->render, graphics->default_texture);
  // tico_render_set_draw_mode(&graphics->render, TIC_LINES);
  // tico_render_reset_if_full(&graphics->render, 8);
  // tico_render_add_rect(&graphics->render, tico_rectf(x, y, width, height), tico_rectf(0, 0, 1, 1), color);

  tico_graphics_draw_line_internal(graphics, x, y, x+width, y, color);
  tico_graphics_draw_line_internal(graphics, x+width, y, x+width, y+height, color);
  tico_graphics_draw_line_internal(graphics, x+width, y+height, x, y+height, color);
  tico_graphics_draw_line_internal(graphics, x, y+height, x, y, color);

  // tico_batch_add_line_rect(&Core.render.batch, tico_rectf(x, y, width, height), tico_rectf(0, 0, 1, 1), color);
}

void tico_graphics_fill_rectangle_internal(tc_Graphics *graphics, float x, float y, float width, float height, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tico_render_reset_if_full(&graphics->render, 4);
  tico_render_add_rect(&graphics->render, tico_rectf(x, y, width, height), tico_rectf(0, 0, 1, 1), color);
}
// void tico_graphics_draw_rect(tc_Rect rect, tc_Color color) {
//   tico_graphics_draw_rectangle(rect.x, rect.y, rect.w, rect.h, color);
// }

// void tico_graphics_fill_rect(tc_Rect rect, tc_Color color) {
//   tico_graphics_fill_rectangle(rect.x, rect.y, rect.w, rect.h, color);
// }


// equation to calc points: shorturl.at/bwxE8
void tico_graphics_draw_circle_internal(tc_Graphics *graphics, int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points += points%2;
//   TRACELOG("%f %f %d", radius, eq, points);
  tico_graphics_draw_circle_ex_internal(graphics, x, y, radius, points, color);
}
void tico_graphics_fill_circle_internal(tc_Graphics *graphics, int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points *= points;
  tico_graphics_fill_circle_ex_internal(graphics, x, y, radius, points, color);
}
void tico_graphics_draw_circle_ex_internal(tc_Graphics *graphics, int x, int y, float radius, int sides, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_set_draw_mode(&graphics->render, TIC_LINES);
  tico_render_reset_if_full(&graphics->render, sides * 2);
  tico_render_add_circle(&graphics->render, (tc_Vec2){x, y}, radius, sides, color);
}

void tico_graphics_fill_circle_ex_internal(tc_Graphics *graphics, int x, int y, float radius, int sides, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tico_render_reset_if_full(&graphics->render, sides * 2);
  tico_render_add_circle(&graphics->render, (tc_Vec2){x, y}, radius, sides, color);
}

void tico_graphics_draw_triangle_internal(tc_Graphics *graphics, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_set_draw_mode(&graphics->render, TIC_LINES);
  tico_render_reset_if_full(&graphics->render, 8);
  tc_Vec2 p0 = tico_vec2(x0, y0);
  tc_Vec2 p1 = tico_vec2(x1, y1);
  tc_Vec2 p2 = tico_vec2(x2, y2);
  tico_render_add_triangle(&graphics->render, p0, p1, p2, color);
}
void tico_graphics_fill_triangle_internal(tc_Graphics *graphics, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tico_render_reset_if_full(&graphics->render, 8);
  tc_Vec2 p0 = tico_vec2(x0, y0);
  tc_Vec2 p1 = tico_vec2(x1, y1);
  tc_Vec2 p2 = tico_vec2(x2, y2);
  tico_render_add_triangle(&graphics->render, p0, p1, p2, color);
}
// void tico_graphics_draw_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
//   tico_graphics_draw_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
// }
// void tico_graphics_fill_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
//   tico_graphics_fill_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
// }

void tico_graphics_draw_line_internal(tc_Graphics *graphics, int x0, int y0, int x1, int y1, tc_Color color) {
  tico_render_set_texture(&graphics->render, graphics->default_texture);
  tico_render_reset_if_full(&graphics->render, 2);
  tico_render_add_line(&graphics->render, (tc_Vec2){x0, y0}, (tc_Vec2){x1, y1}, color);
}

/***********************
 * Text
 ***********************/

void tico_graphics_draw_text_internal(tc_Graphics *graphics, const char *text, int x, int y, tc_Color color) {
  tico_graphics_draw_text_scale_internal(graphics, text, x, y, 1, 1, color);
}

void tico_graphics_draw_text_width_internal(tc_Graphics *graphics, const char *text, int x, int y, int width, tc_Color color) {
  tico_graphics_draw_text_font_scale_width_internal(graphics, graphics->default_font, text, x, y, width, 1, 1, color);
}

void tico_graphics_draw_text_scale_internal(tc_Graphics *graphics, const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tico_graphics_draw_text_font_scale_internal(graphics, graphics->default_font, text, x, y, sx, sy, color);
}
void tico_graphics_draw_text_ex_internal(tc_Graphics *graphics, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

void tico_graphics_draw_text_font_internal(tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, tc_Color color) {
  tico_graphics_draw_text_font_scale_internal(graphics, font, text, x, y, 1, 1, color);
}

void tico_graphics_draw_text_font_scale_internal(tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tico_render_set_texture(&graphics->render, font.texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  int len = strlen(text);
  tico_render_reset_if_full(&graphics->render, 4 * len);
  tc_Texture tex = font.texture;
  unsigned char *p = (unsigned char*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tico_utf8_codepoint(p, &codepoint);
    tico_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, 0);
    tico_render_add_rect(&graphics->render, tico_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

void tico_graphics_draw_text_font_ex_internal(tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

void tico_graphics_draw_text_font_scale_width_internal(tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color) {
  tico_render_set_texture(&graphics->render, font.texture);
  tico_render_set_draw_mode(&graphics->render, TIC_TRIANGLES);
  tc_Texture tex = font.texture;
  unsigned char *p = (unsigned char*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tico_utf8_codepoint(p, &codepoint);
    tico_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, width);
    tico_render_reset_if_full(&graphics->render, 4);
    tico_render_add_rect(&graphics->render, tico_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

/*========================*
 *         Types          *
 *========================*/

/*********************
 * Texture
 *********************/

tc_Texture tico_texture_create(void *data, int width, int height, int mode) {
  tc_Texture texData = {0};
  glGenTextures(1, &texData.id);
  glBindTexture(GL_TEXTURE_2D, texData.id);
  texData.width = width;
  texData.height = height;
  texData.filter[0] = GL_NEAREST;
  texData.filter[1] = GL_NEAREST;
  texData.wrap[0] = GL_CLAMP_TO_BORDER;
  texData.wrap[1] = GL_CLAMP_TO_BORDER;
  // tex.refs = 0;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texData.filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texData.filter[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texData.wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texData.wrap[1]);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, GL_FALSE, mode, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  // tc_Texture tex = tico_resources_add_texture(data, tc_Texture *texture)

  return texData;
}

tc_Texture tico_texture_load(const char *filename) {

 size_t size;
 tc_Texture tex = {0};
 tex.id = 0;
 // TRACELOG("%s", filename);
 unsigned char* buffer = tico_filesystem_read_file((unsigned char*)filename, &size);
 // unsigned char *buffer = "uopa";


 // tex = malloc(sizeof(tc_Texture));
 tex = tico_texture_from_memory(buffer, size);
 // tex->refs = 1;
//  tex.refs = 0;
 // tc_Texture tex = tico_texture_from_memory(buffer, size);
 if (tex.id == 0) {
  TRACELOG("Failed to load texture '%s'", filename);
  // free(tex);
  return (tc_Texture){0};
 }
 // tico_resources_add_texture(filename, tex);

 free(buffer);

 return tex;
}


tc_Texture tico_texture_from_memory(void *data, int bufSize) {
  tc_Texture tex = {0};
  int width, height, channels;

  unsigned char *imgData = stbi_load_from_memory(data, bufSize, &width, &height, &channels, 0);

  if (!imgData) return tex;

  GLenum format;
  switch (channels) {
    case 1: format = GL_RED; break;
    case 2: format = GL_RG; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
  }

  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  tex.width = width;
  tex.height = height;
  tex.filter[0] = GL_NEAREST;
  tex.filter[1] = GL_NEAREST;
  tex.wrap[0] = GL_CLAMP_TO_BORDER;
  tex.wrap[1] = GL_CLAMP_TO_BORDER;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex.filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex.filter[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.wrap[1]);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, GL_FALSE, format, GL_UNSIGNED_BYTE, imgData);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(imgData);

  return tex;
}

void tico_texture_set_filter(tc_Texture *tex, int filter_min, int filter_mag) {
  tex->filter[0] = filter_min;
  tex->filter[1] = filter_mag;

  glBindTexture(GL_TEXTURE_2D, tex->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->filter[1]);
  
  glBindTexture(GL_TEXTURE_2D, 0);
}

void tico_texture_get_filter(tc_Texture tex, int *filter_min, int *filter_mag) {
  if (filter_min) *filter_min = tex.filter[0];
  if (filter_mag) *filter_mag = tex.filter[1];
}

void tico_texture_set_wrap(tc_Texture *tex, int wrap_min, int wrap_mag) {
  tex->wrap[0] = wrap_min;
  tex->wrap[1] = wrap_mag;

  glBindTexture(GL_TEXTURE_2D, tex->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->wrap[1]);
  
  glBindTexture(GL_TEXTURE_2D, 0);
}

void tico_texture_get_wrap(tc_Texture tex, int *wrap_min, int *wrap_mag) {
  if (wrap_min) *wrap_min = tex.wrap[0];
  if (wrap_mag) *wrap_mag = tex.wrap[1];
}

void tico_texture_destroy(tc_Texture *tex) {
  glDeleteTextures(1, &tex->id);
}

void tico_texture_draw(tc_Texture tex, tc_Rectf dest, tc_Rectf src, tc_Color color) {
  tico_graphics_draw(tex, dest, src, color);
}

void tico_texture_draw_ex(tc_Texture tex, tc_Rectf dest, tc_Rectf src, float angle, tc_Vec2 origin, tc_Color color) {
  tico_graphics_draw_ex(tex, dest, src, angle, origin, color);
}

/*********************
 * Image
 *********************/

tc_Image tico_image_create(void *data, int width, int height, int mode) {
  tc_Image image = {0};
  image.texture = tico_texture_create(data, width, height, mode);
  if (image.texture.id == 0) return image;

  image.width = width;
  image.height = height;

  return image;
}
tc_Image tico_image_load(const char *filename) {
  tc_Image image = {0};
//   tc_Image *image = tico_resources_get_image(filename);
//   if (image) return *image;

//   image = malloc(sizeof(*image));
  image.texture = tico_texture_load(filename);
  if (image.texture.id == 0) {
//     free(image);
//     tc_Image img = {0};
    return image;
  }
  image.width = image.texture.width;
  image.height = image.texture.height;
//   tico_resources_add_image(filename, image);
  return image;
}
tc_Image tico_image_from_memory(void *data, int bufSize) {
  tc_Image image = {0};
  image.texture = tico_texture_from_memory(data, bufSize);
  if (image.texture.id == 0) {
    TRACEERR("Failed to load image");
    return image;
  }
  image.width = image.texture.width;
  image.height = image.texture.height;

  return image;
}
void tico_image_destroy(tc_Image *image) {
  tico_texture_destroy(&image->texture);
}

void tico_image_draw(tc_Image image, float x, float y, tc_Color color) {
  tico_image_draw_scale(image, x, y, 1, 1, color);
}
void tico_image_draw_scale(tc_Image image, float x, float y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tico_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tico_rectf(0, 0, image.width, image.height);
  tico_texture_draw(image.texture, dest, src, color);
}
void tico_image_draw_ex(tc_Image image, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  tc_Rectf dest = tico_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tico_rectf(0, 0, image.width, image.height);
  tico_texture_draw_ex(image.texture, dest, src, angle, tico_vec2(cx, cy), color);
}
void tico_image_draw_part(tc_Image image, tc_Rectf rect, float x, float y, tc_Color color) {
  tico_image_draw_part_scale(image, rect, x, y, 1, 1, color);
}
void tico_image_draw_part_scale(tc_Image image, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tico_rectf(x, y, rect.w*sx, rect.h*sy);
//   tc_Rectf src = tc_rectf(0, 0, image.width, image.height);
  tico_texture_draw(image.texture, dest, rect, color);
}
void tico_image_draw_part_ex(tc_Image image, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  tc_Rectf dest = tico_rectf(x, y, rect.w*sx, rect.h*sy);
  tico_texture_draw_ex(image.texture, dest, rect, angle, (tc_Vec2){cx, cy}, color);
}

// void tico_graphics_draw_text_font_ex(tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

/*********************
 * Shader
 *********************/

// #include "shaders/gba.frag.h"
// #include "shaders/outline.frag.h"

void tico_shader_init_shaders() {
  // Core.render.state.fragmentShaders[TIC_GBA_FRAG_SHADER] = tico_shader_compile(gba_frag, GL_FRAGMENT_SHADER);
  // Core.render.state.fragmentShaders[TIC_OUTLINE_FRAG_SHADER] = tico_shader_compile(outline_frag, GL_FRAGMENT_SHADER);
}

tc_Shader tico_shader_create(int vertexShader, int fragmentShader) {
  tc_Shader shader = {0};
  // unsigned int vertexShader = tico_shader_compile(vertexSource, GL_VERTEX_SHADER);
  // unsigned int fragmentShader = tico_shader_compile(fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tico_shader_load_program(vertexShader, fragmentShader);

  return shader;
}

tc_Shader tico_shader_create_from_string(const char *vertexSource, const char *fragmentSource) {
  tc_Shader shader = {0};
  unsigned int vertexShader = tico_shader_compile(vertexSource, GL_VERTEX_SHADER);
  unsigned int fragmentShader = tico_shader_compile(fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tico_shader_load_program(vertexShader, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shader;
}

tc_Shader tico_shader_create_effect(const char * vertEffect, const char * fragEffect) {
    const unsigned char *vertexSource = (const unsigned char*)"layout (location = 0) in vec2 in_Pos;\n"
                             "layout (location = 1) in vec4 in_Color;\n"
                             "layout (location = 2) in vec2 in_Texcoord;\n"
                             "varying vec4 v_Color;\n"
                             "varying vec2 v_Texcoord;\n"
                             "uniform mat4 world;\n"
                             "uniform mat4 modelview;\n"
                             "%s\n"
                             "void main()\n"
                             "{\n"
                             "  gl_Position = position(vec4(in_Pos, 0.0, 1.0), world, modelview);\n"
                             "  //gl_Position = vec4(in_Pos, 0.0, 1.0);\n"
                             "  //gl_Position = vec4(in_Pos, 0.0, 1.0);\n"
                             "  v_Color = in_Color;\n"
                             "  v_Texcoord = in_Texcoord;\n"
                             "}\n";
    const unsigned char *fragmentSource = (const unsigned char*)"out vec4 FragColor;\n"
                             "varying vec4 v_Color;\n"
                             "varying vec2 v_Texcoord;\n"
                             "uniform sampler2D gm_BaseTexture;\n"
                             "%s\n"
                             "void main()\n"
                             "{\n"
                             "  FragColor = effect(v_Color, gm_BaseTexture, v_Texcoord);\n"
                             "  //FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
                             "}\n";


    int usingDefaultVert = tc_true;
    int usingDefaultFrag = tc_true;

    int vertShader = tico_graphics_default_vertex_shader();
    if (vertEffect) {
      size_t len = strlen((const char *)vertexSource) + strlen(vertEffect);
      char vertSource[len];
      sprintf(vertSource, (const char *)vertexSource, vertEffect);

      vertShader = tico_shader_compile(vertSource, GL_VERTEX_SHADER);
      usingDefaultVert = tc_false;
    }

    // int fragShader = Core.render.state.fragmentShaders[TIC_DEFAULT_FRAGMENT];
    int fragShader = tico_graphics_default_fragment_shader();
    if (fragEffect) {
      size_t len = strlen(fragmentSource) + strlen(fragEffect);
      char fragSource[len];
      sprintf(fragSource, fragmentSource, fragEffect);
      fragShader = tico_shader_compile(fragSource, GL_FRAGMENT_SHADER);
      usingDefaultFrag = tc_false;
    }
    

    tc_Shader shader = {0};
    shader.program = tico_shader_load_program(vertShader, fragShader);

    if (!usingDefaultVert) glDeleteShader(vertShader);
    if (!usingDefaultFrag) glDeleteShader(fragShader);

    return shader;
}

// void tico_shader_new_gba(tc_Shader *shader) {

//   int vertexShader = Core.render.state.vertexShaders[TIC_DEFAULT_VERTEX];
//   int fragmentShader = Core.render.state.fragmentShaders[TIC_GBA_FRAG_SHADER];

//   shader->program = tico_shader_load_program(vertexShader, fragmentShader);
// }

// void tico_shader_new_outline(tc_Shader *shader) {
//   int vertexShader = Core.render.state.vertexShaders[TIC_DEFAULT_VERTEX];
//   int fragmentShader = Core.render.state.fragmentShaders[TIC_OUTLINE_FRAG_SHADER];

//   shader->program = tico_shader_load_program(vertexShader, fragmentShader);
// }


int tico_shader_compile(const char *source, int type) {
  unsigned int shader = glCreateShader(type);

  const unsigned char *shaderType = (unsigned char*)(type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
  char shaderDefine[128];
  sprintf(shaderDefine, "#version 330\n#define %s_SHADER\n", shaderType);

  GLchar const* files[] =  {shaderDefine, source};
  GLint lengths[] =  {strlen(shaderDefine), strlen(source)};


  glShaderSource(shader, 2, files, lengths);
  
  glCompileShader(shader);
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    TRACEERR("%s::Compilation failed: %s", shaderType, infoLog);
  }

  return shader;
}

int tico_shader_load_program(int vertexShader, int fragmentShader) {
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
    TRACEERR("Linking failed: %s", infoLog);
  }

  return shaderProgram;
}

void tico_shader_attach(tc_Shader shader) {
  // tico_batch_draw_reset(&Core.render.batch);
  // glUseProgram(shader.program);
  // Core.render.state.currentShader = shader;
  // tico_shader_send_world(shader);
  tico_graphics_push_shader(shader);
}

void tico_shader_detach(void) {
  // tico_batch_draw_reset(&Core.render.batch);
  // glUseProgram(Core.render.state.defaultShader.program);
  // Core.render.state.currentShader = Core.render.state.defaultShader;
  // tico_shader_send_world(Core.render.state.currentShader);
  // tico_render_pop_shader();
  tico_graphics_pop_shader();
}

tc_Shader tico_shader_load_default(int *vertexShader, int *fragmentShader) {
  const unsigned char *vertexSource = (const unsigned char*)"layout (location = 0) in vec2 in_Pos;\n"
                             "layout (location = 1) in vec4 in_Color;\n"
                             "layout (location = 2) in vec2 in_Texcoord;\n"
                             "varying vec4 v_Color;\n"
                             "varying vec2 v_Texcoord;\n"
                             "uniform mat4 world;\n"
                             "uniform mat4 modelview;\n"
                             "void main()\n"
                             "{\n"
                             "  gl_Position = world * modelview *  vec4(in_Pos.x,in_Pos.y , 0.0, 1.0);\n"
                             "  //gl_Position = vec4(in_Pos, 0.0, 1.0);\n"
                             "  v_Color = in_Color;\n"
                             "  v_Texcoord = in_Texcoord;\n"
                             "}\n";

  const unsigned char *fragmentSource = (const unsigned char*)"out vec4 FragColor;\n"
                               "varying vec4 v_Color;\n"
                               "varying vec2 v_Texcoord;\n"
                               "uniform sampler2D gm_BaseTexture;\n"
                               "void main()\n"
                               "{\n"
                               "  FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
                               "}\n";

  tc_Shader shader = {0};
  *vertexShader = tico_shader_compile((const char*)vertexSource, GL_VERTEX_SHADER);
  *fragmentShader = tico_shader_compile((const char*)fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tico_shader_load_program(*vertexShader, *fragmentShader);
  TRACELOG("Default shader created");
  return shader;
}

void tico_shader_send_world(tc_Shader shader) {
  tc_Matrix world;
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  tico_matrix_ortho(&world, 0, view[2], view[3], 0, 0, 1);

  tico_shader_send(shader, "world", world.data, TIC_UNIFORM_MATRIX);
}

void tico_shader_send(tc_Shader shader, const char *name, void *value, TIC_SHADER_UNIFORM_ type) {
  tico_shader_send_count(shader, name, 1, value, type);
}

void tico_shader_send_count(tc_Shader shader, const char *name, int count, void *value, TIC_SHADER_UNIFORM_ type) {
  GLuint uniform = glGetUniformLocation(shader.program, (const GLchar*)name);
  float *val = (float *)value;
  switch (type)
  {
  case TIC_UNIFORM_FLOAT:
    glUniform1fv(uniform, count, val);
    break;
  case TIC_UNIFORM_VEC2:
    glUniform2fv(uniform, count, val);
    break;
  case TIC_UNIFORM_VEC3:
    glUniform3fv(uniform, count, val);
    break;
  case TIC_UNIFORM_VEC4:
    glUniform4fv(uniform, count, val);
    break;
  case TIC_UNIFORM_MATRIX:
    glUniformMatrix4fv(uniform, count, GL_FALSE, val);
    break;
  default:
    printf("ok\n");
  }
}

void tico_shader_destroy(tc_Shader *shader) {
  // TRACELOG("%d", shader->program);
  glDeleteProgram(shader->program);
}

/**********************
 * Canvas
 **********************/

tc_Canvas tico_canvas_create(int width, int height) {
  tc_Canvas canvas = {0};
  ASSERT(width > 0);
  ASSERT(height > 0);

  canvas.width = width;
  canvas.height = height;
  glGenFramebuffers(1, &canvas.id);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);

  canvas.tex = tico_texture_create(NULL, width, height, GL_RGBA);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas.tex.id, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return canvas;
}

void tico_canvas_destroy(tc_Canvas *canvas) {
  tico_texture_destroy(&canvas->tex);
  glDeleteFramebuffers(1, &canvas->id);
}

void tico_canvas_attach(tc_Canvas canvas) {
//   Core.render.state.currentCanvas = canvas;
  // if (!tico_batch_is_empty(Core.render.batch)) tico_batch_draw_reset(&Core.render.batch);
  // glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
  // glViewport(0, 0, canvas.width, canvas.height);
  // tico_shader_send_world(Core.render.state.currentShader);
  // Core.render.state.currentCanvas = canvas;
  // tico_render_push_canvas(canvas);
  tico_graphics_push_canvas(canvas);
}
void tico_canvas_detach(void) {
//   if (!tico_batch_is_empty(Core.render.batch)) tico_batch_draw_reset(&Core.render.batch);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
// //   Core.render.state.backToDefaultCanvas();
//   glViewport(0, 0, Core.window.width, Core.window.height);
//   tico_shader_send_world(Core.render.state.currentShader);
//   Core.render.state.currentCanvas.id = 0;
  tico_graphics_pop_canvas();
}

void tico_canvas_disable(void) {
//   if (!tico_batch_is_empty(Core.render.batch)) tico_batch_draw_reset(&Core.render.batch);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
// //   Core.render.state.backToDefaultCanvas();
//   glViewport(0, 0, Core.window.width, Core.window.height);
//   tico_shader_send_world(Core.render.state.currentShader);
//   Core.render.state.currentCanvas.id = 0;
}

void tico_canvas_draw(tc_Canvas canvas, float x, float y, tc_Color color) {
  tico_canvas_draw_scale(canvas, x, y, 1, 1, color);
}

void tico_canvas_draw_scale(tc_Canvas canvas, float x, float y, float sx, float sy, tc_Color color) {
  // tc_Rectf d = tico_rectf(x, y + canvas.texture.height*sy, canvas.texture.width*sx, canvas.texture.height*-sy);
  // tc_Rectf s = tico_rectf(0, 0, canvas.texture.width, canvas.texture.height);
  // tico_texture_draw(canvas.texture, d, s, color);
  tc_Rectf part = tico_rectf(0, 0, canvas.width, canvas.height);
  tico_canvas_draw_part_scale(canvas, part, x, y, sx, sy, color);
}

void tico_canvas_draw_ex(tc_Canvas canvas, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  // tc_Rectf d = tico_rectf(x, y, canvas.texture.width*sx, canvas.texture.height*-sy);
  // tc_Rectf s = tico_rectf(0, 0, canvas.texture.width, canvas.texture.height);
  // tico_texture_draw_ex(canvas.texture, d, s, angle, sx, sy, cx, cy, color);
  tc_Rectf part = tico_rectf(0, 0, canvas.width, canvas.height);
  tico_canvas_draw_part_ex(canvas, part, x, y, angle, sx, sy, cx, cy, color);
}

void tico_canvas_draw_part(tc_Canvas canvas, tc_Rectf rect, float x, float y, tc_Color color) {
  tico_canvas_draw_part_scale(canvas, rect, x, y, 1, 1, color);
}
void tico_canvas_draw_part_scale(tc_Canvas canvas, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color) {
  // tc_Rectf dest = tico_rectf(x, y + (rect.h*sy), rect.w*sx, rect.h*-sy);
  tc_Rectf dest = tico_rectf(x, y, rect.w*sx, rect.h*sy);
  // TRACELOG("%f %f %f %f", dest.x, dest.y, dest.w, dest.h);
  tc_Rectf part = tico_rectf(rect.x, canvas.height - rect.y, rect.w, -rect.h);
  // TRACELOG("%f %f %f %f", part.x, part.y, part.w, part.h);
  // tc_Rectf part = tico_rectf(rect.x, rect.y, rect.w, rect.h);
  // TRACELOG("%f %f", dest.x, dest.y);
  tico_texture_draw(canvas.tex, dest, part, color);
}
void tico_canvas_draw_part_ex(tc_Canvas canvas, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  int offy = (rect.h-cy)*sy;
  // tc_Rectf dest = tico_rectf(x, y, rect.w*sx, rect.h*-sy);
  tc_Rectf dest = tico_rectf(x, y, rect.w*sx, rect.h*sy);
  tc_Rectf part = tico_rectf(rect.x, canvas.height - rect.y, rect.w, -rect.h);
  // tico_texture_draw_ex(canvas.tex, dest, part, angle, sx, sy, cx, rect.h-cy, color);
  // dest.w *= sx;
  // dest.h *= sy;
  tico_texture_draw_ex(canvas.tex, dest, part, angle, (tc_Vec2){cx, cy}, color);
}

void tico_canvas_draw_auto(tc_Canvas canvas) {
  int width = tico_window_get_width();
  int height = tico_window_get_height();
  float ratio = tico_min(width/(float)canvas.width, height/(float)canvas.height);
  int wCenterX = width/2;
  int wCenterY = height/2;
  int cCenterX = canvas.width/2;
  int cCenterY = canvas.height/2;

  tico_canvas_draw_ex(canvas, wCenterX, wCenterY-(ratio/2), 0, ratio, ratio, cCenterX, cCenterY, WHITE);
}


/***************
 * Font
 ***************/

#include "font_data.h"

tc_Font tico_font_load_default(void) {
  tc_Font font = tico_font_load_from_memory(font_data[0].data, font_data[0].size, 16);
  return font;
}

void tico_font_init(tc_Font *font, const void *data, size_t bufSize, int fontSize) {
  font->data = malloc(bufSize);
  memcpy(font->data, data, bufSize);

  if (!stbtt_InitFont(&font->info, font->data, 0)) {
    TRACEERR("failed to init font");
    return;
  }

  int ascent, descent, lineGap;
  font->size = fontSize;
  float fsize = fontSize;
  font->scale = stbtt_ScaleForMappingEmToPixels(&font->info, fsize);
  stbtt_GetFontVMetrics(&font->info, (int*)&ascent, (int*)&descent, (int*)&lineGap);
  font->baseline = ascent * font->scale;

  int tw = 0;
  int th = 0;

  for (int i = 0; i < MAXFONTCHAR; i++) {
    int ax;
    int bl;
    int x0, y0, x1, y1;
    int w;
    int h;

    stbtt_GetCodepointHMetrics(&font->info, i, &ax, &bl);
    stbtt_GetCodepointBitmapBox(&font->info, i, font->scale, font->scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
    h = y1-y0;

    font->c[i].ax = ax * font->scale;
    font->c[i].ay = 0;
    font->c[i].bl = bl * font->scale;
    font->c[i].bw = w;
    font->c[i].bh = h;
    font->c[i].bt = font->baseline + y0;

    tw += w;
    th = tico_max(th, h);
  }

  font->texture = tico_texture_create(NULL, tw, th, GL_RGBA);
  // tico_texture_set_wrap(&font->texture, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

  glBindTexture(GL_TEXTURE_2D, font->texture.id);
  int x = 0;
  for (int i = 0; i < MAXFONTCHAR; i++) {
    int ww = font->c[i].bw;
    int hh = font->c[i].bh;
    int ssize = ww * hh * 4;
    int ox, oy;

    unsigned char *bitmap = stbtt_GetCodepointBitmap(&font->info, 0, font->scale, i, NULL, NULL, &ox, &oy);

    unsigned char pixels[ssize];
    for (int j = 0; j < ssize; j += 4) {
      int ii = j / 4;
      pixels[j] = 255;
      pixels[j + 1] = 255;
      pixels[j + 2] = 255;
      pixels[j + 3] = bitmap[ii];
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ww, hh, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    font->c[i].tx = (float)x / tw;

    x += font->c[i].bw;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

tc_Font tico_font_load(const char *filename, int size) {
  // tc_Font *font = tico_resources_get_font(filename);
  // if (font) {
  //   return *font;
  // }
  size_t bufSize;
  unsigned char *buffer = tico_filesystem_read_file(filename, &bufSize);
  // unsigned char *buffer = "uopaaa";
  // font = malloc(sizeof(tc_Font));
  tc_Font font = tico_font_load_from_memory(buffer, bufSize, size);
  if (font.size == 0) {
    // free(font);
    TRACEERR("Failed to load font '%s'", filename);
    return (tc_Font){0};
  }
  // tico_resources_add_font(filename, font);
  free(buffer);

  // TRACELOG("%s", filename);
  return font;
}

tc_Font tico_font_load_internal(const char *filename, unsigned char fontSize) {
  size_t size;
  unsigned char *buffer = tico_filesystem_read_internal_file(filename, &size);
  // unsigned char *buffer = filename;

  return tico_font_load_from_memory(buffer, size, fontSize);
}
tc_Font tico_font_load_external(const char *filename, unsigned char fontSize) {
  size_t size;
  unsigned char *buffer = tico_filesystem_read_external_file(filename, &size);
  // unsigned char *buffer = filename;


  return tico_font_load_from_memory(buffer, size, fontSize);
}

tc_Font tico_font_load_from_memory(const void *data, size_t bufSize, int fontSize) {
  tc_Font font = {0};

  tico_font_init(&font, data, bufSize, fontSize);
  return font;
}

void tico_font_destroy(tc_Font *font) {
  tico_texture_destroy(&font->texture);
  free(font->data);
}

void tico_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width) {
  tico_font_get_rect_scale(font, c, x, y, outPos, rect, 1, 1, width);
}

void tico_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width) {
  // const char *p = c;
  if (c == '\n') {
    *x = 0;
    *y += font.texture.height * sy;
    return;
  }
  if (c == '\t') {
    *x += font.c[c].bw*2;
    return;
  }
  if (width != 0 && *x + (font.c[c].bl*sx) > width) {
    *x = 0;
    *y += font.texture.height * sy;
  }

  float x2 = *x + (font.c[c].bl*sx);
  float y2 = *y + (font.c[c].bt*sy);
  float w = font.c[c].bw;
  float h = font.c[c].bh;

  float s0 = font.c[c].tx * font.texture.width;
  float t0 = 0;
  int s1 = font.c[c].bw;
  int t1 = font.c[c].bh;

  *x += font.c[c].ax * sx;
  *y += font.c[c].ay * sy;
  // if (!w || !h)
  //  continue;
  if (outPos) {
    outPos->x = x2;
    outPos->y = y2;
  }
  if (rect) *rect = (tc_Rectf){s0, t0, s1, t1};

  return;
}

int tico_font_get_text_width(tc_Font font, const char *text, int len) {
  unsigned char *p = (unsigned char*)text;
  int width = 0;
  int max_width = 0;
  while (*p && len--) {
    if (*p != '\n') {
      int codepoint;
      p = tico_utf8_codepoint(p, &codepoint);
      width += font.c[codepoint].ax;
    } else {
      max_width = tico_max(width, max_width);
      width = 0;
      p++;
    }
  }
  max_width = tico_max(width, max_width);

  return max_width;
}

int tico_font_get_text_height(tc_Font font, const char *text, int len) {
  unsigned char *p = (unsigned char*)text;
  int height = font.texture.height;
  while (*p) {
    if (*p == '\n') height += font.texture.height;
  }

  return height;
}
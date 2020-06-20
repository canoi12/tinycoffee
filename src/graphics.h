#ifndef TICO_GRAPHICS_H
#define TICO_GRAPHICS_H

#include "tinycoffee.h"
#include "external/stb_image.h"

typedef enum {
  TC_UNIFORM_INT,
  TC_UNIFORM_VEC2I,
  TC_UNIFORM_VEC3I,
  TC_UNIFORM_FLOAT,
  TC_UNIFORM_VEC2,
  TC_UNIFORM_VEC3,
  TC_UNIFORM_MATRIX
} TC_SHADER_UNIFORM_;

typedef struct tc_Font tc_Font;

typedef struct {
  unsigned int id;
  int width;
  int height;
  int filter[2];
  int wrap[2];
} tc_Texture;

typedef struct {
  unsigned int id;
  tc_Texture texture;
  int width;
  int height;
} tc_Canvas;

typedef struct {
  tc_Texture tex;
  int frameWidth;
  int frameHeight;
} tc_Sprite;

typedef struct {
  tc_Texture texture;
  void *data;
  int width;
  int height;
} tc_Image;

typedef struct {
  unsigned int id;
} tc_Shader;


/*********************
 * Graphics
 *********************/

TCDEF void tc_graphics_push();
TCDEF void tc_graphics_translate(int x, int y);
TCDEF void tc_graphics_scale(float x, float y);
TCDEF void tc_graphics_rotate(float angle);
TCDEF void tc_graphics_pop();
TCDEF void tc_scissor(int x, int y, int width, int height);

/**********************
 * Shapes
 **********************/

TCDEF void tc_draw_rectangle(int x, int y, int width, int height, tc_Color color);
TCDEF void tc_fill_rectangle(int x, int y, int width, int height, tc_Color color);
TCDEF void tc_draw_rect(tc_Rect rect, tc_Color color);
TCDEF void tc_fill_rect(tc_Rect rect, tc_Color color);

TCDEF void tc_draw_circle(int x, int y, float radius, tc_Color color);
TCDEF void tc_fill_circle(int x, int y, float radius, tc_Color color);
TCDEF void tc_draw_circle_ex(int x, int y, float radius, int sides, tc_Color color);
TCDEF void tc_fill_circle_ex(int x, int y, float radius, int sides, tc_Color color);

TCDEF void tc_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TCDEF void tc_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TCDEF void tc_draw_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color);
TCDEF void tc_fill_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color);

TCDEF void tc_draw_line(int x0, int y0, int x1, int y1, tc_Color color);

/*********************
 * Texture
 *********************/

TCDEF tc_Texture tc_create_texture(void *data, int width, int height);
TCDEF tc_Texture tc_load_texture(const char *filename);
TCDEF tc_Texture tc_texture_from_memory(void *data, int bufSize);
TCDEF void tc_destroy_texture(tc_Texture *texture);

TCDEF void tc_texture_set_filter(int min, int mag);
TCDEF void tc_texture_get_filter(int *min, int *mag);

TCDEF void tc_texture_set_filter(int s, int t);
TCDEF void tc_texture_get_filter(int *s, int *t);


TCDEF void tc_draw_texture(tc_Texture tex, tc_Rectf dest, tc_Rectf source, tc_Color color);
TCDEF void tc_draw_texture_ex(tc_Texture tex, tc_Rectf dest, tc_Rectf source, float angle, float sx, float sy, int cx, int cy, tc_Color color);
// TCDEF void tc_draw_texture_part(tc_Texture tex, tc_Rectf rect, int x, int y, tc_Color color);
// TCDEF void tc_draw_texture_part_ex(tc_Texture tex, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

/*********************
 * Image
 *********************/

TCDEF tc_Image tc_create_image(void *data, int width, int height);
TCDEF tc_Image tc_load_image(const char *filename);
TCDEF tc_Image tc_image_from_memory(void *data, int bufSize);
TCDEF void tc_destroy_image(tc_Image *image);

TCDEF void tc_draw_image(tc_Image image, int x, int y, tc_Color color);
TCDEF void tc_draw_image_scale(tc_Image image, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_image_ex(tc_Image image, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);
TCDEF void tc_draw_image_part(tc_Image image, tc_Rectf rect, int x, int y, tc_Color color);
TCDEF void tc_draw_image_part_scale(tc_Image image, tc_Rectf rect, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_image_part_ex(tc_Image image, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

/***********************
 * Text
 ***********************/

TCDEF void tc_draw_text(const tc_uint8 *text, int x, int y, tc_Color color);
TCDEF void tc_draw_text_scale(const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_text_ex(const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_text_width(const tc_uint8 *text, int x, int y, int width, tc_Color color);

TCDEF void tc_draw_text_font(tc_Font font, const tc_uint8 *text, int x, int y, tc_Color color);
TCDEF void tc_draw_text_font_scale(tc_Font font, const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_text_font_ex(tc_Font font, const tc_uint8 *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);
TCDEF void tc_draw_text_font_scale_width(tc_Font font, const tc_uint8 *text, int x, int y, int width, float sx, float sy, tc_Color color);

/****************
 * Shader
 ****************/

TCDEF tc_Shader tc_create_shader(const tc_uint8 *vertexSource, const tc_uint8 *fragmentSource);
TCDEF void tc_destroy_shader(tc_Shader *shader);
TCDEF int tc_compile_shader(const tc_uint8 *source, int type);
TCDEF int tc_load_shader_program(int vertexShader, int fragmentShader);
TCDEF void tc_set_shader(tc_Shader shader);
TCDEF tc_Shader tc_load_default_shader(int *vertexShader, int *fragmentShader);
TCDEF void tc_shader_send_worldview(tc_Shader Shader);
TCDEF void tc_shader_send_uniform(tc_Shader shader, const tc_uint8 *name, void *value, TC_SHADER_UNIFORM_ type);

/**********************
 * Canvas
 **********************/

TCDEF tc_Canvas tc_create_canvas(int width, int height);
TCDEF void tc_destroy_canvas(tc_Canvas *canvas);

TCDEF void tc_attach_canvas(tc_Canvas canvas);
TCDEF void tc_detach_canvas(void);

TCDEF void tc_draw_canvas(tc_Canvas canvas, int x, int y, tc_Color color);
TCDEF void tc_draw_canvas_scale(tc_Canvas canvas, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_canvas_ex(tc_Canvas canvas, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);
TCDEF void tc_draw_canvas_part(tc_Canvas canvas, tc_Rectf rect, int x, int y, tc_Color color);
TCDEF void tc_draw_canvas_part_scale(tc_Canvas canvas, tc_Rectf rect, int x, int y, float sx, float sy, tc_Color color);
TCDEF void tc_draw_canvas_part_ex(tc_Canvas canvas, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TCDEF void tc_canvas_auto(tc_Canvas canvas);

#endif /* TICO_GRAPHICS_H */

#if defined(TICO_GRAPHICS_IMPLEMENTATION)

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

/*********************
 * Graphics
 *********************/

TCDEF void tc_graphics_push() {
  Core.render.state.camera = tc_batch_get_transform(&Core.render.batch);
}
TCDEF void tc_graphics_translate(int x, int y) {
  tc_matrix_translate_in_place(Core.render.state.camera, x, y, 0);
}

TCDEF void tc_graphics_scale(float x, float y) {
  tc_matrix_scale_aniso(Core.render.state.camera, *Core.render.state.camera, x, y, 1);
}

TCDEF void tc_graphics_rotate(float angle) {
  tc_matrix_rotate_z(Core.render.state.camera, *Core.render.state.camera, angle);
}

TCDEF void tc_graphics_pop() {
  tc_Matrix i;
  tc_matrix_identity(&i);
  tc_batch_set_transform(&Core.render.batch, i);
  Core.render.state.camera = NULL;
}

TCDEF void tc_scissor(int x, int y, int w, int h) {
  tc_batch_set_clip(&Core.render.batch, tc_rect(x, y, w, h));
}

/**********************
 * Shapes
 **********************/

TCDEF void tc_draw_rectangle(int x, int y, int width, int height, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_LINES);
  tc_batch_reset_if_full(&Core.render.batch, 8);
  tc_batch_add_rect(&Core.render.batch, tc_rectf(x, y, width, height), tc_rectf(0, 0, 1, 1), color);
}

TCDEF void tc_fill_rectangle(int x, int y, int width, int height, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_batch_reset_if_full(&Core.render.batch, 4);
  tc_batch_add_rect(&Core.render.batch, tc_rectf(x, y, width, height), tc_rectf(0, 0, 1, 1), color);
}
TCDEF void tc_draw_rect(tc_Rect rect, tc_Color color) {
  tc_draw_rectangle(rect.x, rect.y, rect.w, rect.h, color);
}

TCDEF void tc_fill_rect(tc_Rect rect, tc_Color color) {
  tc_fill_rectangle(rect.x, rect.y, rect.w, rect.h, color);
}


// equantion to cal points: shorturl.at/bwxE8
TCDEF void tc_draw_circle(int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points += points%2;
//   TRACELOG("%f %f %d", radius, eq, points);
  tc_draw_circle_ex(x, y, radius, points, color);
}
TCDEF void tc_fill_circle(int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points *= points;
  tc_fill_circle_ex(x, y, radius, points, color);
}
TCDEF void tc_draw_circle_ex(int x, int y, float radius, int sides, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_LINES);
  tc_batch_reset_if_full(&Core.render.batch, sides * 2);
  tc_batch_add_circle(&Core.render.batch, (tc_Vec2){x, y}, radius, sides, color);
}

TCDEF void tc_fill_circle_ex(int x, int y, float radius, int sides, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_batch_reset_if_full(&Core.render.batch, sides * 2);
  tc_batch_add_circle(&Core.render.batch, (tc_Vec2){x, y}, radius, sides, color);
}

TCDEF void tc_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_LINES);
  tc_batch_reset_if_full(&Core.render.batch, 8);
  tc_batch_add_triangle(&Core.render.batch, x0, y0, x1, y1, x2, y2, color);
}
TCDEF void tc_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_batch_reset_if_full(&Core.render.batch, 8);
  tc_batch_add_triangle(&Core.render.batch, x0, y0, x1, y1, x2, y2, color);
}
TCDEF void tc_draw_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tc_draw_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}
TCDEF void tc_fill_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tc_fill_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}

TCDEF void tc_draw_line(int x0, int y0, int x1, int y1, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tc_batch_reset_if_full(&Core.render.batch, 2);
  tc_batch_add_line(&Core.render.batch, x0, y0, x1, y1, color);
}

/*********************
 * Texture
 *********************/

TCDEF tc_Texture tc_create_texture(void *data, int width, int height) {
  tc_Texture tex = {0};
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  tex.width = width;
  tex.height = height;
  tex.filter[0] = GL_NEAREST;
  tex.filter[1] = GL_NEAREST;
  tex.wrap[0] = GL_REPEAT;
  tex.wrap[1] = GL_REPEAT;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex.filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex.filter[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.wrap[1]);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return tex;
}

TCDEF tc_Texture tc_load_texture(const char *filename) {

 size_t size;
 tc_uint8* buffer = tc_read_file(filename, &size);
 tc_Texture tex = tc_texture_from_memory(buffer, size);
 if (tex.id == 0) {
  TRACELOG("Failed to load texture '%s'", filename);
 }
 free(buffer);

 return tex;
}

TCDEF tc_Texture tc_texture_from_memory(void *data, int bufSize) {
  tc_Texture tex = {0};
  int width, height, channels;

  tc_uint8 *imgData = stbi_load_from_memory(data, bufSize, &width, &height, &channels, 0);

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
  tex.wrap[0] = GL_REPEAT;
  tex.wrap[1] = GL_REPEAT;

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

TCDEF void tc_destroy_texture(tc_Texture *tex) {
  glDeleteTextures(1, &tex->id);
}

TCDEF void tc_draw_texture(tc_Texture tex, tc_Rectf dest, tc_Rectf src, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, tex);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_batch_reset_if_full(&Core.render.batch, 4);
//   tc_batch_add_rect(&Core.render.batch, tc_rectf(x, y, tex.width, tex.height), tc_rectf(0, 0, tex.width, tex.height), color);
  tc_batch_add_rect(&Core.render.batch, dest, src, color);
}

TCDEF void tc_draw_texture_ex(tc_Texture tex, tc_Rectf dest, tc_Rectf src, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, tex);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_batch_reset_if_full(&Core.render.batch, 4);
  tc_Rectf fdest = tc_rectf(dest.x, dest.y, dest.w * sx, dest.h * sy);
//   TRACELOG("%f", fdest.w);
  tc_batch_add_rect_ex(&Core.render.batch, dest, src, angle, tc_vec2_new(cx, cy), color);
}

// TCDEF void tc_draw_texture_part(tc_Texture tex, tc_Rectf rect, int x, int y, tc_Color color) {
//   tc_batch_set_texture(&Core.render.batch, tex);
//   tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
//   tc_batch_reset_if_full(&Core.render.batch);
//   tc_batch_add_rect(&Core.render.batch, tc_rectf(x, y, rect.w, rect.h), rect, color);
// }

// TCDEF void tc_draw_texture_part_ex(tc_Texture tex, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
//   tc_batch_set_texture(&Core.render.batch, tex);
//   tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
//   tc_batch_reset_if_full(&Core.render.batch);
//   tc_batch_add_rect_ex(&Core.render.batch, tc_rectf(x, y, rect.w*sx, rect.h*sy), rect, angle, vec2_new(cx, cy), color);
// }

/*********************
 * Image
 *********************/

TCDEF tc_Image tc_create_image(void *data, int width, int height) {
  tc_Image image = {0};
  image.texture = tc_create_texture(data, width, height);
  if (image.texture.id == 0) return image;

  image.width = width;
  image.height = height;

  return image;
}
TCDEF tc_Image tc_load_image(const char *filename) {
  tc_Image image = {0};
  image.texture = tc_load_texture(filename);
  if (image.texture.id == 0) return image;
  image.width = image.texture.width;
  image.height = image.texture.height;
  return image;
}
TCDEF tc_Image tc_image_from_memory(void *data, int bufSize) {
  tc_Image image = {0};
  image.texture = tc_texture_from_memory(data, bufSize);
  if (image.texture.id == 0) {
    TRACEERR("Failed to load image");
    return image;
  }
  image.width = image.texture.width;
  image.height = image.texture.height;

  return image;
}
TCDEF void tc_destroy_image(tc_Image *image) {
  tc_destroy_texture(&image->texture);
}

TCDEF void tc_draw_image(tc_Image image, int x, int y, tc_Color color) {
  tc_draw_image_scale(image, x, y, 1, 1, color);
}
TCDEF void tc_draw_image_scale(tc_Image image, int x, int y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tc_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tc_rectf(0, 0, image.width, image.height);
  tc_draw_texture(image.texture, dest, src, color);
}
TCDEF void tc_draw_image_ex(tc_Image image, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  tc_Rectf dest = tc_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tc_rectf(0, 0, image.width, image.height);
  tc_draw_texture_ex(image.texture, dest, src, angle, sx, sy, cx, cy, color);
}
TCDEF void tc_draw_image_part(tc_Image image, tc_Rectf rect, int x, int y, tc_Color color) {
  tc_draw_image_part_scale(image, rect, x, y, 1, 1, color);
}
TCDEF void tc_draw_image_part_scale(tc_Image image, tc_Rectf rect, int x, int y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tc_rectf(x, y, rect.w*sx, rect.h*sy);
//   tc_Rectf src = tc_rectf(0, 0, image.width, image.height);
  tc_draw_texture(image.texture, dest, rect, color);
}
TCDEF void tc_draw_image_part_ex(tc_Image image, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  tc_Rectf dest = tc_rectf(x, y, rect.w*sx, rect.h*sy);
  tc_draw_texture_ex(image.texture, dest, rect, angle, sx, sy, cx, cy, color);
}

/***********************
 * Text
 ***********************/

TCDEF void tc_draw_text(const tc_uint8 *text, int x, int y, tc_Color color) {
  tc_draw_text_scale(text, x, y, 1, 1, color);
}

TCDEF void tc_draw_text_width(const tc_uint8 *text, int x, int y, int width, tc_Color color) {
  tc_draw_text_font_scale_width(Core.defaultFont, text, x, y, width, 1, 1, color);
}

TCDEF void tc_draw_text_scale(const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color) {
  tc_draw_text_font_scale(Core.defaultFont, text, x, y, sx, sy, color);
}
TCDEF void tc_draw_text_ex(const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color) {}

TCDEF void tc_draw_text_font(tc_Font font, const tc_uint8 *text, int x, int y, tc_Color color) {
  tc_draw_text_font_scale(font, text, x, y, 1, 1, color);
}

TCDEF void tc_draw_text_font_scale(tc_Font font, const tc_uint8 *text, int x, int y, float sx, float sy, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, font.texture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_Texture tex = font.texture;
  tc_uint8 *p = (tc_uint8*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tc_utf8_codepoint(p, &codepoint);
    tc_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, 0);
    tc_batch_reset_if_full(&Core.render.batch, 4);
    tc_batch_add_rect(&Core.render.batch, tc_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

TCDEF void tc_draw_text_font_scale_width(tc_Font font, const tc_uint8 *text, int x, int y, int width, float sx, float sy, tc_Color color) {
  tc_batch_set_texture(&Core.render.batch, font.texture);
  tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
  tc_Texture tex = font.texture;
  tc_uint8 *p = (tc_uint8*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tc_utf8_codepoint(p, &codepoint);
    tc_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, width);
    tc_batch_reset_if_full(&Core.render.batch, 4);
    tc_batch_add_rect(&Core.render.batch, tc_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

TCDEF void tc_draw_text_font_ex(tc_Font font, const tc_uint8 *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

/*********************
 * Shader
 *********************/

TCDEF tc_Shader tc_create_shader(const tc_uint8 *vertexSource, const tc_uint8 *fragmentSource) {
  tc_Shader shader = {0};
  unsigned int vertexShader = tc_compile_shader(vertexSource, GL_VERTEX_SHADER);
  unsigned int fragmentShader = tc_compile_shader(fragmentSource, GL_FRAGMENT_SHADER);

  shader.id = tc_load_shader_program(vertexShader, fragmentShader);

  return shader;
}

TCDEF int tc_compile_shader(const tc_uint8 *source, int type) {
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
    TRACEERR("%s::Compilation failed: %s", shaderType, infoLog);
  }

  return shader;
}

TCDEF int tc_load_shader_program(int vertexShader, int fragmentShader) {
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

TCDEF void tc_set_shader(tc_Shader shader) {
  // tc_reset_batch(&Core.render);
  glUseProgram(shader.id);
  Core.render.state.currentShader = shader;
  tc_shader_send_worldview(shader);
}

TCDEF void tc_unset_shader(void) {
  // tc_reset_batch(&Core.render);
  glUseProgram(Core.render.state.defaultShader.id);
  Core.render.state.currentShader = Core.render.state.defaultShader;
  tc_shader_send_worldview(Core.render.state.currentShader);
}

TCDEF tc_Shader tc_load_default_shader(int *vertexShader, int *fragmentShader) {
  const tc_uint8 *vertexSource = (const tc_uint8*)"#version 330\n"
                             "layout (location = 0) in vec2 in_Pos;\n"
                             "layout (location = 1) in vec4 in_Color;\n"
                             "layout (location = 2) in vec2 in_Texcoord;\n"
                             "varying vec4 v_Color;\n"
                             "varying vec2 v_Texcoord;\n"
                             "uniform mat4 world;\n"
                             "uniform mat4 modelview;\n"
                             "void main()\n"
                             "{\n"
                             "  gl_Position = world * modelview *  vec4(in_Pos, 0.0, 1.0);\n"
                             "  //gl_Position = vec4(in_Pos, 0.0, 1.0);\n"
                             "  v_Color = in_Color;\n"
                             "  v_Texcoord = in_Texcoord;\n"
                             "}\n";

  const tc_uint8 *fragmentSource = (const tc_uint8*)"#version 330\n"
                               "out vec4 FragColor;\n"
                               "varying vec4 v_Color;\n"
                               "varying vec2 v_Texcoord;\n"
                               "uniform sampler2D gm_BaseTexture;\n"
                               "void main()\n"
                               "{\n"
                               "  FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
                               "}\n";

  tc_Shader shader = {0};
  *vertexShader = tc_compile_shader(vertexSource, GL_VERTEX_SHADER);
  *fragmentShader = tc_compile_shader(fragmentSource, GL_FRAGMENT_SHADER);

  shader.id = tc_load_shader_program(*vertexShader, *fragmentShader);
  TRACELOG("Default shader created");
  return shader;
}

TCDEF void tc_shader_send_worldview(tc_Shader shader) {
  tc_Matrix world;
//   matrix mview;
//   matrix_identity(&mview);
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  tc_matrix_ortho(&world, 0, view[2], view[3], 0, 0, 1);

  tc_shader_send_uniform(shader, (tc_uint8*)"world", world.data, TC_UNIFORM_MATRIX);
//   tc_shader_send_uniform(shader, (tc_uint8*)"view", mview.data, TC_UNIFORM_MATRIX);
}

TCDEF void tc_shader_send_uniform(tc_Shader shader, const tc_uint8 *name, void *value, TC_SHADER_UNIFORM_ type) {
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

TCDEF void tc_destroy_shader(tc_Shader *shader) {
  glDeleteProgram(shader->id);
}

/**********************
 * Canvas
 **********************/

TCDEF tc_Canvas tc_create_canvas(int width, int height) {
  tc_Canvas canvas = {0};
  ASSERT(width > 0);
  ASSERT(height > 0);

  canvas.width = width;
  canvas.height = height;
  glGenFramebuffers(1, &canvas.id);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);

  canvas.texture = tc_create_texture(NULL, width, height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas.texture.id, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return canvas;
}
TCDEF void tc_destroy_canvas(tc_Canvas *canvas) {
  tc_destroy_texture(&canvas->texture);
  glDeleteFramebuffers(1, &canvas->id);
}

TCDEF void tc_attach_canvas(tc_Canvas canvas) {
//   Core.render.state.currentCanvas = canvas;
  if (!tc_batch_is_empty(Core.render.batch)) tc_batch_draw_reset(&Core.render.batch);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
  glViewport(0, 0, canvas.width, canvas.height);
  tc_shader_send_worldview(Core.render.state.currentShader);
  Core.render.state.currentCanvas = canvas;
}
TCDEF void tc_detach_canvas(void) {
  if (!tc_batch_is_empty(Core.render.batch)) tc_batch_draw_reset(&Core.render.batch);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, Core.window.width, Core.window.height);
  tc_shader_send_worldview(Core.render.state.currentShader);
  Core.render.state.currentCanvas.id = 0;
}

TCDEF void tc_draw_canvas(tc_Canvas canvas, int x, int y, tc_Color color) {
  tc_draw_canvas_scale(canvas, x, y, 1, 1, color);
}

TCDEF void tc_draw_canvas_scale(tc_Canvas canvas, int x, int y, float sx, float sy, tc_Color color) {
//   tc_batch_set_texture(&Core.render.batch, canvas.texture);
//   tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
//   tc_batch_reset_if_full(&Core.render.batch, 8);
  tc_Rectf d = tc_rectf(x, y + canvas.texture.height*sy, canvas.texture.width*sx, canvas.texture.height*-sy);
  tc_Rectf s = tc_rectf(0, 0, canvas.texture.width, canvas.texture.height);
//   tc_batch_add_rect(&Core.render.batch, d, s, color);
  tc_draw_texture(canvas.texture, d, s, color);
}

TCDEF void tc_draw_canvas_ex(tc_Canvas canvas, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
//   tc_batch_set_texture(&Core.render.batch, canvas.texture);
//   tc_batch_set_draw_mode(&Core.render.batch, TC_TRIANGLES);
//   tc_batch_reset_if_full(&Core.render.batch, 8);
  tc_Rectf d = tc_rectf(x, y, canvas.texture.width*sx, canvas.texture.height*-sy);
  tc_Rectf s = tc_rectf(0, 0, canvas.texture.width, canvas.texture.height);
//   tc_batch_add_rect(&Core.render.batch, d, s, color);
  tc_draw_texture_ex(canvas.texture, d, s, angle, sx, sy, cx, cy, color);
}

TCDEF void tc_draw_canvas_part(tc_Canvas canvas, tc_Rectf rect, int x, int y, tc_Color color) {
  tc_draw_canvas_part_scale(canvas, rect, x, y, 1, 1, color);
}
TCDEF void tc_draw_canvas_part_scale(tc_Canvas canvas, tc_Rectf rect, int x, int y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tc_rectf(x, y + (rect.h*sy), rect.w*sx, rect.h*-sy);
  tc_draw_texture(canvas.texture, dest, rect, color);
}
TCDEF void tc_draw_canvas_part_ex(tc_Canvas canvas, tc_Rectf rect, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {
  int offy = (rect.h-cy)*sy;
  tc_Rectf dest = tc_rectf(x, y, rect.w*sx, rect.h*-sy);
//   TRACELOG("%f", dest.y);
  tc_draw_texture_ex(canvas.texture, dest, rect, angle, sx, sy, cx, rect.h-cy, color);
}

TCDEF void tc_canvas_auto(tc_Canvas canvas) {
  float ratio = tc_min(Core.window.width/(float)canvas.width, Core.window.height/(float)canvas.height);
  int wCenterX = Core.window.width/2;
  int wCenterY = Core.window.height/2;
  int cCenterX = canvas.width/2;
  int cCenterY = canvas.height/2;

  tc_draw_canvas_ex(canvas, wCenterX, wCenterY, 0, ratio, ratio, cCenterX, cCenterY, WHITE);
}

#endif

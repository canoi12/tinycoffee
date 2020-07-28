#include "tico.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

/*********************
 * Graphics
 *********************/

void tic_graphics_push() {
  tic_batch_next_drawcall(&Core.render.batch, tc_false);
  Core.render.state.camera = tic_batch_get_transform(&Core.render.batch);
}
void tic_graphics_translate(float x, float y) {
  float yy = y + 0.1f;
  tic_matrix_translate_in_place(Core.render.state.camera, x, yy, 0);
}

void tic_graphics_scale(float x, float y) {
  tic_matrix_scale_aniso(Core.render.state.camera, *Core.render.state.camera, x, y, 1);
}

void tic_graphics_rotate(float angle) {
  tic_matrix_rotate_z(Core.render.state.camera, *Core.render.state.camera, angle);
}

void tic_graphics_pop() {
  tc_Matrix i;
  tic_matrix_identity(&i);
  tic_batch_set_transform(&Core.render.batch, i);
  Core.render.state.camera = NULL;
}

void tic_graphics_scissor(int x, int y, int w, int h) {
  tic_batch_set_clip(&Core.render.batch, tic_rect(x, y, w, h));
  // TRACELOG("%d %d", w, h);
}

void tic_graphics_clear(tc_Color color) {
  glClearColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);
  glClear(GL_COLOR_BUFFER_BIT);
}

/**********************
 * Shapes
 **********************/

void tic_graphics_draw_rectangle(float x, float y, int width, int height, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_LINES);
  tic_batch_reset_if_full(&Core.render.batch, 8);
  tic_batch_add_rect(&Core.render.batch, tic_rectf(x, y, width, height), tic_rectf(0, 0, 1, 1), color);
  // tic_batch_add_line_rect(&Core.render.batch, tic_rectf(x, y, width, height), tic_rectf(0, 0, 1, 1), color);
}

void tic_graphics_fill_rectangle(float x, float y, float width, float height, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tic_batch_reset_if_full(&Core.render.batch, 4);
  tic_batch_add_rect(&Core.render.batch, tic_rectf(x, y, width, height), tic_rectf(0, 0, 1, 1), color);
}
void tic_graphics_draw_rect(tc_Rect rect, tc_Color color) {
  tic_graphics_draw_rectangle(rect.x, rect.y, rect.w, rect.h, color);
}

void tic_graphics_fill_rect(tc_Rect rect, tc_Color color) {
  tic_graphics_fill_rectangle(rect.x, rect.y, rect.w, rect.h, color);
}


// equation to calc points: shorturl.at/bwxE8
void tic_graphics_draw_circle(int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points += points%2;
//   TRACELOG("%f %f %d", radius, eq, points);
  tic_graphics_draw_circle_ex(x, y, radius, points, color);
}
void tic_graphics_fill_circle(int x, int y, float radius, tc_Color color) {
  float err = 0.5f;
  float val = (1-err/radius)*(1-err/radius);
  float eq = rad2deg(acos(2 * val - 1));
  int points = ceil(360 / eq);
  points *= points;
  tic_graphics_fill_circle_ex(x, y, radius, points, color);
}
void tic_graphics_draw_circle_ex(int x, int y, float radius, int sides, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_LINES);
  tic_batch_reset_if_full(&Core.render.batch, sides * 2);
  tic_batch_add_circle(&Core.render.batch, x, y, radius, sides, color);
}

void tic_graphics_fill_circle_ex(int x, int y, float radius, int sides, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tic_batch_reset_if_full(&Core.render.batch, sides * 2);
  tic_batch_add_circle(&Core.render.batch, x, y, radius, sides, color);
}

void tic_graphics_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_LINES);
  tic_batch_reset_if_full(&Core.render.batch, 8);
  tic_batch_add_triangle(&Core.render.batch, x0, y0, x1, y1, x2, y2, color);
}
void tic_graphics_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tic_batch_reset_if_full(&Core.render.batch, 8);
  tic_batch_add_triangle(&Core.render.batch, x0, y0, x1, y1, x2, y2, color);
}
void tic_graphics_draw_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tic_graphics_draw_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}
void tic_graphics_fill_trianglev(tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tic_graphics_fill_triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}

void tic_graphics_draw_line(int x0, int y0, int x1, int y1, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, Core.render.state.shapeTexture);
  tic_batch_reset_if_full(&Core.render.batch, 2);
  tic_batch_add_line(&Core.render.batch, x0, y0, x1, y1, color);
}

/*********************
 * Texture
 *********************/

tc_Texture tic_texture_create(void *data, int width, int height, int mode) {
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

  // tc_Texture tex = tic_resources_add_texture(data, tc_Texture *texture)

  return texData;
}

tc_Texture tic_texture_create_named(const char *name, void *data, int width, int height, int mode) {
 // tc_Texture *tex = tic_resources_get_texture(name);
 // if (tex) {
 //   TRACELOG("Same texture for %s", name);
 //   return *tex;
 // }

 // tex = malloc(sizeof(tc_Texture));
 tc_Texture tex = tic_texture_create(data, width, height, mode);
 // tex->refs = 1;
//  tex.refs = 0;
 if (tex.id == 0) {
  TRACELOG("Failed to load texture '%s'", name);
  // free(tex);
  return (tc_Texture){0};
 }
 // tic_resources_add_texture(name, tex);

 return tex;
}

tc_Texture tic_texture_load(const char *filename) {

 size_t size;
 // tc_Texture *tex = tic_resources_get_texture(filename);
 // if (tex) {
 //   // TRACELOG("Same texture for %s", filename);
 //   return *tex;
 // }

 // tc_Texture *tex = NULL;
 tc_Texture tex = {0};
 tex.id = 0;
 tc_uint8* buffer = tic_filesystem_read_file((tc_uint8*)filename, &size);

 // tex = malloc(sizeof(tc_Texture));
 tex = tic_texture_from_memory(buffer, size);
 // tex->refs = 1;
//  tex.refs = 0;
 // tc_Texture tex = tic_texture_from_memory(buffer, size);
 if (tex.id == 0) {
  TRACELOG("Failed to load texture '%s'", filename);
  // free(tex);
  return (tc_Texture){0};
 }
 // tic_resources_add_texture(filename, tex);

 free(buffer);

 return tex;
}


tc_Texture tic_texture_from_memory(void *data, int bufSize) {
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

void tic_texture_set_filter(tc_Texture *tex, int filter_min, int filter_mag) {
  tex->filter[0] = filter_min;
  tex->filter[1] = filter_mag;

  glBindTexture(GL_TEXTURE_2D, tex->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex->filter[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex->filter[1]);
  
  glBindTexture(GL_TEXTURE_2D, 0);
}

void tic_texture_get_filter(tc_Texture tex, int *filter_min, int *filter_mag) {
  if (filter_min) *filter_min = tex.filter[0];
  if (filter_mag) *filter_mag = tex.filter[1];
}

void tic_texture_set_wrap(tc_Texture *tex, int wrap_min, int wrap_mag) {
  tex->wrap[0] = wrap_min;
  tex->wrap[1] = wrap_mag;

  glBindTexture(GL_TEXTURE_2D, tex->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->wrap[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->wrap[1]);
  
  glBindTexture(GL_TEXTURE_2D, 0);
}

void tic_texture_get_wrap(tc_Texture tex, int *wrap_min, int *wrap_mag) {
  if (wrap_min) *wrap_min = tex.wrap[0];
  if (wrap_mag) *wrap_mag = tex.wrap[1];
}

void tic_texture_destroy(tc_Texture *tex) {
  glDeleteTextures(1, &tex->id);
}

void tic_texture_draw(tc_Texture tex, tc_Rectf dest, tc_Rectf src, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, tex);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tic_batch_reset_if_full(&Core.render.batch, 4);
  tic_batch_add_rect(&Core.render.batch, dest, src, color);
}

void tic_texture_draw_ex(tc_Texture tex, tc_Rectf dest, tc_Rectf src, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, tex);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tic_batch_reset_if_full(&Core.render.batch, 4);
  tc_Rectf fdest = tic_rectf(dest.x, dest.y, dest.w * sx, dest.h * sy);
  tic_batch_add_rect_ex(&Core.render.batch, dest, src, angle, cx, cy, color);
}

/*********************
 * Image
 *********************/

tc_Image tic_image_create(void *data, int width, int height, int mode) {
  tc_Image image = {0};
  image.texture = tic_texture_create(data, width, height, mode);
  if (image.texture.id == 0) return image;

  image.width = width;
  image.height = height;

  return image;
}
tc_Image tic_image_load(const char *filename) {
  tc_Image image = {0};
//   tc_Image *image = tic_resources_get_image(filename);
//   if (image) return *image;

//   image = malloc(sizeof(*image));
  image.texture = tic_texture_load(filename);
  if (image.texture.id == 0) {
//     free(image);
//     tc_Image img = {0};
    return image;
  }
  image.width = image.texture.width;
  image.height = image.texture.height;
//   tic_resources_add_image(filename, image);
  return image;
}
tc_Image tic_image_from_memory(void *data, int bufSize) {
  tc_Image image = {0};
  image.texture = tic_texture_from_memory(data, bufSize);
  if (image.texture.id == 0) {
    TRACEERR("Failed to load image");
    return image;
  }
  image.width = image.texture.width;
  image.height = image.texture.height;

  return image;
}
void tic_image_destroy(tc_Image *image) {
  tic_texture_destroy(&image->texture);
}

void tic_image_draw(tc_Image image, float x, float y, tc_Color color) {
  tic_image_draw_scale(image, x, y, 1, 1, color);
}
void tic_image_draw_scale(tc_Image image, float x, float y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tic_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tic_rectf(0, 0, image.width, image.height);
  tic_texture_draw(image.texture, dest, src, color);
}
void tic_image_draw_ex(tc_Image image, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  tc_Rectf dest = tic_rectf(x, y, image.width*sx, image.height*sy);
  tc_Rectf src = tic_rectf(0, 0, image.width, image.height);
  tic_texture_draw_ex(image.texture, dest, src, angle, sx, sy, cx, cy, color);
}
void tic_image_draw_part(tc_Image image, tc_Rectf rect, float x, float y, tc_Color color) {
  tic_image_draw_part_scale(image, rect, x, y, 1, 1, color);
}
void tic_image_draw_part_scale(tc_Image image, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color) {
  tc_Rectf dest = tic_rectf(x, y, rect.w*sx, rect.h*sy);
//   tc_Rectf src = tc_rectf(0, 0, image.width, image.height);
  tic_texture_draw(image.texture, dest, rect, color);
}
void tic_image_draw_part_ex(tc_Image image, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  tc_Rectf dest = tic_rectf(x, y, rect.w*sx, rect.h*sy);
  tic_texture_draw_ex(image.texture, dest, rect, angle, sx, sy, cx, cy, color);
}

/***********************
 * Text
 ***********************/

void tic_graphics_draw_text(const char *text, int x, int y, tc_Color color) {
  tic_graphics_draw_text_scale(text, x, y, 1, 1, color);
}

void tic_graphics_draw_text_width(const char *text, int x, int y, int width, tc_Color color) {
  tic_graphics_draw_text_font_scale_width(Core.defaultFont, text, x, y, width, 1, 1, color);
}

void tic_graphics_draw_text_scale(const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tic_graphics_draw_text_font_scale(Core.defaultFont, text, x, y, sx, sy, color);
}
void tic_graphics_draw_text_ex(const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

void tic_graphics_draw_text_font(tc_Font font, const char *text, int x, int y, tc_Color color) {
  tic_graphics_draw_text_font_scale(font, text, x, y, 1, 1, color);
}

void tic_graphics_draw_text_font_scale(tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, font.texture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  int len = strlen(text);
  tic_batch_reset_if_full(&Core.render.batch, 4 * len);
  tc_Texture tex = font.texture;
  tc_uint8 *p = (tc_uint8*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tic_utf8_codepoint(p, &codepoint);
    tic_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, 0);
    tic_batch_add_rect(&Core.render.batch, tic_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

void tic_graphics_draw_text_font_scale_width(tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color) {
  tic_batch_set_texture(&Core.render.batch, font.texture);
  tic_batch_set_draw_mode(&Core.render.batch, TIC_TRIANGLES);
  tc_Texture tex = font.texture;
  tc_uint8 *p = (tc_uint8*)text;
  float x0 = 0;
  float y0 = 0;
  while (*p) {
    tc_Vec2 pos;
    tc_Rectf rect;
    int codepoint;
    p = tic_utf8_codepoint(p, &codepoint);
    tic_font_get_rect_scale(font, codepoint, &x0, &y0, &pos, &rect, sx, sy, width);
    tic_batch_reset_if_full(&Core.render.batch, 4);
    tic_batch_add_rect(&Core.render.batch, tic_rectf(x + pos.x, y + pos.y, rect.w*sx, rect.h*sy), rect, color);
  }
}

// void tic_graphics_draw_text_font_ex(tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color) {}

/*********************
 * Shader
 *********************/

#include "shaders/gba.frag.h"
#include "shaders/outline.frag.h"

void tic_shader_init_shaders() {
  Core.render.state.fragmentShaders[TIC_GBA_FRAG_SHADER] = tic_shader_compile(gba_frag, GL_FRAGMENT_SHADER);
  Core.render.state.fragmentShaders[TIC_OUTLINE_FRAG_SHADER] = tic_shader_compile(outline_frag, GL_FRAGMENT_SHADER);
}

tc_Shader tic_shader_create(int vertexShader, int fragmentShader) {
  tc_Shader shader = {0};
  // unsigned int vertexShader = tic_shader_compile(vertexSource, GL_VERTEX_SHADER);
  // unsigned int fragmentShader = tic_shader_compile(fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tic_shader_load_program(vertexShader, fragmentShader);

  return shader;
}

tc_Shader tic_shader_create_from_string(const char *vertexSource, const char *fragmentSource) {
  tc_Shader shader = {0};
  unsigned int vertexShader = tic_shader_compile(vertexSource, GL_VERTEX_SHADER);
  unsigned int fragmentShader = tic_shader_compile(fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tic_shader_load_program(vertexShader, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shader;
}

tc_Shader tic_shader_create_effect(const char * vertEffect, const char * fragEffect) {
    const tc_uint8 *vertexSource = (const tc_uint8*)"layout (location = 0) in vec2 in_Pos;\n"
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
    const tc_uint8 *fragmentSource = (const tc_uint8*)"out vec4 FragColor;\n"
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

    int vertShader = Core.render.state.vertexShaders[TIC_DEFAULT_VERTEX];
    if (vertEffect) {
      size_t len = strlen(vertexSource) + strlen(vertEffect);
      char vertSource[len];
      sprintf(vertSource, vertexSource, vertEffect);

      vertShader = tic_shader_compile(vertSource, GL_VERTEX_SHADER);
      usingDefaultVert = tc_false;
    }

    int fragShader = Core.render.state.fragmentShaders[TIC_DEFAULT_FRAGMENT];
    if (fragEffect) {
      size_t len = strlen(fragmentSource) + strlen(fragEffect);
      char fragSource[len];
      sprintf(fragSource, fragmentSource, fragEffect);
      fragShader = tic_shader_compile(fragSource, GL_FRAGMENT_SHADER);
      usingDefaultFrag = tc_false;
    }
    

    tc_Shader shader = {0};
    shader.program = tic_shader_load_program(vertShader, fragShader);

    if (!usingDefaultVert) glDeleteShader(vertShader);
    if (!usingDefaultFrag) glDeleteShader(fragShader);

    return shader;
}

void tic_shader_new_gba(tc_Shader *shader) {

  int vertexShader = Core.render.state.vertexShaders[TIC_DEFAULT_VERTEX];
  int fragmentShader = Core.render.state.fragmentShaders[TIC_GBA_FRAG_SHADER];

  shader->program = tic_shader_load_program(vertexShader, fragmentShader);
}

void tic_shader_new_outline(tc_Shader *shader) {
  int vertexShader = Core.render.state.vertexShaders[TIC_DEFAULT_VERTEX];
  int fragmentShader = Core.render.state.fragmentShaders[TIC_OUTLINE_FRAG_SHADER];

  shader->program = tic_shader_load_program(vertexShader, fragmentShader);
}


int tic_shader_compile(const char *source, int type) {
  unsigned int shader = glCreateShader(type);

  const tc_uint8 *shaderType = (tc_uint8*)(type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
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

int tic_shader_load_program(int vertexShader, int fragmentShader) {
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

void tic_shader_attach(tc_Shader shader) {
  // tic_batch_draw_reset(&Core.render.batch);
  // glUseProgram(shader.program);
  // Core.render.state.currentShader = shader;
  // tic_shader_send_world(shader);
  tic_render_push_shader(shader);
}

void tic_shader_detach(void) {
  // tic_batch_draw_reset(&Core.render.batch);
  // glUseProgram(Core.render.state.defaultShader.program);
  // Core.render.state.currentShader = Core.render.state.defaultShader;
  // tic_shader_send_world(Core.render.state.currentShader);
  tic_render_pop_shader();
}

tc_Shader tic_shader_load_default(int *vertexShader, int *fragmentShader) {
  const tc_uint8 *vertexSource = (const tc_uint8*)"layout (location = 0) in vec2 in_Pos;\n"
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

  const tc_uint8 *fragmentSource = (const tc_uint8*)"out vec4 FragColor;\n"
                               "varying vec4 v_Color;\n"
                               "varying vec2 v_Texcoord;\n"
                               "uniform sampler2D gm_BaseTexture;\n"
                               "void main()\n"
                               "{\n"
                               "  FragColor = v_Color * texture2D(gm_BaseTexture, v_Texcoord);\n"
                               "}\n";

  tc_Shader shader = {0};
  *vertexShader = tic_shader_compile(vertexSource, GL_VERTEX_SHADER);
  *fragmentShader = tic_shader_compile(fragmentSource, GL_FRAGMENT_SHADER);

  shader.program = tic_shader_load_program(*vertexShader, *fragmentShader);
  TRACELOG("Default shader created");
  return shader;
}

void tic_shader_send_world(tc_Shader shader) {
  tc_Matrix world;
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  tic_matrix_ortho(&world, 0, view[2], view[3], 0, 0, 1);

  tic_shader_send(shader, "world", world.data, TIC_UNIFORM_MATRIX);
}

void tic_shader_send(tc_Shader shader, const char *name, void *value, TIC_SHADER_UNIFORM_ type) {
  tic_shader_send_count(shader, name, 1, value, type);
}

void tic_shader_send_count(tc_Shader shader, const char *name, int count, void *value, TIC_SHADER_UNIFORM_ type) {
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

void tic_shader_destroy(tc_Shader *shader) {
  glDeleteProgram(shader->program);
}

/**********************
 * Canvas
 **********************/

tc_Canvas tic_canvas_create(int width, int height) {
  tc_Canvas canvas = {0};
  ASSERT(width > 0);
  ASSERT(height > 0);

  canvas.width = width;
  canvas.height = height;
  glGenFramebuffers(1, &canvas.id);
  glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);

  canvas.texture = tic_texture_create(NULL, width, height, GL_RGBA);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas.texture.id, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return canvas;
}

void tic_canvas_destroy(tc_Canvas *canvas) {
  tic_texture_destroy(&canvas->texture);
  glDeleteFramebuffers(1, &canvas->id);
}

void tic_canvas_attach(tc_Canvas canvas) {
//   Core.render.state.currentCanvas = canvas;
  // if (!tic_batch_is_empty(Core.render.batch)) tic_batch_draw_reset(&Core.render.batch);
  // glBindFramebuffer(GL_FRAMEBUFFER, canvas.id);
  // glViewport(0, 0, canvas.width, canvas.height);
  // tic_shader_send_world(Core.render.state.currentShader);
  // Core.render.state.currentCanvas = canvas;
  tic_render_push_canvas(canvas);
}
void tic_canvas_detach(void) {
//   if (!tic_batch_is_empty(Core.render.batch)) tic_batch_draw_reset(&Core.render.batch);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
// //   Core.render.state.backToDefaultCanvas();
//   glViewport(0, 0, Core.window.width, Core.window.height);
//   tic_shader_send_world(Core.render.state.currentShader);
//   Core.render.state.currentCanvas.id = 0;
  tic_render_pop_canvas();
}

void tic_canvas_disable(void) {
//   if (!tic_batch_is_empty(Core.render.batch)) tic_batch_draw_reset(&Core.render.batch);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
// //   Core.render.state.backToDefaultCanvas();
//   glViewport(0, 0, Core.window.width, Core.window.height);
//   tic_shader_send_world(Core.render.state.currentShader);
//   Core.render.state.currentCanvas.id = 0;
}

void tic_canvas_draw(tc_Canvas canvas, float x, float y, tc_Color color) {
  tic_canvas_draw_scale(canvas, x, y, 1, 1, color);
}

void tic_canvas_draw_scale(tc_Canvas canvas, float x, float y, float sx, float sy, tc_Color color) {
  // tc_Rectf d = tic_rectf(x, y + canvas.texture.height*sy, canvas.texture.width*sx, canvas.texture.height*-sy);
  // tc_Rectf s = tic_rectf(0, 0, canvas.texture.width, canvas.texture.height);
  // tic_texture_draw(canvas.texture, d, s, color);
  tc_Rectf part = tic_rectf(0, 0, canvas.width, canvas.height);
  tic_canvas_draw_part(canvas, part, x, y, color);
}

void tic_canvas_draw_ex(tc_Canvas canvas, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  // tc_Rectf d = tic_rectf(x, y, canvas.texture.width*sx, canvas.texture.height*-sy);
  // tc_Rectf s = tic_rectf(0, 0, canvas.texture.width, canvas.texture.height);
  // tic_texture_draw_ex(canvas.texture, d, s, angle, sx, sy, cx, cy, color);
  tc_Rectf part = tic_rectf(0, 0, canvas.width, canvas.height);
  tic_canvas_draw_part_ex(canvas, part, x, y, angle, sx, sy, cx, cy, color);
}

void tic_canvas_draw_part(tc_Canvas canvas, tc_Rectf rect, float x, float y, tc_Color color) {
  tic_canvas_draw_part_scale(canvas, rect, x, y, 1, 1, color);
}
void tic_canvas_draw_part_scale(tc_Canvas canvas, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color) {
  // tc_Rectf dest = tic_rectf(x, y + (rect.h*sy), rect.w*sx, rect.h*-sy);
  tc_Rectf dest = tic_rectf(x, y, rect.w*sx, rect.h*sy);
  tc_Rectf part = tic_rectf(rect.x, rect.h, rect.w, rect.y-rect.h);
  // tc_Rectf part = tic_rectf(rect.x, rect.y, rect.w, rect.h);
  // TRACELOG("%f %f", dest.x, dest.y);
  tic_texture_draw(canvas.texture, dest, part, color);
}
void tic_canvas_draw_part_ex(tc_Canvas canvas, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  int offy = (rect.h-cy)*sy;
  // tc_Rectf dest = tic_rectf(x, y, rect.w*sx, rect.h*-sy);
  tc_Rectf dest = tic_rectf(x, y, rect.w*sx, rect.h*sy);
  tc_Rectf part = tic_rectf(rect.x, rect.h, rect.w, rect.y-rect.h);
  // tic_texture_draw_ex(canvas.texture, dest, part, angle, sx, sy, cx, rect.h-cy, color);
  tic_texture_draw_ex(canvas.texture, dest, part, angle, sx, sy, cx, cy, color);
}

void tic_canvas_draw_auto(tc_Canvas canvas) {
  float ratio = tic_min(Core.window.width/(float)canvas.width, Core.window.height/(float)canvas.height);
  int wCenterX = Core.window.width/2;
  int wCenterY = Core.window.height/2;
  int cCenterX = canvas.width/2;
  int cCenterY = canvas.height/2;

  tic_canvas_draw_ex(canvas, wCenterX, wCenterY-(ratio/2), 0, ratio, ratio, cCenterX, cCenterY, WHITE);
}

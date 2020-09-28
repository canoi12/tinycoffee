#ifndef TICO_GRAPHICS_H
#define TICO_GRAPHICS_H

#include "tico.h"

#include "stb/stb_truetype.h"
#include "stb/stb_truetype.h"


#define MATRIX_STACK_SIZE 32
#define SHADER_STACK_SIZE 32
#define CANVAS_STACK_SIZE 32

typedef stack_arr_t(tc_Canvas, CANVAS_STACK_SIZE) stack_canvas_t;
typedef stack_arr_t(tc_Shader, SHADER_STACK_SIZE) stack_shader_t;
typedef stack_arr_t(tc_Matrix, MATRIX_STACK_SIZE) stack_matrix_t;

typedef struct tc_Graphics {
  tc_Font default_font;
  stack_canvas_t canvas_stack;
  stack_shader_t shader_stack;
  stack_matrix_t matrix_stack;

  tc_Render render;
  tc_Texture default_texture;

  int default_vertex;
  int default_fragment;
  tc_Shader default_shader;
} tc_Graphics;

TIC_API int tico_graphics_init(tc_Graphics* graphics);
TIC_API void tico_graphics_terminate(tc_Graphics *graphics);

/****************
 * Texture
 ****************/

TIC_API tc_Texture tico_texture_create(void *data, int width, int height, int mode);
// TIC_API tc_Texture tico_texture_create_named(const char *name, void *data, int width, int height, int mode);
TIC_API tc_Texture tico_texture_load(const char *filename);
TIC_API tc_Texture tico_texture_from_memory(void *data, int bufSize);
TIC_API void tico_texture_destroy(tc_Texture *texture);


TIC_API int tico_texture_get_width(tc_Texture texture);
TIC_API int tico_texture_get_height(tc_Texture texture);
TIC_API void tico_texture_get_size(tc_Texture texture, int *width, int *height);

TIC_API void tico_texture_set_filter(tc_Texture *texture, int min, int mag);
TIC_API void tico_texture_set_wrap(tc_Texture *texture, int min, int mag);
TIC_API void tico_texture_get_filter(tc_Texture texture, int *min, int *mag);
TIC_API void tico_texture_get_wrap(tc_Texture texture, int *min, int *mag);

TIC_API void tico_texture_draw(tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tico_texture_draw_ex(tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color);


/****************
 * Image
 ****************/

TIC_API tc_Image tico_image_create(void *data, int width, int height, int mode);
TIC_API tc_Image tico_image_load(const char *image);
TIC_API tc_Image tico_image_from_memory(void *data, int bufSize);
TIC_API void tico_image_destroy(tc_Image *image);

TIC_API int tico_image_get_width(tc_Image image);
TIC_API int tico_image_get_height(tc_Image image);
TIC_API void tico_image_get_size(tc_Image image, int *width, int *height);

TIC_API void tico_image_set_filter(tc_Image *image, int min, int mag);
TIC_API void tico_image_get_filter(tc_Image image, int *min, int *mag);
TIC_API void tico_image_set_wrap(tc_Image *image, int min, int mag);
TIC_API void tico_image_get_wrap(tc_Image image, int *min, int *mag);

TIC_API void tico_image_draw(tc_Image image, float x, float y, tc_Color color);
TIC_API void tico_image_draw_scale(tc_Image image, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_image_draw_ex(tc_Image image, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tico_image_draw_part(tc_Image image, tc_Rectf part, float x, float y, tc_Color color);
TIC_API void tico_image_draw_part_scale(tc_Image image, tc_Rectf part, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_image_draw_part_ex(tc_Image image, tc_Rectf part, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);


/****************
 * Canvas
 ****************/

TIC_API tc_Canvas tico_canvas_create(int width, int height);
TIC_API void tico_canvas_destroy(tc_Canvas *canvas);

TIC_API int tico_canvas_get_width(tc_Canvas canvas);
TIC_API int tico_canvas_get_height(tc_Canvas canvas);
TIC_API void tico_canvas_get_size(tc_Canvas canvas, int *width, int *height);

TIC_API void tico_canvas_attach(tc_Canvas canvas);
TIC_API void tico_canvas_detach(void);
TIC_API void tico_canvas_disable(void);

TIC_API void tico_canvas_draw(tc_Canvas canvas, float x, float y, tc_Color color);
TIC_API void tico_canvas_draw_scale(tc_Canvas canvas, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_canvas_draw_ex(tc_Canvas canvas, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tico_canvas_draw_part(tc_Canvas canvas, tc_Rectf rect, float x, float y, tc_Color color);
TIC_API void tico_canvas_draw_part_scale(tc_Canvas canvas, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_canvas_draw_part_ex(tc_Canvas canvas, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tico_canvas_draw_auto(tc_Canvas canvas);

/****************
 * Shader
 ****************/

TIC_API tc_Shader tico_shader_create(int vertShader, int fragShader);
TIC_API tc_Shader tico_shader_create_from_string(const char *vertSource, const char *fragSource);
TIC_API tc_Shader tico_shader_create_from_ustring(const char *source);
TIC_API tc_Shader tico_shader_create_from_file(const char *filename);
TIC_API tc_Shader tico_shader_load_default(int *vertexShader, int *fragShader);
TIC_API tc_Shader tico_shader_create_effect(const char * vertEffect, const char * fragEffect);
TIC_API void tico_shader_destroy(tc_Shader *shader);

TIC_API int tico_shader_compile(const char *source, int shaderType);
TIC_API int tico_shader_load_program(int vertexShader, int fragShader);

TIC_API void tico_shader_send_world(tc_Shader shader);
TIC_API void tico_shader_send(tc_Shader shader, const char *name, void *data, TIC_SHADER_UNIFORM_ type);
TIC_API void tico_shader_send_count(tc_Shader shader, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type);

TIC_API void tico_shader_attach(tc_Shader shader);
TIC_API void tico_shader_detach(void);


/****************
 * Font
 ****************/

TIC_API tc_Font tico_font_load_default(void);
TIC_API void tico_font_init(tc_Font *font, const void *data, size_t bufSize, int fontSize);
TIC_API tc_Font tico_font_load(const char *filename, int size);
TIC_API tc_Font tico_font_load_from_memory(const void *data, size_t bufSize, int fontSize);
TIC_API tc_Font tico_font_load_from_texture(tc_Texture texture, int fontWidth, int fontHeight);
TIC_API void tico_font_destroy(tc_Font *font);

TIC_API void tico_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width);
TIC_API void tico_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width);

TIC_API int tico_font_get_text_width(tc_Font font, const char *text, int len);
TIC_API int tico_font_get_text_height(tc_Font font, const char *text, int len);

/*======================*
 *    Internal API      *
 *======================*/

TIC_API void INTERNAL_API(tico_graphics_begin, tc_Graphics* graphics);
TIC_API void INTERNAL_API(tico_graphics_end, tc_Graphics *graphics);

TIC_API void INTERNAL_API(tico_graphics_draw, tc_Graphics *graphics, tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void INTERNAL_API(tico_graphics_draw_ex, tc_Graphics *graphics, tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color);

TIC_API void INTERNAL_API(tico_graphics_push, tc_Graphics *graphics);
TIC_API void INTERNAL_API(tico_graphics_pop, tc_Graphics *graphics);

TIC_API void INTERNAL_API(tico_graphics_origin, tc_Graphics *graphics);
TIC_API void INTERNAL_API(tico_graphics_translate, tc_Graphics *graphics, float x, float y);
TIC_API void INTERNAL_API(tico_graphics_rotate, tc_Graphics *graphics, float angle);
TIC_API void INTERNAL_API(tico_graphics_scale, tc_Graphics *graphics, float x, float y);

TIC_API void INTERNAL_API(tico_graphics_scissor, tc_Graphics *graphics, int x, int y, int w, int h);

TIC_API void INTERNAL_API(tico_graphics_send, tc_Graphics *graphics, const char *name, void *data, TIC_SHADER_UNIFORM_ type);
TIC_API void INTERNAL_API(tico_graphics_send_count, tc_Graphics *graphics, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type);

TIC_API int INTERNAL_API(tico_graphics_default_vertex_shader, tc_Graphics *graphics);
TIC_API int INTERNAL_API(tico_graphics_default_fragment_shader, tc_Graphics *graphics);
TIC_API tc_Shader INTERNAL_API(tico_graphics_default_shader, tc_Graphics *graphics);

TIC_API void INTERNAL_API(tico_graphics_push_shader, tc_Graphics *graphics, tc_Shader shader);
TIC_API void INTERNAL_API(tico_graphics_pop_shader, tc_Graphics *graphics);

TIC_API void INTERNAL_API(tico_graphics_push_canvas, tc_Graphics *graphics, tc_Canvas canvas);
TIC_API void INTERNAL_API(tico_graphics_pop_canvas, tc_Graphics *graphics);
TIC_API tc_Canvas INTERNAL_API(tico_graphics_top_canvas, tc_Graphics *graphics);

TIC_API void INTERNAL_API(tico_graphics_push_matrix, tc_Graphics *graphics, tc_Matrix matrix);
TIC_API void INTERNAL_API(tico_graphics_pop_matrix, tc_Graphics *graphics);


/*******************
 * Shapes
 *******************/

TIC_API INTERNAL_API(void tico_graphics_draw_rectangle, tc_Graphics *graphics, float x, float y, int width, int height, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_fill_rectangle, tc_Graphics *graphics, float x, float y, float width, float height, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_rect, tc_Graphics *graphics, tc_Rect rect, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_fill_rect, tc_Graphics *graphics, tc_Rect rect, tc_Color color);

TIC_API INTERNAL_API(void tico_graphics_draw_circle, tc_Graphics *graphics, int x, int y, float radius, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_fill_circle, tc_Graphics *graphics, int x, int y, float radius, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_circle_ex, tc_Graphics *graphics, int x, int y, float radius, int sides, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_fill_circle_ex, tc_Graphics *graphics, int x, int y, float radius, int sides, tc_Color color);

TIC_API INTERNAL_API(void tico_graphics_draw_triangle, tc_Graphics *graphics, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_fill_triangle, tc_Graphics *graphics, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);

TIC_API INTERNAL_API(void tico_graphics_draw_line, tc_Graphics *graphics, int x0, int y0, int x1, int y1, tc_Color color);

/********************
 * Text
 ********************/

TIC_API INTERNAL_API(void tico_graphics_draw_text, tc_Graphics *graphics, const char *text, int x, int y, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_text_scale, tc_Graphics *graphics, const char *text, int x, int y, float sx, float sy, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_text_ex, tc_Graphics *graphics, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TIC_API INTERNAL_API(void tico_graphics_draw_text_font, tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_text_font_scale, tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color);
TIC_API INTERNAL_API(void tico_graphics_draw_text_font_ex, tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TIC_API INTERNAL_API(void tico_graphics_draw_text_font_scale_width, tc_Graphics *graphics, tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color);


#endif // TICO_GRAPHICS_H
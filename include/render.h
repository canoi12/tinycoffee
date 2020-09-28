#ifndef TICO_RENDER_H
#define TICO_RENDER_H

#include "tico.h"

#define DEFAULT_MAX_QUADS 10000
#define DEFAULT_MAX_VERTICES 4 * DEFAULT_MAX_QUADS
#define DEFAULT_MAX_INDICES 6 * DEFAULT_MAX_QUADS
#define MAX_DRAW_CALLS 128
#define MATRIX_STACK_SIZE 32

#define SHADER_STACK_SIZE 32
#define CANVAS_STACK_SIZE 32

typedef enum {
  TIC_TRIANGLES = 0,
  TIC_LINES
} TIC_DRAW_MODE_;

typedef enum {
  TIC_DEFAULT_FRAGMENT = 0,
  TIC_GBA_FRAG_SHADER,
  TIC_COLOR8_FRAG_SHADER,
  TIC_COLOR16_FRAG_SHADER,
  TIC_OUTLINE_FRAG_SHADER
} TIC_FRAGMENT_SHADERS_;

typedef enum {
  TIC_DEFAULT_VERTEX = 0
} TIC_VERTEX_SHADERS_;

typedef struct {
  union {
    float data[8];
    struct {
      float pos[2];
      float color[4];
      float uv[2];
    };
    struct {
      float x;
      float y;
      float r;
      float g;
      float b;
      float a;
      float s;
      float t;
    };
  };
} tc_Vertex;

typedef struct {
  unsigned int indexStart;
  unsigned int indexCount;
  unsigned int vertexCount;
  TIC_DRAW_MODE_ mode;

  unsigned int textureId;

  tc_Matrix modelview;

  tc_Rect clip;

} tc_DrawCall;

typedef struct {
  unsigned int *indices;
  tc_Vertex *vertices;
  tc_Vertex *verticesPtr;

  tc_DrawCall drawCalls[MAX_DRAW_CALLS];
  unsigned int drawCallIndex;

  unsigned int totalIndexCount;
  unsigned int totalVertexCount;

  unsigned int maxQuads;
  unsigned int maxVertices;
  unsigned int maxIndices;

  unsigned int vao;
  unsigned int vbo[2];

  tc_Rect currentClip;
  tc_Texture currentTexture;
} tc_Batch;

// typedef stack_arr_t(tc_Canvas, CANVAS_STACK_SIZE) stack_canvas_t;
// typedef stack_arr_t(tc_Shader, SHADER_STACK_SIZE) stack_shader_t;
// typedef stack_arr_t(tc_Matrix, MATRIX_STACK_SIZE) stack_matrix_t;

typedef struct tc_Render {
  tc_Batch batch;
  struct {
    tc_Shader defaultShader;
    tc_Shader currentShader;

    int canvasIndex;
    int shaderIndex;

    // stack_canvas_t canvasStack;
    // stack_shader_t shaderStack;
    // stack_matrix_t matrixStack;

    int vertexShaders[32];
    int fragmentShaders[32];

    int defaultVertexShader;
    int defaultFragmentShader;

    unsigned int defaultTextureId;
    unsigned int currentTextureId;
    tc_Texture shapeTexture;

    unsigned int drawCount;
    tc_Canvas currentCanvas;

    tc_Matrix modelview;
    tc_Canvas defaultCanvas;

    tc_Matrix *camera;

    // tc_Matrix matrixStack[MATRIX_STACK_SIZE];
    int currentMatrix;

    void (*backToDefaultCanvas)(void *);
  } state;
} tc_Render;

/*================
 * Render
 *================*/

TIC_API tc_bool tico_render_init(tc_Render *render);
TIC_API void tico_render_terminate(tc_Render *render);

TIC_API void tico_render_push_canvas(tc_Canvas canvas);
TIC_API void tico_render_pop_canvas(void);
TIC_API void tico_render_push_shader(tc_Shader shader);
TIC_API void tico_render_pop_shader(void);

TIC_API void tico_render_set_texture(tc_Render *render, tc_Texture texture);
TIC_API void tico_render_set_clip(tc_Render *render, tc_Rect rect);
TIC_API void tico_render_set_draw_mode(tc_Render *render, TIC_DRAW_MODE_ mode);
TIC_API void tico_render_set_transform(tc_Render *render, tc_Matrix matrix);

TIC_API void tico_render_align(tc_Render *render);
TIC_API void tico_render_update_count(tc_Render *render, int indexCount, int vertexCount);

TIC_API void tico_render_add_rect(tc_Render *render, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tico_render_add_rect_ex(tc_Render *render, tc_Rectf dest, tc_Rectf source, float angle, tc_Vec2 origin, tc_Color color);

TIC_API void tico_render_add_triangle(tc_Render *render, tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color);
TIC_API void tico_render_add_circle(tc_Render *render, tc_Vec2 p, float radius, int sides, tc_Color color);
TIC_API void tico_render_add_line(tc_Render *render, tc_Vec2 p0, tc_Vec2 p1, tc_Color color);

TIC_API void tico_render_begin(tc_Render *render);
TIC_API void tico_render_flush(tc_Render *render);
TIC_API void tico_render_draw(tc_Render *render);
TIC_API void tico_render_clear(tc_Render *render);

TIC_API int tico_render_is_empty(tc_Render *render);
TIC_API int tico_render_is_full(tc_Render *render, int vertices);

TIC_API void tico_render_draw_reset(tc_Render *render);
TIC_API void tico_render_reset_if_full(tc_Render *render, int vertex);

/**************
 * DrawCall
 **************/

TIC_API tc_DrawCall tico_drawcall_create(int start, int texture, TIC_DRAW_MODE_ mode, tc_Rect *clip, tc_Matrix modelview);
TIC_API void tico_drawcall_destroy(tc_DrawCall *call);

/***************
 * Batch
 ***************/

TIC_API tc_Batch tico_batch_create(tc_Texture texture, int maxQuads);
TIC_API void tico_batch_destroy(tc_Batch *batch);

TIC_API tc_Matrix* tico_batch_get_transform(tc_Batch *batch);

TIC_API tc_DrawCall* tico_batch_next_drawcall(tc_Batch *batch, tc_bool inv);

TIC_API void tico_batch_set_texture(tc_Batch *batch, tc_Texture texture);
TIC_API void tico_batch_set_clip(tc_Batch *batch, tc_Rect rect);
TIC_API void tico_batch_set_draw_mode(tc_Batch *batch, TIC_DRAW_MODE_ mode);
TIC_API void tico_batch_set_transform(tc_Batch *batch, tc_Matrix matrix);

TIC_API void tico_batch_align(tc_Batch *batch);
TIC_API void tico_batch_update_count(tc_Batch *batch, int indexCount, int vertexCount);

TIC_API void tico_batch_add_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tico_batch_add_line_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf src, tc_Color color);
TIC_API void tico_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, float angle, float cx, float cy, tc_Color color);

TIC_API void tico_batch_add_triangle(tc_Batch *batch, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TIC_API void tico_batch_add_circle(tc_Batch *batch, int x, int y, float radius, int sides, tc_Color color);
TIC_API void tico_batch_add_line(tc_Batch *batch, int x0, int y0, int x1, int y1, tc_Color color);

TIC_API void tico_batch_begin(tc_Batch *batch);
TIC_API void tico_batch_flush(tc_Batch *batch);
TIC_API void tico_batch_draw(tc_Batch *batch);
TIC_API void tico_batch_clear(tc_Batch *batch);

TIC_API tc_bool tico_batch_is_empty(tc_Batch batch);
TIC_API tc_bool tico_batch_is_full(tc_Batch batch, int vertices);
TIC_API void tico_batch_draw_reset(tc_Batch *batch);
TIC_API void tico_batch_reset_if_full(tc_Batch *batch, int vertex);


#endif // TICO_RENDER_H
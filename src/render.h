#ifndef TICO_RENDER_H
#define TICO_RENDER_H

#include "tinycoffee.h"
#include "graphics.h"

#define DEFAULT_MAX_QUADS 10000
#define DEFAULT_MAX_VERTICES 4 * DEFAULT_MAX_QUADS
#define DEFAULT_MAX_INDICES 6 * DEFAULT_MAX_QUADS
#define MAX_DRAW_CALLS 64

typedef enum {
  TC_TRIANGLES = 0,
  TC_LINES
} TC_DRAW_MODE;

enum {
  TC_POS_INDEX = 0,
  TC_COLOR_INDEX = 2,
  TC_TEXCOORD_INDEX = 6
};

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
  TC_DRAW_MODE mode;

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

typedef struct {
  tc_Batch batch;
  struct {
    tc_Shader defaultShader;
    tc_Shader currentShader;

    int defaultVertexShader;
    int defaultFragmentShader;

    unsigned int defaultTextureId;
    unsigned int currentTextureId;
    tc_Texture shapeTexture;

    unsigned int drawCount;
    tc_Canvas currentCanvas;

    tc_Matrix modelview;

    tc_Matrix *camera;
  } state;
} tc_Render;

TCDEF tc_Render tc_create_render();
TCDEF void rc_render_restore_modelview(tc_Render *render);
TCDEF void tc_destroy_render(tc_Render *render);

TCDEF tc_Batch tc_create_batch(tc_Texture tex, int maxQuads);
TCDEF void tc_destroy_batch(tc_Batch *batch);

TCDEF tc_DrawCall tc_create_drawcall(int start, int texture, TC_DRAW_MODE mode, tc_Rect *clip, tc_Matrix modelview);
TCDEF void tc_destroy_drawcall(tc_DrawCall *call);

TCDEF void tc_batch_set_texture(tc_Batch *batch, tc_Texture tex);
TCDEF void tc_batch_set_draw_mode(tc_Batch *batch, TC_DRAW_MODE mode);
TCDEF void tc_batch_set_clip(tc_Batch *batch, tc_Rect rect);
TCDEF void tc_batch_set_transform(tc_Batch *batch, tc_Matrix transform);
TCDEF void tc_batch_add_quad(tc_Batch *batch, int x, int y, int width, int height, tc_Color color);
TCDEF void tc_batch_add_rect(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, tc_Color color);
// TCDEF void tc_batch_add_rect_scale(tc_Batch *batch, tc_Rectf rect, float sx, float sy, tc_Color color);
TCDEF void tc_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, float angle, tc_Vec2 center, tc_Color color);
TCDEF void tc_batch_add_triangle(tc_Batch *batch, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TCDEF void tc_batch_add_trianglev(tc_Batch *batch, tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color);
TCDEF void tc_batch_add_circle(tc_Batch *batch, tc_Vec2 pos, float radius, int sides, tc_Color color);
TCDEF void tc_batch_add_line(tc_Batch *batch, int x0, int y0, int x1, int y1, tc_Color color);

TCDEF void tc_begin_batch(tc_Batch *batch);
TCDEF void tc_flush_batch(tc_Batch *batch);
TCDEF void tc_draw_batch(tc_Batch *batch);
TCDEF void tc_clear_batch(tc_Batch *batch);
TCDEF tc_Matrix* tc_batch_get_transform(tc_Batch *batch);
TCDEF tc_bool tc_batch_is_empty(tc_Batch batch);
TCDEF tc_bool tc_batch_is_full(tc_Batch batch, int vertices);
TCDEF void tc_batch_draw_reset(tc_Batch *batch);
TCDEF void tc_batch_reset_if_full(tc_Batch *batch, int vertices);

#endif

#if defined(TICO_RENDER_IMPLEMENTATION)

TCDEF tc_Render tc_create_render() {
  tc_Render render = {0};
  render.state.shapeTexture = tc_create_texture(WHITE.data, 1, 1);
  render.state.defaultTextureId = render.state.shapeTexture.id;
  render.state.currentTextureId = render.state.defaultTextureId;

  render.state.camera = NULL;
  tc_matrix_identity(&render.state.modelview);

  render.batch = tc_create_batch(render.state.shapeTexture, DEFAULT_MAX_QUADS);

  render.state.drawCount = 0;
  render.state.defaultShader = tc_load_default_shader(&render.state.defaultVertexShader, &render.state.defaultFragmentShader);
  render.state.currentShader = render.state.defaultShader;

  render.state.currentCanvas.id = 0;

  return render;
}

TCDEF void rc_render_restore_modelview(tc_Render *render) {
  tc_matrix_identity(&render->state.modelview);
}

TCDEF void tc_destroy_render(tc_Render *render) {
  tc_destroy_batch(&render->batch);
  tc_destroy_texture(&render->state.shapeTexture);
  glDeleteShader(render->state.defaultVertexShader);
  glDeleteShader(render->state.defaultFragmentShader);
  tc_destroy_shader(&render->state.defaultShader);
}

TCDEF tc_Batch tc_create_batch(tc_Texture tex, int maxQuads) {
  tc_Batch batch = {0};
  if (maxQuads <= 0) maxQuads = DEFAULT_MAX_QUADS;
  batch.maxQuads = maxQuads;
  batch.maxIndices = maxQuads * 6;
  batch.maxVertices = maxQuads * 4;
  TRACELOG("max: %d", batch.maxIndices);
  batch.totalIndexCount = 0;
  batch.totalVertexCount = 0;

  batch.vertices = TC_MALLOC(batch.maxVertices * sizeof(tc_Vertex));
  batch.indices = TC_MALLOC(batch.maxIndices * sizeof(int));

  batch.currentTexture = tex;

  int offset = 0;

  for (int i = 0; i < batch.maxIndices; i+=6) {
    batch.indices[i] = offset;
    batch.indices[i + 1] = offset + 1;
    batch.indices[i + 2] = offset + 2;

    batch.indices[i + 3] = offset;
    batch.indices[i + 4] = offset + 2;
    batch.indices[i + 5] = offset + 3;
    offset += 4;
  }

  tc_Matrix modelview;
  tc_matrix_identity(&modelview);
  batch.drawCalls[0] = tc_create_drawcall(0, tex.id, TC_TRIANGLES, NULL, modelview);
  batch.drawCallIndex = 0;

  glGenVertexArrays(1, &batch.vao);
  glGenBuffers(2, batch.vbo);

  glBindVertexArray(batch.vao);
  glBindBuffer(GL_ARRAY_BUFFER, batch.vbo[0]);

  glBufferData(GL_ARRAY_BUFFER, batch.maxVertices * sizeof(tc_Vertex), NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)TC_POS_INDEX);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)(sizeof(float) * TC_COLOR_INDEX));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)(sizeof(float) * TC_TEXCOORD_INDEX));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch.maxIndices * sizeof(int), batch.indices, GL_STATIC_DRAW);

  TRACELOG("Batch created");

  return batch;
}
TCDEF void tc_destroy_batch(tc_Batch *batch) {
  TC_FREE(batch->vertices);
  TC_FREE(batch->indices);
  glDeleteVertexArrays(1, &batch->vao);
  glDeleteBuffers(2, batch->vbo);
}

TCDEF tc_DrawCall tc_create_drawcall(int start, int texture, TC_DRAW_MODE mode, tc_Rect *clip, tc_Matrix modelview) {
  tc_DrawCall drawCall = {0};
  drawCall.indexStart = start;
  drawCall.textureId = texture;
  drawCall.mode = mode;
  if (clip) drawCall.clip = *clip;
  drawCall.indexCount = 0;
  drawCall.vertexCount = 0;
//   drawCall.modelview = modelview;
  tc_matrix_clone(&drawCall.modelview, modelview);

  return drawCall;
}

/* Drawing functions */
TCDEF void tc_batch_set_texture(tc_Batch *batch, tc_Texture tex) {
  if (tex.id != batch->drawCalls[batch->drawCallIndex].textureId) {
    tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];

//     batch->totalIndexCount += prev->indexCount;
//     batch->totalVertexCount += prev->vertexCount;

    int start;
    if (prev->mode == TC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tc_create_drawcall(start, tex.id, prev->mode, &prev->clip, prev->modelview);
    } else {
      prev->textureId = tex.id;
      prev->indexStart = start;
    }
    batch->currentTexture = tex;
  }
}

TCDEF void tc_batch_set_draw_mode(tc_Batch *batch, TC_DRAW_MODE mode) {
  if (mode != batch->drawCalls[batch->drawCallIndex].mode) {
    tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
    int start = 0;

    if (prev->mode == TC_LINES) start = batch->totalIndexCount;
    else start = batch->totalVertexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tc_create_drawcall(start, prev->textureId, mode, &prev->clip, prev->modelview);
    } else {
      prev->mode = mode;
      prev->indexStart = start;
    }
  }
}

TCDEF void tc_batch_set_clip(tc_Batch *batch, tc_Rect clip) {
  tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
  int diff = clip.x != prev->clip.x || clip.y != prev->clip.y || clip.w != prev->clip.w || clip.h != prev->clip.h;
  if (diff) {
//     tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
    int start = 0;

    if (prev->mode == TC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tc_create_drawcall(start, prev->textureId, prev->mode, &clip, prev->modelview);
    } else {
      prev->clip = clip;
      prev->indexStart = start;
    }
  }
}

TCDEF void tc_batch_set_transform(tc_Batch *batch, tc_Matrix transform) {
  tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
  int diff = !tc_matrix_equals(transform, prev->modelview);
  if (diff) {
    int start = 0;

    if (prev->mode == TC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tc_create_drawcall(start, prev->textureId, prev->mode, &prev->clip, transform);
    } else {
      prev->modelview = transform;
      prev->indexStart = start;
    }
  }
}

TCDEF void tc_batch_add_quad(tc_Batch *batch, int x, int y, int width, int height, tc_Color color) {
  tc_bool inv = tc_false;
  if (height < 0) {
    inv = tc_true;
    height = -height;
  }

  tc_Rectf rect = {0, 0, width, height};

  float s0 = (float)rect.x / (float)width;
  float t0 = (float)rect.y / (float)height;
  float s1 = s0 + ((float)rect.w / (float)width);
  float t1 = t0 + ((float)rect.h / (float)height);
  if (inv) {
    int yy = t0;
    t0 = t1;
    t1 = yy;
  }

  tc_Vec4 pos = tc_vec4_new(x, y, x + rect.w, y + rect.h);
  tc_Vec4 uv = tc_vec4_new(s0, t0, s1, t1);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  if (tc_batch_is_full(*batch, 4)) return;
  if (curr->mode == TC_TRIANGLES) {
    batch->verticesPtr[0] = tc_vertexc(pos.x, pos.y, color, uv.x, uv.y);
    batch->verticesPtr[1] = tc_vertexc(pos.z, pos.y, color, uv.z, uv.y);
    batch->verticesPtr[2] = tc_vertexc(pos.z, pos.w, color, uv.z, uv.w);
    batch->verticesPtr[3] = tc_vertexc(pos.x, pos.w, color, uv.x, uv.w);

    curr->indexCount += 6;
    curr->vertexCount += 4;
    batch->verticesPtr += 4;
  } else {
    pos.w = pos.w - 1;
    float offset = 0;
    // if (render->state.camera) offset = 1.f-(1.f/render->state.camera->data[0][0]);
    batch->verticesPtr[0] = tc_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);
    batch->verticesPtr[1] = tc_vertexc(pos.z, pos.y, color, uv.z, uv.y);

    batch->verticesPtr[2] = tc_vertexc(pos.z, pos.y, color, uv.z, uv.y);
    batch->verticesPtr[3] = tc_vertexc(pos.z, pos.w, color, uv.z, uv.w);

    batch->verticesPtr[4] = tc_vertexc(pos.z, pos.w, color, uv.z, uv.w);
    batch->verticesPtr[5] = tc_vertexc(pos.x+offset, pos.w, color, uv.x, uv.w);

    batch->verticesPtr[6] = tc_vertexc(pos.x+1, pos.w, color, uv.x, uv.w);
    batch->verticesPtr[7] = tc_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);

    curr->indexCount += 12;
    curr->vertexCount += 8;
    batch->verticesPtr += 8;
  }
  curr->vertexCount += curr->vertexCount % 4;
  curr->indexCount += curr->indexCount % 6;
}

TCDEF void tc_batch_add_rect(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, tc_Color color) {
  int width = batch->currentTexture.width;
  int height = batch->currentTexture.height;
  tc_bool inv = tc_false;
//   if (dst.h < 0) {
//     inv = tc_true;
// //     dst.h = -dst.h;
//   }

  // tc_Rectf rect = {0, 0, dst.width, dst.height};

  float s0 = src.x / (float)width;
  float t0 = src.y / (float)height;
  float s1 = s0 + (src.w / (float)width);
  float t1 = t0 + (src.h / (float)height);
//   if (inv) {
//     int yy = t0;
//     t0 = t1;
//     t1 = yy;
//   }
  tc_Vec2 i = tc_vec2_new(0, 0);
//   if (inv) i = vec2_new(-dst.h, -dst.h);

//   TRACELOG("%f %f", dst.h, i.x);

  tc_Vec4 pos = tc_vec4_new((int)dst.x, (int)dst.y, (int)(dst.x + dst.w), (int)(dst.y + dst.h));
  tc_Vec4 uv = tc_vec4_new(s0, t0, s1, t1);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];

  int indexCount = batch->totalIndexCount % 6;
  int vertexCount = batch->totalVertexCount % 4;
  if (indexCount != 0 || vertexCount != 0) {
    batch->totalIndexCount += indexCount;
    batch->totalVertexCount += vertexCount;
    curr->indexCount += indexCount;
    curr->vertexCount += vertexCount;
    batch->verticesPtr += vertexCount;
    indexCount = 0;
    vertexCount = 0;
  }

  if (tc_batch_is_full(*batch, 8)) return;
  if (curr->mode == TC_TRIANGLES) {
    batch->verticesPtr[0] = tc_vertexc(pos.x, pos.y + i.x, color, uv.x, uv.y);
    batch->verticesPtr[1] = tc_vertexc(pos.z, pos.y + i.x, color, uv.z, uv.y);
    batch->verticesPtr[2] = tc_vertexc(pos.z, pos.w + i.y, color, uv.z, uv.w);
    batch->verticesPtr[3] = tc_vertexc(pos.x, pos.w + i.y, color, uv.x, uv.w);

    indexCount += 6;
    vertexCount += 4;
    batch->verticesPtr += 4;
  } else {
//     pos.y = pos.y + 1;
//     pos.w = pos.w - 1;
    if (Core.render.state.currentCanvas.id == 0) pos.y += 1;
    else {
      pos.w -= 1;
//       pos.z -= 1;
    }
    float offset = 0;
    tc_Matrix *camera = Core.render.state.camera;
    if (camera) offset = 1.f-(1.f/camera->data[0][0]);
    batch->verticesPtr[0] = tc_vertexc(pos.x+offset, pos.y, color, uv.x, uv.y);
    batch->verticesPtr[1] = tc_vertexc(pos.z, pos.y, color, uv.z, uv.y);

    batch->verticesPtr[2] = tc_vertexc(pos.z, pos.y, color, uv.z, uv.y);
    batch->verticesPtr[3] = tc_vertexc(pos.z, pos.w, color, uv.z, uv.w);

    batch->verticesPtr[4] = tc_vertexc(pos.z, pos.w, color, uv.z, uv.w);
    batch->verticesPtr[5] = tc_vertexc(pos.x+offset, pos.w, color, uv.x, uv.w);

    batch->verticesPtr[6] = tc_vertexc(pos.x+1, pos.w, color, uv.x, uv.w);
    batch->verticesPtr[7] = tc_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);

    indexCount += 12;
    vertexCount += 8;
    batch->verticesPtr += 8;
  }
  curr->vertexCount += vertexCount;
  curr->indexCount += indexCount;
  batch->totalVertexCount += vertexCount;
  batch->totalIndexCount += indexCount;
}
// TCDEF void tc_batch_add_rect_scale(tc_Batch *batch, tc_Rectf rect, float sx, float sy, tc_Color color);
TCDEF void tc_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, float angle, tc_Vec2 center, tc_Color color) {
  tc_bool inv = tc_false;
// 	if (dst.h < 0) {
//     inv = tc_true;
// 		dst.h = -dst.h;
// 	}
	int x = dst.x;
	int y = dst.y;
  int width = batch->currentTexture.width;
  int height = batch->currentTexture.height;

  tc_Matrix model;
	tc_matrix_translate(&model, dst.x, dst.y, 0);
	tc_matrix_rotate_z(&model, model, angle);
	// matrix_translate(&model, -cx, -cy, 0);
	tc_matrix_scale_aniso(&model, model, dst.w, dst.h, 1);
	float ox = center.x / (float)src.w;
	float oy = center.y / (float)src.h;
	tc_Matrix pos = {
			-ox, -oy, 0, 0,
			1-ox, -oy, 0, 0,
			1 - ox, 1 - oy, 0, 0,
			-ox, 1 - oy, 0, 0};

	tc_matrix_mul(&pos, model, pos);

  float s0 = src.x / (float)width;
	float t0 = src.y / (float)height;
	float s1 = s0 + (src.w / (float)width);
	float t1 = t0 + (src.h / (float)height);

//   if (inv) {
//     float y = t0;
//     t0 = t1;
//     t1 = y;
//   }

	tc_Vec2 t[4] = {
			{s0, t0},
			{s1, t0},
			{s1, t1},
			{s0, t1}};

  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  int indexCount = batch->totalIndexCount % 6;
  int vertexCount = batch->totalVertexCount % 4;
  if (indexCount != 0 || vertexCount != 0) {
    batch->totalIndexCount += indexCount;
    batch->totalVertexCount += vertexCount;
    curr->indexCount += indexCount;
    curr->vertexCount += vertexCount;
    batch->verticesPtr += vertexCount;
    indexCount = 0;
    vertexCount = 0;
  }

  if (tc_batch_is_full(*batch, 8)) return;
  if (curr->mode == TC_TRIANGLES) {
    for (int i = 0; i < 4; i++)
    {
      batch->verticesPtr[i] = tc_vertexc(x + pos.v[i].x, y + pos.v[i].y, color, t[i].x, t[i].y);
    }

    batch->verticesPtr += 4;
    indexCount += 6;
    vertexCount += 4;
  } else {
    int indices[8] = {0, 1, 1, 2, 2, 3, 3, 0};
    for (int i = 0; i < 8; i++) {
      int index = indices[i];
      batch->verticesPtr[i] = tc_vertexc(x + pos.v[index].x, y + pos.v[index].y, color, t[index].x, t[index].y);
    }
    batch->verticesPtr += 8;
    indexCount += 12;
    vertexCount += 8;
  }

  curr->indexCount += indexCount;
  curr->vertexCount += vertexCount;
  batch->totalIndexCount += indexCount;
  batch->totalVertexCount += vertexCount;
}

TCDEF void tc_batch_add_triangle(tc_Batch *batch, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
    tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
    int indexCount = batch->totalIndexCount % 6;
    int vertexCount = batch->totalVertexCount % 4;
    if (indexCount != 0 || vertexCount != 0) {
      batch->totalIndexCount += indexCount;
      batch->totalVertexCount += vertexCount;
      curr->indexCount += indexCount;
      curr->vertexCount += vertexCount;
      batch->verticesPtr += vertexCount;
      indexCount = 0;
      vertexCount = 0;
    }

    if (tc_batch_is_full(*batch, 8)) return;
    if (curr->mode == TC_TRIANGLES) {
      *batch->verticesPtr = tc_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tc_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      *batch->verticesPtr = tc_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tc_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      indexCount += 6;
      vertexCount += 4;
    } else {
      *batch->verticesPtr = tc_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;
      *batch->verticesPtr = tc_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      *batch->verticesPtr = tc_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;
      *batch->verticesPtr = tc_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tc_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;
      *batch->verticesPtr = tc_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;

      indexCount += 12;
      vertexCount += 8;
    }

    curr->vertexCount += vertexCount;
    curr->indexCount += indexCount;
    batch->totalVertexCount += vertexCount;
    batch->totalIndexCount += indexCount;
}

TCDEF void tc_batch_add_trianglev(tc_Batch *batch, tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tc_batch_add_triangle(batch, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}

TCDEF void tc_batch_add_circle(tc_Batch *batch, tc_Vec2 pos, float radius, int sides, tc_Color color) {
  int x = pos.x;
  int y = pos.y;
  int numSides = sides >= 4 ? sides : 4;

  double pi2 = 2.0 * PI;

  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  int indexCount = batch->totalIndexCount % 6;
  int vertexCount = batch->totalVertexCount % 4;
  if (indexCount != 0 || vertexCount != 0) {
    batch->totalIndexCount += indexCount;
    batch->totalVertexCount += vertexCount;
    curr->indexCount += indexCount;
    curr->vertexCount += vertexCount;
    batch->verticesPtr += vertexCount;
    indexCount = 0;
    vertexCount = 0;
  }

  if (tc_batch_is_full(*batch, numSides * 2)) return;
  if (curr->mode == TC_TRIANGLES) {
    for (int i = 0; i < numSides; i += 2) {
      *batch->verticesPtr = tc_vertexc(x, y, color, 0, 0);
      batch->verticesPtr++;

      float theta = (i * pi2) / numSides;
      float rad = deg2rad(i);
      *batch->verticesPtr = tc_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+1) * pi2) / numSides;
      *batch->verticesPtr = tc_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+2) * pi2) / numSides;
      *batch->verticesPtr = tc_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      vertexCount += 4;
      indexCount += 6;
    }
  } else {
    for (int i = 0; i < numSides; i++) {
      float theta = (i * pi2) / numSides;
      *batch->verticesPtr = tc_vertexc(x + (cosf(theta) * radius), y + (sinf(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+1) * pi2) / numSides;
      *batch->verticesPtr = tc_vertexc(x + (cosf(theta) * radius), y + (sinf(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      vertexCount += 2;
      indexCount += 3;
    }
  }
//   vertexCount += vertexCount % 4;
//   indexCount += indexCount % 6;

  curr->vertexCount += vertexCount;
  curr->indexCount += indexCount;
  batch->totalVertexCount += vertexCount;
  batch->totalIndexCount += indexCount;
}

TCDEF void tc_batch_add_line(tc_Batch *batch, int x0, int y0, int x1, int y1, tc_Color color) {
  tc_batch_set_draw_mode(batch, TC_LINES);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  if (tc_batch_is_full(*batch, 2)) return;

  batch->verticesPtr[0] = tc_vertexc(x0, y0, color, 0, 0);
  batch->verticesPtr[1] = tc_vertexc(x1, y1, color, 0, 0);

  batch->verticesPtr += 2;
  curr->vertexCount += 2;
  curr->indexCount += 3;
  batch->totalVertexCount += 2;
  batch->totalIndexCount += 3;
}

TCDEF void tc_begin_batch(tc_Batch *batch) {
  batch->drawCallIndex = 0;
  batch->totalVertexCount = 0;
  batch->totalIndexCount = 0;
  batch->verticesPtr = batch->vertices;
  batch->currentClip = tc_rect(0, 0, Core.window.width, Core.window.height);
  tc_Matrix modelview;
  tc_matrix_identity(&modelview);
  batch->drawCalls[0] = tc_create_drawcall(0, batch->currentTexture.id, TC_TRIANGLES, &batch->currentClip, modelview);
}

TCDEF void tc_flush_batch(tc_Batch *batch) {
  size_t size = (tc_uint8 *)batch->verticesPtr - (tc_uint8 *)batch->vertices;
  glBindVertexArray(batch->vao);
  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch->vertices);
}

TCDEF void tc_draw_batch(tc_Batch *batch) {
  if (batch->drawCalls[0].indexCount <= 0) return;

//   matrix modelview;
//   matrix_identity(&modelview);

  int offset = 0;
  int index = batch->drawCallIndex + 1;
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);

  for (int i = 0; i < index; i++) {
    tc_DrawCall curr = batch->drawCalls[i];
    tc_shader_send_uniform(Core.render.state.currentShader, "modelview", curr.modelview.data, TC_UNIFORM_MATRIX);

    glScissor(curr.clip.x, view[3] - (curr.clip.y + curr.clip.h), curr.clip.w, curr.clip.h);

    glBindTexture(GL_TEXTURE_2D, curr.textureId);
    glBindVertexArray(batch->vao);
    if (curr.mode == TC_LINES) glDrawArrays(GL_LINES, curr.indexStart, curr.vertexCount);
    else glDrawElements(GL_TRIANGLES, curr.indexCount, GL_UNSIGNED_INT, (GLvoid*) (curr.indexStart*sizeof(GLuint)));
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

TCDEF void tc_clear_batch(tc_Batch *batch) {
  tc_begin_batch(batch);
}

TCDEF tc_Matrix* tc_batch_get_transform(tc_Batch *batch) {
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  return &curr->modelview;
}

TCDEF tc_bool tc_batch_is_empty(tc_Batch batch) {
  return batch.totalIndexCount <= 0 && batch.totalVertexCount <= 0;
}

TCDEF tc_bool tc_batch_is_full(tc_Batch batch, int vertices) {
  return batch.totalIndexCount >= batch.maxIndices || (batch.totalVertexCount+vertices) >= batch.maxVertices || batch.drawCallIndex >= MAX_DRAW_CALLS;
}

TCDEF void tc_batch_draw_reset(tc_Batch *batch) {
  tc_flush_batch(batch);
  tc_draw_batch(batch);
  tc_begin_batch(batch);
}

TCDEF void tc_batch_reset_if_full(tc_Batch *batch, int vertex) {
  if (tc_batch_is_full(*batch, vertex)) {
    tc_batch_draw_reset(batch);
  }
}

#endif

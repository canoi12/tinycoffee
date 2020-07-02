#include "tico.h"

enum {
  TIC_POS_INDEX = 0,
  TIC_COLOR_INDEX = 2,
  TIC_TEXCOORD_INDEX = 6
};

static void tic_render_mode_canvas_2D() {
  glBindFramebuffer(GL_FRAMEBUFFER, Core.render.state.defaultCanvas.id);
}
static void tic_render_mode_canvas_stretch() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

tc_bool tic_render_init(tc_Render *render) {
	render->state.shapeTexture = tic_texture_create_named("default", WHITE.data, 1, 1, GL_RGBA);
  tic_texture_set_wrap(&render->state.shapeTexture, GL_REPEAT, GL_REPEAT);
  render->state.defaultTextureId = render->state.shapeTexture.id;
  render->state.currentTextureId = render->state.defaultTextureId;
  render->state.backToDefaultCanvas = &tic_render_mode_canvas_2D;
  if (Core.state.mode == TIC_MODE_EDITOR) {
    render->state.backToDefaultCanvas = &tic_render_mode_canvas_stretch;
  }

  render->state.camera = NULL;
  tic_matrix_identity(&render->state.modelview);
  render->state.defaultCanvas = tic_canvas_create(1366, 768);

  render->batch = tic_batch_create(render->state.shapeTexture, DEFAULT_MAX_QUADS);

  render->state.drawCount = 0;
  render->state.defaultShader = tic_shader_load_default(&render->state.defaultVertexShader, &render->state.defaultFragmentShader);
  render->state.currentShader = render->state.defaultShader;

  render->state.currentCanvas.id = 0;
  render->state.currentMatrix = 0;

  return tc_true;
}

void tic_render_destroy(tc_Render *render) {
  tic_batch_destroy(&render->batch);
  tic_texture_destroy(&render->state.shapeTexture);
  glDeleteShader(render->state.defaultVertexShader);
  glDeleteShader(render->state.defaultFragmentShader);
  tic_shader_destroy(&render->state.defaultShader);
  TRACELOG("Render destroyed");
}


tc_Batch tic_batch_create(tc_Texture tex, int maxQuads) {
  tc_Batch batch = {0};
  if (maxQuads <= 0) maxQuads = DEFAULT_MAX_QUADS;
  batch.maxQuads = maxQuads;
  batch.maxIndices = maxQuads * 6;
  batch.maxVertices = maxQuads * 4;
  batch.totalIndexCount = 0;
  batch.totalVertexCount = 0;

  batch.vertices = TIC_MALLOC(batch.maxVertices * sizeof(tc_Vertex));
  batch.indices = TIC_MALLOC(batch.maxIndices * sizeof(int));

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
  tic_matrix_identity(&modelview);
  batch.drawCalls[0] = tic_drawcall_create(0, tex.id, TIC_TRIANGLES, NULL, modelview);
  batch.drawCallIndex = 0;

  glGenVertexArrays(1, &batch.vao);
  glGenBuffers(2, batch.vbo);

  glBindVertexArray(batch.vao);
  glBindBuffer(GL_ARRAY_BUFFER, batch.vbo[0]);

  glBufferData(GL_ARRAY_BUFFER, batch.maxVertices * sizeof(tc_Vertex), NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)TIC_POS_INDEX);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)(sizeof(float) * TIC_COLOR_INDEX));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(tc_Vertex), (void*)(sizeof(float) * TIC_TEXCOORD_INDEX));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch.maxIndices * sizeof(int), batch.indices, GL_STATIC_DRAW);

  TRACELOG("Batch created");

  return batch;
}
void tic_batch_destroy(tc_Batch *batch) {
  TIC_FREE(batch->vertices);
  TIC_FREE(batch->indices);
  glDeleteVertexArrays(1, &batch->vao);
  glDeleteBuffers(2, batch->vbo);
}

tc_DrawCall tic_drawcall_create(int start, int texture, TIC_DRAW_MODE mode, tc_Rect *clip, tc_Matrix modelview) {
  tc_DrawCall drawCall = {0};
  drawCall.indexStart = start;
  drawCall.textureId = texture;
  drawCall.mode = mode;
  if (clip) drawCall.clip = *clip;
  drawCall.indexCount = 0;
  drawCall.vertexCount = 0;
//   drawCall.modelview = modelview;
  tic_matrix_clone(&drawCall.modelview, modelview);

  return drawCall;
}

tc_DrawCall *tic_batch_next_drawcall(tc_Batch *batch, tc_bool inv) {
  int index = batch->drawCallIndex;
  tc_DrawCall *curr = &batch->drawCalls[index];
  if (curr->indexCount == 0) return curr;
  int next = ++batch->drawCallIndex;
  if (next >= MAX_DRAW_CALLS) return NULL;

  tic_batch_align(batch);
  int start;
  if (batch->drawCalls[index].mode == TIC_LINES) start = inv ? batch->totalIndexCount : batch->totalVertexCount;
  else start = inv ? batch->totalVertexCount : batch->totalIndexCount;

  batch->drawCalls[next] = tic_drawcall_create(start, curr->textureId, curr->mode, &curr->clip, curr->modelview);
  return &batch->drawCalls[next];
}

/* Drawing functions */
void tic_batch_set_texture(tc_Batch *batch, tc_Texture tex) {
  if (tex.id != batch->drawCalls[batch->drawCallIndex].textureId) {
    tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];

//     batch->totalIndexCount += prev->indexCount;
//     batch->totalVertexCount += prev->vertexCount;

    tic_batch_align(batch);
    int start;
    if (prev->mode == TIC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tic_drawcall_create(start, tex.id, prev->mode, &prev->clip, prev->modelview);
    } else {
      prev->textureId = tex.id;
      prev->indexStart = start;
    }
    batch->currentTexture = tex;
  }
}

void tic_batch_set_draw_mode(tc_Batch *batch, TIC_DRAW_MODE mode) {
  if (mode != batch->drawCalls[batch->drawCallIndex].mode) {
    tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
    int start = 0;

    tic_batch_align(batch);
    if (prev->mode == TIC_LINES) start = batch->totalIndexCount;
    else start = batch->totalVertexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tic_drawcall_create(start, prev->textureId, mode, &prev->clip, prev->modelview);
    } else {
      prev->mode = mode;
      prev->indexStart = start;
    }
  }
}

void tic_batch_set_clip(tc_Batch *batch, tc_Rect clip) {
  tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
  int diff = clip.x != prev->clip.x || clip.y != prev->clip.y || clip.w != prev->clip.w || clip.h != prev->clip.h;
  if (diff) {
//     tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
    tic_batch_align(batch);
    int start = 0;

    if (prev->mode == TIC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tic_drawcall_create(start, prev->textureId, prev->mode, &clip, prev->modelview);
    } else {
      prev->clip = clip;
      prev->indexStart = start;
    }
  }
}

void tic_batch_set_transform(tc_Batch *batch, tc_Matrix transform) {
  tc_DrawCall *prev = &batch->drawCalls[batch->drawCallIndex];
  int diff = !tic_matrix_equals(transform, prev->modelview);
  if (diff) {
    tic_batch_align(batch);
    int start = 0;

    if (prev->mode == TIC_LINES) start = batch->totalVertexCount;
    else start = batch->totalIndexCount;

    if (prev->indexCount != 0) {
      batch->drawCallIndex++;
      batch->drawCalls[batch->drawCallIndex] = tic_drawcall_create(start, prev->textureId, prev->mode, &prev->clip, transform);
    } else {
      prev->modelview = transform;
      prev->indexStart = start;
    }
  }
}

void tic_batch_align(tc_Batch *batch) {
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
	int indexDif = batch->totalIndexCount % 6;
	int vertexDif = batch->totalVertexCount % 4;
	batch->totalIndexCount += indexDif;
	batch->totalVertexCount += vertexDif;
	curr->indexCount += indexDif;
	curr->vertexCount += vertexDif;
	batch->verticesPtr += vertexDif;
}

void tic_batch_update_count(tc_Batch *batch, int indexCount, int vertexCount) {
	tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
	curr->vertexCount += vertexCount;
  curr->indexCount += indexCount;
  batch->totalVertexCount += vertexCount;
  batch->totalIndexCount += indexCount;
}

void tic_batch_add_quad(tc_Batch *batch, int x, int y, int width, int height, tc_Color color) {
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

  tc_Vec4 pos = tic_vec4_new(x, y, x + rect.w, y + rect.h);
  tc_Vec4 uv = tic_vec4_new(s0, t0, s1, t1);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  if (tic_batch_is_full(*batch, 4)) return;
  if (curr->mode == TIC_TRIANGLES) {
    batch->verticesPtr[0] = tic_vertexc(pos.x, pos.y, color, uv.x, uv.y);
    batch->verticesPtr[1] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);
    batch->verticesPtr[2] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);
    batch->verticesPtr[3] = tic_vertexc(pos.x, pos.w, color, uv.x, uv.w);

    curr->indexCount += 6;
    curr->vertexCount += 4;
    batch->verticesPtr += 4;
  } else {
    pos.w = pos.w - 1;
    float offset = 0;
    // if (render->state.camera) offset = 1.f-(1.f/render->state.camera->data[0][0]);
    batch->verticesPtr[0] = tic_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);
    batch->verticesPtr[1] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);

    batch->verticesPtr[2] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);
    batch->verticesPtr[3] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);

    batch->verticesPtr[4] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);
    batch->verticesPtr[5] = tic_vertexc(pos.x+offset, pos.w, color, uv.x, uv.w);

    batch->verticesPtr[6] = tic_vertexc(pos.x+1, pos.w, color, uv.x, uv.w);
    batch->verticesPtr[7] = tic_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);

    curr->indexCount += 12;
    curr->vertexCount += 8;
    batch->verticesPtr += 8;
  }
  curr->vertexCount += curr->vertexCount % 4;
  curr->indexCount += curr->indexCount % 6;
}

void tic_batch_add_rect(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, tc_Color color) {
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
  tc_Vec2 i = tic_vec2_new(0, 0);
//   if (inv) i = vec2_new(-dst.h, -dst.h);

//   TRACELOG("%f %f", dst.h, i.x);

  tc_Vec4 pos = tic_vec4_new((int)dst.x, (int)dst.y, (int)(dst.x + dst.w), (int)(dst.y + dst.h));
  tc_Vec4 uv = tic_vec4_new(s0, t0, s1, t1);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  int indexCount = 0;
  int vertexCount = 0;

  tic_batch_align(batch);

  if (tic_batch_is_full(*batch, 8)) return;
  if (curr->mode == TIC_TRIANGLES) {
	  batch->verticesPtr[0] = tic_vertexc(pos.x, pos.y + i.x, color, uv.x, uv.y);
	  batch->verticesPtr[1] = tic_vertexc(pos.z, pos.y + i.x, color, uv.z, uv.y);
	  batch->verticesPtr[2] = tic_vertexc(pos.z, pos.w + i.y, color, uv.z, uv.w);
	  batch->verticesPtr[3] = tic_vertexc(pos.x, pos.w + i.y, color, uv.x, uv.w);

	  indexCount += 6;
	  vertexCount += 4;

	  batch->verticesPtr += 4;
	} else {
		if (Core.render.state.currentCanvas.id == 0) pos.y += 1;
	  else {
	    pos.w -= 1;
  	}
	  float offset = 0;
	  tc_Matrix *camera = Core.render.state.camera;
	  if (camera) offset = 1.f-(1.f/camera->data[0][0]);
	  batch->verticesPtr[0] = tic_vertexc(pos.x+offset, pos.y, color, uv.x, uv.y);
	  batch->verticesPtr[1] = tic_vertexc(pos.z-0.5, pos.y, color, uv.z, uv.y);

	  batch->verticesPtr[2] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);
	  batch->verticesPtr[3] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);

	  batch->verticesPtr[4] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);
	  batch->verticesPtr[5] = tic_vertexc(pos.x+offset, pos.w, color, uv.x, uv.w);

	  batch->verticesPtr[6] = tic_vertexc(pos.x+1, pos.w, color, uv.x, uv.w);
	  batch->verticesPtr[7] = tic_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);

	  batch->verticesPtr += 8;

	  indexCount += 12;
	  vertexCount += 8;
	}
  tic_batch_update_count(batch, indexCount, vertexCount);
}

void tic_batch_add_line_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf src, tc_Color color) {
	tic_batch_set_draw_mode(batch, TIC_LINES);
	int width = batch->currentTexture.width;
  int height = batch->currentTexture.height;
  tc_bool inv = tc_false;

  float s0 = src.x / (float)width;
  float t0 = src.y / (float)height;
  float s1 = s0 + (src.w / (float)width);
  float t1 = t0 + (src.h / (float)height);
  tc_Vec2 i = tic_vec2_new(0, 0);

  tc_Vec4 pos = tic_vec4_new((int)dest.x, (int)dest.y, (int)(dest.x + dest.w), (int)(dest.y + dest.h));
  tc_Vec4 uv = tic_vec4_new(s0, t0, s1, t1);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];

  int indexCount = 0;
  int vertexCount = 0;

  tic_batch_align(batch);

	if (Core.render.state.currentCanvas.id == 0) pos.y += 1;
  else {
    pos.w -= 1;
  }
  float offset = 0;
  tc_Matrix *camera = Core.render.state.camera;
  if (camera) offset = 1.f-(1.f/camera->data[0][0]);
  batch->verticesPtr[0] = tic_vertexc(pos.x+offset, pos.y, color, uv.x, uv.y);
  batch->verticesPtr[1] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);

  batch->verticesPtr[2] = tic_vertexc(pos.z, pos.y, color, uv.z, uv.y);
  batch->verticesPtr[3] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);

  batch->verticesPtr[4] = tic_vertexc(pos.z, pos.w, color, uv.z, uv.w);
  batch->verticesPtr[5] = tic_vertexc(pos.x+offset, pos.w, color, uv.x, uv.w);

  batch->verticesPtr[6] = tic_vertexc(pos.x+1, pos.w, color, uv.x, uv.w);
  batch->verticesPtr[7] = tic_vertexc(pos.x+1, pos.y, color, uv.x, uv.y);

  batch->verticesPtr += 8;

  indexCount += 12;
  vertexCount += 8;

  curr->vertexCount += vertexCount;
  curr->indexCount += indexCount;
  batch->totalVertexCount += vertexCount;
  batch->totalIndexCount += indexCount;
  tic_batch_update_count(batch, indexCount, vertexCount);
}

void tic_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dst, tc_Rectf src, float angle, int cx, int cy, tc_Color color) {
  tc_bool inv = tc_false;

	int x = dst.x;
	int y = dst.y;
  int width = batch->currentTexture.width;
  int height = batch->currentTexture.height;

  tc_Matrix model;
	tic_matrix_translate(&model, dst.x, dst.y, 0);
	tic_matrix_rotate_z(&model, model, angle);
	// matrix_translate(&model, -cx, -cy, 0);
	tic_matrix_scale_aniso(&model, model, dst.w, dst.h, 1);
	float ox = cx / (float)src.w;
	float oy = cy / (float)src.h;
	tc_Matrix pos = {
			-ox, -oy, 0, 0,
			1-ox, -oy, 0, 0,
			1 - ox, 1 - oy, 0, 0,
			-ox, 1 - oy, 0, 0};

	tic_matrix_mul(&pos, model, pos);

  float s0 = src.x / (float)width;
	float t0 = src.y / (float)height;
	float s1 = s0 + (src.w / (float)width);
	float t1 = t0 + (src.h / (float)height);

	tc_Vec2 t[4] = {
			{s0, t0},
			{s1, t0},
			{s1, t1},
			{s0, t1}};

  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  int indexCount = 0;
  int vertexCount = 0;
  tic_batch_align(batch);

  if (tic_batch_is_full(*batch, 8)) return;
  if (curr->mode == TIC_TRIANGLES) {
    for (int i = 0; i < 4; i++)
    {
      batch->verticesPtr[i] = tic_vertexc(x + pos.v[i].x, y + pos.v[i].y, color, t[i].x, t[i].y);
    }

    batch->verticesPtr += 4;
    indexCount += 6;
    vertexCount += 4;
  } else {
    int indices[8] = {0, 1, 1, 2, 2, 3, 3, 0};
    for (int i = 0; i < 8; i++) {
      int index = indices[i];
      batch->verticesPtr[i] = tic_vertexc(x + pos.v[index].x, y + pos.v[index].y, color, t[index].x, t[index].y);
    }
    batch->verticesPtr += 8;
    indexCount += 12;
    vertexCount += 8;
  }

  tic_batch_update_count(batch, indexCount, vertexCount);
}

void tic_batch_add_triangle(tc_Batch *batch, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color) {
    tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];

    int indexCount = 0;
    int vertexCount = 0;
    tic_batch_align(batch);

    if (tic_batch_is_full(*batch, 8)) return;
    if (curr->mode == TIC_TRIANGLES) {
      *batch->verticesPtr = tic_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tic_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      *batch->verticesPtr = tic_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tic_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      indexCount += 6;
      vertexCount += 4;
    } else {
      *batch->verticesPtr = tic_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;
      *batch->verticesPtr = tic_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;

      *batch->verticesPtr = tic_vertexc(x1, y1, color, 0.5, 1);
      batch->verticesPtr++;
      *batch->verticesPtr = tic_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;

      *batch->verticesPtr = tic_vertexc(x2, y2, color, 1, 0);
      batch->verticesPtr++;
      *batch->verticesPtr = tic_vertexc(x0, y0, color, 0, 0);
      batch->verticesPtr++;

      indexCount += 12;
      vertexCount += 8;
    }

    tic_batch_update_count(batch, indexCount, vertexCount);
}

void tic_batch_add_trianglev(tc_Batch *batch, tc_Vec2 p0, tc_Vec2 p1, tc_Vec2 p2, tc_Color color) {
  tic_batch_add_triangle(batch, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, color);
}

void tic_batch_add_circle(tc_Batch *batch, int x, int y, float radius, int sides, tc_Color color) {
  int numSides = sides >= 4 ? sides : 4;

  double pi2 = 2.0 * PI;

  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  int indexCount = 0;
  int vertexCount = 0;
  tic_batch_align(batch);

  if (tic_batch_is_full(*batch, numSides * 2)) return;
  if (curr->mode == TIC_TRIANGLES) {
    for (int i = 0; i < numSides; i += 2) {
      *batch->verticesPtr = tic_vertexc(x, y, color, 0, 0);
      batch->verticesPtr++;

      float theta = (i * pi2) / numSides;
      float rad = deg2rad(i);
      *batch->verticesPtr = tic_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+1) * pi2) / numSides;
      *batch->verticesPtr = tic_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+2) * pi2) / numSides;
      *batch->verticesPtr = tic_vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      vertexCount += 4;
      indexCount += 6;
    }
  } else {
    for (int i = 0; i < numSides; i++) {
      float theta = (i * pi2) / numSides;
      *batch->verticesPtr = tic_vertexc(x + (cosf(theta) * radius), y + (sinf(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      theta = ((i+1) * pi2) / numSides;
      *batch->verticesPtr = tic_vertexc(x + (cosf(theta) * radius), y + (sinf(theta) * radius), color, 0, 0);
      batch->verticesPtr++;

      vertexCount += 2;
      indexCount += 3;
    }
  }
  tic_batch_update_count(batch, indexCount, vertexCount);

  // curr->vertexCount += vertexCount;
  // curr->indexCount += indexCount;
  // batch->totalVertexCount += vertexCount;
  // batch->totalIndexCount += indexCount;
}

void tic_batch_add_line(tc_Batch *batch, int x0, int y0, int x1, int y1, tc_Color color) {
  tic_batch_set_draw_mode(batch, TIC_LINES);
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  if (tic_batch_is_full(*batch, 2)) return;

  batch->verticesPtr[0] = tic_vertexc(x0, y0, color, 0, 0);
  batch->verticesPtr[1] = tic_vertexc(x1, y1, color, 0, 0);

  batch->verticesPtr += 2;
  curr->vertexCount += 2;
  curr->indexCount += 3;
  batch->totalVertexCount += 2;
  batch->totalIndexCount += 3;
}

void tic_batch_begin(tc_Batch *batch) {
  batch->drawCallIndex = 0;
  batch->totalVertexCount = 0;
  batch->totalIndexCount = 0;
  batch->verticesPtr = batch->vertices;
  batch->currentClip = tic_rect(0, 0, Core.window.width, Core.window.height);
  tc_Matrix modelview;
  tic_matrix_identity(&modelview);
  batch->drawCalls[0] = tic_drawcall_create(0, batch->currentTexture.id, TIC_TRIANGLES, &batch->currentClip, modelview);
}

void tic_batch_flush(tc_Batch *batch) {
  size_t size = (tc_uint8 *)batch->verticesPtr - (tc_uint8 *)batch->vertices;
  glBindVertexArray(batch->vao);
  glBindBuffer(GL_ARRAY_BUFFER, batch->vbo[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch->vertices);
}

void tic_batch_draw(tc_Batch *batch) {
  if (batch->drawCalls[0].indexCount <= 0) return;

//   matrix modelview;
//   matrix_identity(&modelview);

  int offset = 0;
  int index = batch->drawCallIndex + 1;
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);

  for (int i = 0; i < index; i++) {
    tc_DrawCall curr = batch->drawCalls[i];
    tic_shader_send(Core.render.state.currentShader, "modelview", curr.modelview.data, TIC_UNIFORM_MATRIX);

    glScissor(curr.clip.x, view[3] - (curr.clip.y + curr.clip.h), curr.clip.w, curr.clip.h);

    glBindTexture(GL_TEXTURE_2D, curr.textureId);
    glBindVertexArray(batch->vao);
    if (curr.mode == TIC_LINES) glDrawArrays(GL_LINES, curr.indexStart, curr.vertexCount);
    else glDrawElements(GL_TRIANGLES, curr.indexCount, GL_UNSIGNED_INT, (GLvoid*) (curr.indexStart*sizeof(GLuint)));
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // TRACELOG("%d: %d %d %d %d", i, curr.mode, curr.indexStart, curr.indexCount, curr.vertexCount);
  }
}

void tic_batch_clear(tc_Batch *batch) {
  tic_batch_begin(batch);
}

tc_Matrix* tic_batch_get_transform(tc_Batch *batch) {
  tc_DrawCall *curr = &batch->drawCalls[batch->drawCallIndex];
  return &curr->modelview;
}

tc_bool tic_batch_is_empty(tc_Batch batch) {
  return batch.totalIndexCount <= 0 && batch.totalVertexCount <= 0;
}

tc_bool tic_batch_is_full(tc_Batch batch, int vertices) {
  return batch.totalIndexCount >= batch.maxIndices || (batch.totalVertexCount+vertices) >= batch.maxVertices || batch.drawCallIndex >= MAX_DRAW_CALLS;
}

void tic_batch_draw_reset(tc_Batch *batch) {
	tc_DrawCall last = batch->drawCalls[batch->drawCallIndex];
  tic_batch_flush(batch);
  tic_batch_draw(batch);
  tic_batch_begin(batch);
  batch->drawCalls[0].textureId = last.textureId;
  batch->drawCalls[0].clip = last.clip;
  batch->drawCalls[0].mode = last.mode;
  tic_matrix_clone(&batch->drawCalls[0].modelview, last.modelview);
}

void tic_batch_reset_if_full(tc_Batch *batch, int vertex) {
  if (tic_batch_is_full(*batch, vertex)) {
    tic_batch_draw_reset(batch);
  }
}

#ifndef TC_RENDER_H
#define TC_RENDER_H

#include "tinycoffee.h"

#define MAX_QUADS 10000
#define MAX_INDICES 6 * MAX_QUADS
#define MAX_VERTEX 4 * MAX_QUADS

#ifndef TCDEF
  #define TCDEF extern
#endif

enum {
  POS_INDEX = 0,
  COLOR_INDEX = 2,
  TEXCOORD_INDEX = 6
};

typedef enum {
  TC_TRIANGLES = 0,
  TC_LINES = 1
} RENDER_DRAW_MODE;

typedef struct tc_batch {
  unsigned int *indices;
  tc_vertex *vertices;
  tc_vertex *verticesPtr;

  unsigned int indexCount;

  unsigned int vao;
  unsigned int vbo[2];
} tc_batch;

typedef struct {
  tc_batch batch;
  struct {
    unsigned int defaultVertexShader;
    unsigned int defaultFragmentShader;

    unsigned int defaultTextureId;
    unsigned int currentTextureId;

    tc_shader defaultShader;
    tc_shader currentShader;

    tc_texture shapeTexture;
    tc_canvas currentCanvas;

    RENDER_DRAW_MODE drawMode;
  } state;
} tc_render;

TCDEF tc_render tc_create_render(unsigned int vertexShader, unsigned int fragmentShader);
TCDEF void tc_destroy_render(tc_render *render);

TCDEF void tc_render_draw_mode(tc_render *render, RENDER_DRAW_MODE mode);

TCDEF tc_batch tc_create_batch();
TCDEF void tc_destroy_batch(tc_batch *batch);

TCDEF void tc_begin_batch(tc_render *render);
TCDEF void tc_end_batch(tc_render *render);
TCDEF void tc_flush_batch(tc_render *render);
TCDEF void tc_reset_batch(tc_render *render);

TCDEF void tc_render_draw_quad(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, tc_color color);
TCDEF void tc_render_draw_quad_scale(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, float sx, float sy, tc_color color);
TCDEF void tc_render_draw_quad_ex(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, float angle, float sx, float sy, float cx, float cy, tc_color color);

TCDEF void tc_render_draw_circle(tc_render *render, unsigned int textureId, float x, float y, float radius, tc_color color);
TCDEF void tc_render_draw_triangle(tc_render *render, unsigned int textureId, float x0, float y0, float x1, float y1, float x2, float y2, tc_color color);

#endif /* TC_RENDER_H */

#if defined(TC_RENDER_IMPLEMENTATION)
TCDEF tc_render tc_create_render(unsigned int vertexShader, unsigned int fragmentShader) {
  TRACELOG("Creating render\n");
  tc_render render = {0};
  render.batch = tc_create_batch();
  render.state.defaultVertexShader = vertexShader;
  render.state.defaultFragmentShader = fragmentShader;

  tc_shader shader = {0};
  shader.id = tc_load_shader_program(vertexShader, fragmentShader);
  render.state.defaultShader = shader;
  render.state.currentShader = shader;

	render.state.shapeTexture = tc_create_texture(WHITE.data, 1, 1);
	render.state.defaultTextureId = render.state.shapeTexture.id;
	render.state.currentCanvas.id = 0;

  render.batch.indexCount = 0;
  render.state.drawMode = TC_TRIANGLES;

  TRACELOG("Render created\n");

  return render;
}

TCDEF void tc_destroy_render(tc_render *render) {
  tc_destroy_batch(&render->batch);
  tc_delete_texture(&render->state.shapeTexture);
}

TCDEF void tc_render_draw_mode(tc_render *render, RENDER_DRAW_MODE mode) {
  if (mode != render->state.drawMode) tc_reset_batch(render);
  render->state.drawMode = mode;
}

TCDEF tc_batch tc_create_batch() {
  tc_batch batch = { 0 };
	batch.indexCount = 0;
  int indicesSize = MAX_INDICES * sizeof(int);
  int verticesSize = MAX_VERTEX * sizeof(tc_vertex);
  batch.indices = malloc(indicesSize);
  batch.vertices = malloc(verticesSize);

	int offset = 0;

	for (int i = 0; i < MAX_INDICES; i+=6) {
		batch.indices[i] = offset;
		batch.indices[i + 1] = offset + 1;
		batch.indices[i + 2] = offset + 2;

		batch.indices[i + 3] = offset;
		batch.indices[i + 4] = offset + 2;
		batch.indices[i + 5] = offset + 3;
		offset += 4;
	}

  glGenVertexArrays(1, &batch.vao);
	glGenBuffers(2, batch.vbo);

	glBindVertexArray(batch.vao);
	glBindBuffer(GL_ARRAY_BUFFER, batch.vbo[0]);

	glBufferData(GL_ARRAY_BUFFER, verticesSize, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(tc_vertex), (void*)POS_INDEX);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(tc_vertex), (void*)(sizeof(float) * COLOR_INDEX));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(tc_vertex), (void*)(sizeof(float) * TEXCOORD_INDEX));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, batch.indices, GL_STATIC_DRAW);

  TRACELOG("Batch created\n");

	return batch;
}

TCDEF void tc_destroy_batch(tc_batch *batch) {
  free(batch->vertices);
  free(batch->indices);
  glDeleteVertexArrays(1, &batch->vao);
  glDeleteBuffers(2, batch->vbo);
}

TCDEF void tc_begin_batch(tc_render *render) {
  render->batch.verticesPtr = render->batch.vertices;
}
TCDEF void tc_end_batch(tc_render *render) {
  size_t size = (uint8_t *)render->batch.verticesPtr - (uint8_t *)render->batch.vertices;
	glBindVertexArray(render->batch.vao);
	glBindBuffer(GL_ARRAY_BUFFER, render->batch.vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, render->batch.vertices);
}
TCDEF void tc_flush_batch(tc_render *render) {
  matrix modelview;
	matrix_identity(&modelview);

	tc_shader_send_uniform(render->state.currentShader, "model", modelview.data, TC_UNIFORM_MATRIX);

	glBindFramebuffer(GL_FRAMEBUFFER, render->state.currentCanvas.id);
	glBindTexture(GL_TEXTURE_2D, render->state.currentTextureId);
	glBindVertexArray(render->batch.vao);
	if (render->state.drawMode == TC_TRIANGLES) glDrawElements(GL_TRIANGLES, render->batch.indexCount, GL_UNSIGNED_INT, NULL);
	else glDrawArrays(GL_LINES, 0, render->batch.indexCount);
//   glDrawArrays(GL_LINES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	render->batch.indexCount = 0;
}
TCDEF void tc_reset_batch(tc_render *render) {
  tc_end_batch(render);
  tc_flush_batch(render);
  tc_begin_batch(render);
}

TCDEF void tc_render_draw_quad(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, tc_color color) {
  if (render->batch.indexCount >= MAX_INDICES || render->state.currentTextureId != textureId)
		tc_reset_batch(render);

  // vec4_t uv = vec4(0, 0, 1, 1);
  int inv = TC_FALSE;
	if (height < 0) {
    inv = TC_TRUE;
		height = -height;
	}

  // tc_rectangle r;
  // r.x = rect.x / width;
	// r.y = rect.y / height;
	// r.width = rect.width / width;
	// r.height = rect.height / height;
	float s0 = rect.x / width;
	float t0 = rect.y / height;
	float s1 = s0 + (rect.width / width);
	float t1 = t0 + (rect.height / height);
  if (inv) {
    float y = t0;
    t0 = t1;
    t1 = y;
  }

  vec4 pos = vec4_new(x, y, x + rect.width, y + rect.height);
  vec4 uv = vec4_new(s0, t0, s1, t1);

  if (render->state.drawMode == TC_TRIANGLES) {
  	render->batch.verticesPtr[0] = vertexc(pos.x, pos.y, color, uv.x, uv.y);
  	render->batch.verticesPtr[1] = vertexc(pos.z, pos.y, color, uv.z, uv.y);
  	render->batch.verticesPtr[2] = vertexc(pos.z, pos.w, color, uv.z, uv.w);
  	render->batch.verticesPtr[3] = vertexc(pos.x, pos.w, color, uv.x, uv.w);

  	render->batch.indexCount += 6;
  	render->batch.verticesPtr += 4;
	} else {
	  pos.x = floor(pos.x);
	  pos.y = floor(pos.y);
	  pos.z = floor(pos.z);
	  pos.w = floor(pos.w - 1);
	  render->batch.verticesPtr[0] = vertexc(pos.x + 1, pos.y, color, uv.x, uv.y);
	  render->batch.verticesPtr[1] = vertexc(pos.z, pos.y, color, uv.z, uv.y);

	  render->batch.verticesPtr[2] = vertexc(pos.z, pos.y, color, uv.z, uv.y);
	  render->batch.verticesPtr[3] = vertexc(pos.z, pos.w, color, uv.z, uv.w);

	  render->batch.verticesPtr[4] = vertexc(pos.z, pos.w, color, uv.z, uv.w);
	  render->batch.verticesPtr[5] = vertexc(pos.x, pos.w, color, uv.x, uv.w);

	  render->batch.verticesPtr[6] = vertexc(pos.x + 1, pos.w, color, uv.x, uv.w);
	  render->batch.verticesPtr[7] = vertexc(pos.x + 1, pos.y, color, uv.x, uv.y);

	  render->batch.indexCount += 8;
	  render->batch.verticesPtr += 8;
	}
	render->state.currentTextureId = textureId;
}

TCDEF void tc_render_draw_quad_scale(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, float sx, float sy, tc_color color) {
  if (render->batch.indexCount >= MAX_INDICES || render->state.currentTextureId != textureId)
		tc_reset_batch(render);

  // vec4_t uv = vec4(0, 0, 1, 1);
  int inv = TC_FALSE;
	if (height < 0) {
    inv = TC_TRUE;
		height = -height;
	}

  // tc_rectangle r;
  // r.x = rect.x / width;
	// r.y = rect.y / height;
	// r.width = rect.width / width;
	// r.height = rect.height / height;
	float s0 = rect.x / width;
	float t0 = rect.y / height;
	float s1 = s0 + (rect.width / width);
	float t1 = t0 + (rect.height / height);

  if (inv) {
    float y = t0;
    t0 = t1;
    t1 = y;
  }

  // width *= sx;
  // height *= sy;

  vec4 pos = vec4_new(x, y, x + rect.width*sx, y + rect.height*sy);
  vec4 uv = vec4_new(s0, t0, s1, t1);

	render->batch.verticesPtr[0] = vertexc(pos.x, pos.y, color, uv.x, uv.y);
	render->batch.verticesPtr[1] = vertexc(pos.z, pos.y, color, uv.z, uv.y);
	render->batch.verticesPtr[2] = vertexc(pos.z, pos.w, color, uv.z, uv.w);
	render->batch.verticesPtr[3] = vertexc(pos.x, pos.w, color, uv.x, uv.w);

	// render->batch.verticesPtr[0] = vertexc(x, y, color, r.x, r.y);
	// render->batch.verticesPtr[1] = vertexc(x + width, y, color, r.x + r.width, r.y);
	// render->batch.verticesPtr[2] = vertexc(x + width, y + height, color, r.x + r.width, r.y + r.height);
	// render->batch.verticesPtr[3] = vertexc(x, y + height, color, r.x, r.y + r.height);

	render->batch.indexCount += 6;
	render->batch.verticesPtr += 4;
	render->state.currentTextureId = textureId;
}

TCDEF void tc_render_draw_quad_ex(tc_render *render, unsigned int textureId, tc_rectangle rect, float x, float y, float width, float height, float angle, float sx, float sy, float cx, float cy, tc_color color) {
  if (render->batch.indexCount >= MAX_INDICES || render->state.currentTextureId != textureId)
    tc_reset_batch(render);

  int inv = TC_FALSE;
	if (height < 0) {
    inv = TC_TRUE;
		height = -height;
	}

  matrix model;
	matrix_identity(&model);
	matrix_translate(&model, x, y, 0);
	matrix_rotate_z(&model, angle);
	// matrix_translate(&model, -cx, -cy, 0);
	matrix_scale(&model, rect.width * sx, rect.height * sy, 1);
	float ox = cx / rect.width;
	float oy = cy / rect.height;
	matrix pos = {
			-ox, -oy, 0, 0,
			1-ox, -oy, 0, 0,
			1 - ox, 1 - oy, 0, 0,
			-ox, 1 - oy, 0, 0};

	matrix_mul(&pos, model, pos);

  // tc_rectangle r;
	// r.x = rect.x / width;
	// r.y = rect.y / height;
	// r.width = rect.width / width;
	// r.height = rect.height / height;
  float s0 = rect.x / width;
	float t0 = rect.y / height;
	float s1 = s0 + (rect.width / width);
	float t1 = t0 + (rect.height / height);

  if (inv) {
    float y = t0;
    t0 = t1;
    t1 = y;
  }

	vec2 t[4] = {
			{s0, t0},
			{s1, t0},
			{s1, t1},
			{s0, t1}};

  for (int i = 0; i < 4; i++)
  {
    render->batch.verticesPtr[i] = vertexc(x + pos.v[i].x, y + pos.v[i].y, color, t[i].x, t[i].y);
  }

  render->batch.indexCount += 6;
  render->batch.verticesPtr += 4;
  render->state.currentTextureId = textureId;
}

TCDEF void tc_render_draw_circle(tc_render *render, unsigned int textureId, float x, float y, float radius, tc_color color) {
  if (render->batch.indexCount >= MAX_INDICES || render->state.currentTextureId != textureId)
    tc_reset_batch(render);

  if (render->state.drawMode == TC_TRIANGLES) {
    for (int i = 0; i < 360; i += 2) {
      *render->batch.verticesPtr = vertexc(x, y, color, 0, 0);
      render->batch.verticesPtr++;

      float rad = deg2rad(i);
      *render->batch.verticesPtr = vertexc(x + (cos(rad) * radius), y + (sin(rad) * radius), color, 1, 0);
      render->batch.verticesPtr++;

      rad = deg2rad(i + 1);
      *render->batch.verticesPtr = vertexc(x + (cos(rad) * radius), y + (sin(rad) * radius), color, 1, 1);
      render->batch.verticesPtr++;

      rad = deg2rad(i + 2);
      *render->batch.verticesPtr = vertexc(x + (cos(rad) * radius), y + (sin(rad) * radius), color, 0, 1);
      render->batch.verticesPtr++;

      render->batch.indexCount += 6;
    }
  } else {
    int numSides = 60;
    for (int i = 0; i < numSides; i++) {
      float theta = (i * PI * 2) / numSides;
      float rad = deg2rad(i);
      *render->batch.verticesPtr = vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      render->batch.verticesPtr++;

      theta = ((i+1) * PI * 2) / numSides;
      *render->batch.verticesPtr = vertexc(x + (cos(theta) * radius), y + (sin(theta) * radius), color, 0, 0);
      render->batch.verticesPtr++;

//       rad = deg2rad(i + 1);

      render->batch.indexCount += 2;
    }
  }


//   render->batch.indexCount += 180;
  render->state.currentTextureId = textureId;
}

TCDEF void tc_render_draw_triangle(tc_render *render, unsigned int textureId, float x0, float y0, float x1, float y1, float x2, float y2, tc_color color) {
  if (render->batch.indexCount >= MAX_INDICES || render->state.currentTextureId != textureId)
    tc_reset_batch(render);

    if (render->state.drawMode == TC_TRIANGLES) {
      *render->batch.verticesPtr = vertexc(x0, y0, color, 0, 0);
      render->batch.verticesPtr++;

      *render->batch.verticesPtr = vertexc(x1, y1, color, 0.5, 1);
      render->batch.verticesPtr++;

      *render->batch.verticesPtr = vertexc(x2, y2, color, 1, 0);
      render->batch.verticesPtr++;

      *render->batch.verticesPtr = vertexc(x1, y1, color, 0.5, 1);
      render->batch.verticesPtr++;

      render->batch.indexCount += 6;
    } else {
      *render->batch.verticesPtr = vertexc(x0, y0, color, 0, 0);
      render->batch.verticesPtr++;
      *render->batch.verticesPtr = vertexc(x1, y1, color, 0.5, 1);
      render->batch.verticesPtr++;

      *render->batch.verticesPtr = vertexc(x1, y1, color, 0.5, 1);
      render->batch.verticesPtr++;
      *render->batch.verticesPtr = vertexc(x2, y2, color, 1, 0);
      render->batch.verticesPtr++;

      *render->batch.verticesPtr = vertexc(x2, y2, color, 1, 0);
      render->batch.verticesPtr++;
      *render->batch.verticesPtr = vertexc(x0, y0, color, 0, 0);
      render->batch.verticesPtr++;

      render->batch.indexCount += 6;
    }

    render->state.currentTextureId = textureId;
}

#endif

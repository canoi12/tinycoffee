#ifndef TC_FONT_H
#define TC_FONT_H

#include "tinycoffee.h"

#include "font_data.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef struct {
	float ax; // advance.x
	float ay; // advance.y

	float bw; // bitmap.width;
	float bh; // bitmap.rows;

	float bl; // bitmap_left;
	float bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
} tc_character_info;

typedef struct {
    tc_texture texture;
    // FT_Face face;
    tc_character_info c[128];
    struct {
      float ax; // advance.x
      float ay; // advance.y

      float bw; // bitmap.width;
      float bh; // bitmap.rows;

      float bl; // bitmap_left;
      float bt; // bitmap_top;

      float tx; // x offset of glyph in texture coordinates
    } cInfo;

    int size;
    FT_Face face;

    unsigned int atlasVao;
    unsigned int atlasVbo;
} tc_font;

TCDEF void tc_init_font_lib();
TCDEF tc_font tc_load_default_font(void);
TCDEF tc_font tc_load_font(const char *filename, int size);
TCDEF tc_font tc_load_font_internal(const char *filename, int fontSize);
TCDEF tc_font tc_load_font_external(const char *filename, int fontSize);
TCDEF tc_font tc_load_font_from_memory(const char *buffer, size_t bufSize, int fontSize);
TCDEF tc_font tc_load_font_from_texture(tc_texture tex, int fontWidth, int fontHeight);
TCDEF void tc_unload_font(tc_font *font);

TCDEF void tc_font_get_rect(tc_font font, const char c, float *x, float *y, vec2 *outPos, tc_rectangle *rect);
TCDEF void tc_font_get_rect_scale(tc_font font, const char c, float *x, float *y, vec2 *outPos, tc_rectangle *rect, float sx, float sy);

#endif /* TC_FONT_H */

#if defined(TC_FONT_IMPLEMENTATION)

FT_Library ft;

static const char * data = "";

TCDEF void tc_init_font_lib() {
  if (FT_Init_FreeType(&ft))
	{
		ERROR("FONT", "Font lib init failed\n");
	}
}

TCDEF tc_font tc_load_default_font(void) {
  //tc_font font = tc_load_font("assets/basis33.ttf", 16);
  tc_font font = tc_load_font_from_memory(font_data[0].data, font_data[0].size, 16);
  return font;
}

TCDEF tc_font tc_load_font(const char *filename, int size) {
  // if (CORE.packed) {
  //   return tc_load_font_internal(filename, size);
  // } else {
  //   return tc_load_font_external(filename, size);
  // }
  size_t bufSize;
  unsigned char *buffer = tc_read_file(filename, &bufSize);
  tc_font font = tc_load_font_from_memory(buffer, bufSize, size);
  if (font.size == 0) {
  	ERROR("FONT", "Failed to load font '%s'\n", filename);
  }
  free(buffer);
  return font;
}

TCDEF tc_font tc_load_font_internal(const char *filename, int fontSize) {
  size_t size;
  unsigned char *buffer = tc_fs_read_file_from_zip("data.pack", filename, &size);

  return tc_load_font_from_memory(buffer, size, fontSize);
}
TCDEF tc_font tc_load_font_external(const char *filename, int fontSize) {
  size_t size;
  unsigned char *buffer = tc_fs_read_file(filename, &size, TC_TRUE);

  return tc_load_font_from_memory(buffer, size, fontSize);
//   tc_font font = {0};
// 	if (FT_New_Face(ft, filename, 0, &font.face))
// 	{
// 		ERROR("FONT", "Failed to load '%s'\n", filename);
// 		return font;
// 	}

// 	// FT_Select_Charmap(font.face, ft_encoding_unicode);

// 	FT_Set_Pixel_Sizes(font.face, 0, size);

// 	font.size = size;

// 	FT_GlyphSlot g = font.face->glyph;
// 	int w = 0;
// 	int h = 0;
// 	for (int i = 32; i < 128; i++)
// 	{
// 		if (FT_Load_Char(font.face, i, FT_LOAD_RENDER))
// 		{
// 			ERROR("FONT", "Failed to load character '%c' from '%s'", i, filename);
// 			continue;
// 		}

// 		w += g->bitmap.width;
// 		h = max(h, g->bitmap.rows);
// 	}

// 	font.texture = tc_create_texture(0, w, h);

// 	glBindTexture(GL_TEXTURE_2D, font.texture.id);
// 	int x = 0;
// 	int tsize = font.texture.width * font.texture.height * 4;
//   int channels = 4;
// 	for (int i = 32; i < 128; i++)
// 	{
// 		if (FT_Load_Char(font.face, i, FT_LOAD_RENDER))
// 			continue;
// 		int ww = g->bitmap.width;
// 		int hh = g->bitmap.rows;
// 		int ssize = ww * hh * channels;
// 		unsigned char pixel[ssize];
// 		for (int j = 0; j < ssize; j += channels)
// 		{
// 			int ii = j / channels;
// 			pixel[j] = 255;
// 			pixel[j + 1] = 255;
// 			pixel[j + 2] = 255;
// 			pixel[j + 3] = g->bitmap.buffer[ii];
//       // int ii = j/4;

// 		}

// 		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ww, hh, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

// 		font.c[i].ax = g->advance.x >> 6;
// 		font.c[i].ay = g->advance.y >> 6;
// 		font.c[i].bw = g->bitmap.width;
// 		font.c[i].bh = g->bitmap.rows;
// 		font.c[i].bl = g->bitmap_left;
// 		font.c[i].bt = g->bitmap_top;
// 		font.c[i].tx = (float)x / font.texture.width;

// 		x += g->bitmap.width;
// 	}

// 	// char *data = malloc(tsize);
// 	// glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
// 	// FILE *f = fopen("data.txt", "wb+");
// 	// for (int i = 0; i < tsize; i++) {
// 	// 	fputc(data[i], f);
// 	// }

// 	// fclose(f);
// 	// free(data);

// 	return font;
}

TCDEF tc_font tc_load_font_from_memory(const char *buffer, size_t bufSize, int fontSize) {
	tc_font font = {0};
	if (FT_New_Memory_Face(ft, buffer, bufSize, 0, &font.face))
	{
		ERROR("FONT", "Failed to load font from memory\n");
		return font;
	}

	FT_Set_Pixel_Sizes(font.face, 0, fontSize);

	font.size = fontSize;

	FT_GlyphSlot g = font.face->glyph;
	int w = 0;
	int h = 0;
	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(font.face, i, FT_LOAD_RENDER))
		{
			ERROR("FONT", "Failed to load character '%c' from memory\n", i);
			continue;
		}

		w += g->bitmap.width;
		h = max(h, g->bitmap.rows);
	}

	font.texture = tc_create_texture(0, w, h);

	glBindTexture(GL_TEXTURE_2D, font.texture.id);
	int x = 0;
	int tsize = font.texture.width * font.texture.height * 4;
  int channels = 4;
	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(font.face, i, FT_LOAD_RENDER))
			continue;
		int ww = g->bitmap.width;
		int hh = g->bitmap.rows;
		int ssize = ww * hh * channels;
		unsigned char pixel[ssize];
		for (int j = 0; j < ssize; j += channels)
		{
			int ii = j / channels;
			pixel[j] = 255;
			pixel[j + 1] = 255;
			pixel[j + 2] = 255;
			pixel[j + 3] = g->bitmap.buffer[ii];
      // int ii = j/4;

		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ww, hh, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

		font.c[i].ax = g->advance.x >> 6;
		font.c[i].ay = g->advance.y >> 6;
		font.c[i].bw = g->bitmap.width;
		font.c[i].bh = g->bitmap.rows;
		font.c[i].bl = g->bitmap_left;
		font.c[i].bt = g->bitmap_top;
		font.c[i].tx = (float)x / font.texture.width;

		x += g->bitmap.width;
	}

	return font;
}

TCDEF void tc_font_get_rect(tc_font font, const char c, float *x, float *y, vec2 *outPos, tc_rectangle *rect) {
	// // const char *p = c;
	// if (c >= 32 && c < 128)
	// {
	// 	float x2 = *x + font.c[c].bl;
	// 	float y2 = *y + (font.texture.height - font.c[c].bt);
	// 	float w = font.c[c].bw;
	// 	float h = font.c[c].bh;

	// 	float s0 = font.c[c].tx * font.texture.width;
	// 	float t0 = 0;
	// 	// float s1 = font.c[c].tx + font.c[c].bw;
	// 	float s1 = font.c[c].bw;
	// 	float t1 = font.c[c].bh;

	// 	// printf("%c %f\n", c, s1);

	// 	*x += font.c[c].ax;
	// 	*y += font.c[c].ay;
	// 	// if (!w || !h)
	// 	// 	continue;
	// 	if (outPos) {
	// 		outPos->x = x2;
	// 		outPos->y = y2;
	// 	}
	// 	*rect = (tc_rectangle){s0, t0, s1, t1};
	// 	return;
	// 	// tc_render_draw_quad(&CORE.render, font.texture.id, rect, x2, y2, font.texture.width, font.texture.height, color);
	// }
	tc_font_get_rect_scale(font, c, x, y, outPos, rect, 1, 1);
}

TCDEF void tc_font_get_rect_scale(tc_font font, const char c, float *x, float *y, vec2 *outPos, tc_rectangle *rect, float sx, float sy) {
	// const char *p = c;
	if (c == '\n') {
  	  *x = 0;
  	  *y += font.texture.height * sy;
  	  return;
  	}
	if (c >= 32 && c < 128)
	{

		float x2 = *x + font.c[c].bl;
		float y2 = *y + (font.texture.height - font.c[c].bt);
		float w = font.c[c].bw;
		float h = font.c[c].bh;

		float s0 = font.c[c].tx * font.texture.width;
		float t0 = 0;
		// float s1 = font.c[c].tx + font.c[c].bw;
		float s1 = font.c[c].bw;
		float t1 = font.c[c].bh;

		// printf("%c %f\n", c, s1);
    *x += font.c[c].ax * sx;
		*y += font.c[c].ay * sy;
		// if (!w || !h)
		// 	continue;
		if (outPos) {
			outPos->x = x2;
			outPos->y = y2;
		}
		*rect = (tc_rectangle){s0, t0, s1, t1};
		return;
		// tc_render_draw_quad(&CORE.render, font.texture.id, rect, x2, y2, font.texture.width, font.texture.height, color);
	}
}

#endif /* TC_FONT_IMPLEMENTATION */

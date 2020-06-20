#ifndef TICO_FONT_H
#define TICO_FONT_H

#include "tinycoffee.h"

#include "external/font_data.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H

#include "external/stb_truetype.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

#define MAXFONTCHAR 256

typedef struct {
	int ax; // advance.x
	int ay; // advance.y

	int bw; // bitmap.width;
	int bh; // bitmap.rows;

	int bl; // bitmap_left;
	int bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
} tc_CharacterInfo;

typedef struct tc_Font {
    tc_Texture texture;
    // FT_Face face;
    tc_CharacterInfo c[MAXFONTCHAR];

    tc_uint8 size;

    stbtt_fontinfo info;
    float ptsize;
    float scale;
    int baseline;
    void *data;

    unsigned int atlasVao;
    unsigned int atlasVbo;
} tc_Font;

// TCDEF void tc_init_font_lib();
TCDEF tc_Font tc_load_default_font(void);
TCDEF void tc_init_font(tc_Font *font, const void *data, size_t bufSize, tc_uint8 fontSize);
TCDEF tc_Font tc_load_font(const tc_uint8 *filename, tc_uint8 size);
TCDEF tc_Font tc_load_internal_font(const tc_uint8 *filename, tc_uint8 fontSize);
TCDEF tc_Font tc_load_external_font(const tc_uint8 *filename, tc_uint8 fontSize);
TCDEF tc_Font tc_load_font_from_memory(const void *data, size_t bufSize, tc_uint8 fontSize);
TCDEF tc_Font tc_load_font_from_texture(tc_Texture tex, int fontWidth, int fontHeight);
TCDEF void tc_destroy_font(tc_Font *font);

TCDEF void tc_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width);
TCDEF void tc_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width);

TCDEF int tc_font_get_text_width(tc_Font font, const tc_uint8 *text, int len);
TCDEF int tc_font_get_text_height(tc_Font font, const tc_uint8 *text, int len);

#endif /* TICO_FONT_H */

#if defined(TICO_FONT_IMPLEMENTATION)

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

// TCDEF void tc_init_font_lib() {

// }

TCDEF tc_Font tc_load_default_font(void) {
  tc_Font font = tc_load_font_from_memory(font_data[0].data, font_data[0].size, 16);
  return font;
}

TCDEF void tc_init_font(tc_Font *font, const void *data, size_t bufSize, tc_uint8 fontSize) {
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
    th = tc_max(th, h);
  }

  font->texture = tc_create_texture(NULL, tw, th);

  glBindTexture(GL_TEXTURE_2D, font->texture.id);
  int x = 0;
  for (int i = 0; i < MAXFONTCHAR; i++) {
    int ww = font->c[i].bw;
    int hh = font->c[i].bh;
    int ssize = ww * hh * 4;
    int ox, oy;

    tc_uint8 *bitmap = stbtt_GetCodepointBitmap(&font->info, 0, font->scale, i, NULL, NULL, &ox, &oy);

    tc_uint8 pixels[ssize];
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

TCDEF tc_Font tc_load_font(const tc_uint8 *filename, tc_uint8 size) {
  size_t bufSize;
  tc_uint8 *buffer = tc_read_file(filename, &bufSize);
  tc_Font font = tc_load_font_from_memory(buffer, bufSize, size);
  if (font.size == 0) {
  	TRACEERR("Failed to load font '%s'", filename);
  }
  free(buffer);
  return font;
}

TCDEF tc_Font tc_load_internal_font(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tc_read_internal_file(filename, &size);

  return tc_load_font_from_memory(buffer, size, fontSize);
}
TCDEF tc_Font tc_load_external_font(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tc_read_external_file(filename, &size);


  return tc_load_font_from_memory(buffer, size, fontSize);
}

TCDEF tc_Font tc_load_font_from_memory(const void *data, size_t bufSize, tc_uint8 fontSize) {
	tc_Font font = {0};

  tc_init_font(&font, data, bufSize, fontSize);
	return font;
}

TCDEF void tc_destroy_font(tc_Font *font) {
  tc_destroy_texture(&font->texture);
  free(font->data);
}

TCDEF void tc_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width) {
	tc_font_get_rect_scale(font, c, x, y, outPos, rect, 1, 1, width);
}

TCDEF void tc_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width) {
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
	// 	continue;
	if (outPos) {
		outPos->x = x2;
		outPos->y = y2;
	}
	if (rect) *rect = (tc_Rectf){s0, t0, s1, t1};

	return;
}

TCDEF int tc_font_get_text_width(tc_Font font, const tc_uint8 *text, int len) {
  tc_uint8 *p = (tc_uint8*)text;
  int width = 0;
  int max_width = 0;
  while (*p && len--) {
    if (*p != '\n') {
      int codepoint;
      p = tc_utf8_codepoint(p, &codepoint);
      width += font.c[codepoint].ax;
    } else {
      max_width = tc_max(width, max_width);
      width = 0;
    }
  }
  max_width = tc_max(width, max_width);

  return max_width;
}

TCDEF int tc_font_get_text_height(tc_Font font, const tc_uint8 *text, int len) {
  tc_uint8 *p = (tc_uint8*)text;
  int height = font.texture.height;
  while (*p) {
    if (*p == '\n') height += font.texture.height;
  }

  return height;
}

#endif /* TC_FONT_IMPLEMENTATION */


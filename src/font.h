#ifndef TC_FONT_H
#define TC_FONT_H

#include "tinycoffee.h"

#include "font_data.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H

#include "external/stb_truetype.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

#define MAXFONTCHAR 256

typedef struct {
	tc_int16 ax; // advance.x
	tc_int16 ay; // advance.y

	tc_int16 bw; // bitmap.width;
	tc_int16 bh; // bitmap.rows;

	tc_int16 bl; // bitmap_left;
	tc_int16 bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
} tc_character_info;

typedef struct {
    tc_texture texture;
    // FT_Face face;
    tc_character_info c[MAXFONTCHAR];

    tc_uint8 size;

    stbtt_fontinfo info;
    float ptsize;
    float scale;
    tc_uint16 baseline;
    void *data;

    unsigned int atlasVao;
    unsigned int atlasVbo;
} tc_font;

TCDEF void tc_init_font_lib();
TCDEF tc_font tc_load_default_font(void);
TCDEF tc_font tc_load_font(const tc_uint8 *filename, tc_uint8 size);
TCDEF tc_font tc_load_font_internal(const tc_uint8 *filename, tc_uint8 fontSize);
TCDEF tc_font tc_load_font_external(const tc_uint8 *filename, tc_uint8 fontSize);
TCDEF tc_font tc_load_font_from_memory(const tc_uint8 *buffer, size_t bufSize, tc_uint8 fontSize);
TCDEF tc_font tc_load_font_from_texture(tc_texture tex, tc_uint16 fontWidth, tc_uint16 fontHeight);
TCDEF void tc_unload_font(tc_font *font);

TCDEF void tc_font_get_rect(tc_font font, const tc_uint16 c, float *x, float *y, vec2 *outPos, tc_rectangle *rect);
TCDEF void tc_font_get_rect_scale(tc_font font, const tc_uint16 c, float *x, float *y, vec2 *outPos, tc_rectangle *rect, float sx, float sy);

#endif /* TC_FONT_H */

#if defined(TC_FONT_IMPLEMENTATION)

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

static const tc_uint8 * data = "";

TCDEF void tc_init_font_lib() {

}

TCDEF tc_font tc_load_default_font(void) {
  tc_font font = tc_load_font_from_memory(font_data[0].data, font_data[0].size, 16);
  return font;
}

TCDEF tc_font tc_load_font(const tc_uint8 *filename, tc_uint8 size) {
  size_t bufSize;
  tc_uint8 *buffer = tc_read_file(filename, &bufSize);
  tc_font font = tc_load_font_from_memory(buffer, bufSize, size);
  if (font.size == 0) {
  	ERROR("Failed to load font '%s'", filename);
  }
  free(buffer);
  return font;
}

TCDEF tc_font tc_load_font_internal(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tc_fs_read_file_from_zip("data.pack", filename, &size);

  return tc_load_font_from_memory(buffer, size, fontSize);
}
TCDEF tc_font tc_load_font_external(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tc_fs_read_file(filename, &size, TC_TRUE);

  return tc_load_font_from_memory(buffer, size, fontSize);
}

TCDEF tc_font tc_load_font_from_memory(const tc_uint8 *buffer, size_t bufSize, tc_uint8 fontSize) {
	tc_font font = {0};
  font.data = malloc(bufSize);
	memcpy(font.data, buffer, bufSize);

	if (!stbtt_InitFont(&font.info, font.data, 0)) {
	  ERROR("failed to init font");
    return font;
	}

	int ascent, descent, lineGap;
	font.size = fontSize;
  float fsize = fontSize;
	font.scale = stbtt_ScaleForMappingEmToPixels(&font.info, fsize);
	stbtt_GetFontVMetrics(&font.info, (int*)&ascent, (int*)&descent, (int*)&lineGap);
	font.baseline = ascent * font.scale;

  int tw = 0;
  int th = 0;

  for (int i = 0; i < MAXFONTCHAR; i++) {
    tc_int32 ax;
    tc_int32 bl;
    tc_int32 x0, y0, x1, y1;
    tc_int32 w;
    tc_int32 h;

    stbtt_GetCodepointHMetrics(&font.info, i, &ax, &bl);
    stbtt_GetCodepointBitmapBox(&font.info, i, font.scale, font.scale, &x0, &y0, &x1, &y1);
    w = x1-x0;
    h = y1-y0;

    font.c[i].ax = ax * font.scale;
    font.c[i].ay = 0;
    font.c[i].bl = bl * font.scale;
    font.c[i].bw = w;
    font.c[i].bh = h;
    font.c[i].bt = font.baseline + y0;

    tw += w;
    th = max(th, h);
  }

  font.texture = tc_create_texture(NULL, tw, th);

  glBindTexture(GL_TEXTURE_2D, font.texture.id);
  int x = 0;
  for (int i = 0; i < MAXFONTCHAR; i++) {
    tc_int32 ww = font.c[i].bw;
    tc_int32 hh = font.c[i].bh;
    tc_int32 ssize = ww * hh * 4;
    tc_int32 ox, oy;

    tc_uint8 *bitmap = stbtt_GetCodepointBitmap(&font.info, 0, font.scale, i, NULL, NULL, &ox, &oy);

    tc_uint8 pixels[ssize];
    for (int j = 0; j < ssize; j += 4) {
      int ii = j / 4;
      pixels[j] = 255;
      pixels[j + 1] = 255;
      pixels[j + 2] = 255;
      pixels[j + 3] = bitmap[ii];
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ww, hh, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    font.c[i].tx = (float)x / tw;

    x += font.c[i].bw;
  }

  glBindTexture(GL_TEXTURE_2D, 0);


	return font;
}

TCDEF void tc_font_get_rect(tc_font font, const tc_uint16 c, float *x, float *y, vec2 *outPos, tc_rectangle *rect) {
	tc_font_get_rect_scale(font, c, x, y, outPos, rect, 1, 1);
}

TCDEF void tc_font_get_rect_scale(tc_font font, const tc_uint16 c, float *x, float *y, vec2 *outPos, tc_rectangle *rect, float sx, float sy) {
	// const char *p = c;
	if (c == '\n') {
	  *x = 0;
	  *y += font.texture.height * sy;
	  return;
	}

	float x2 = *x + (font.c[c].bl*sx);
  float y2 = *y + (font.c[c].bt*sy);
	float w = font.c[c].bw;
	float h = font.c[c].bh;

	float s0 = font.c[c].tx * font.texture.width;
	float t0 = 0;
	float s1 = font.c[c].bw;
	float t1 = font.c[c].bh;

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
}

#endif /* TC_FONT_IMPLEMENTATION */

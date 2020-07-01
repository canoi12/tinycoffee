#include "tico.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

#include "external/font_data.h"

tc_Font tic_font_load_default(void) {
  tc_Font font = tic_font_load_from_memory(font_data[0].data, font_data[0].size, 16);
  return font;
}

void tic_font_init(tc_Font *font, const void *data, size_t bufSize, int fontSize) {
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
    th = tic_max(th, h);
  }

  font->texture = tic_texture_create(NULL, tw, th, GL_RGBA);

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

tc_Font tic_font_load(const tc_uint8 *filename, int size) {
  // tc_Font *font = tic_resources_get_font(filename);
  // if (font) {
  //   return *font;
  // }
  size_t bufSize;
  tc_uint8 *buffer = tic_filesystem_read_file(filename, &bufSize);
  // font = malloc(sizeof(tc_Font));
  tc_Font font = tic_font_load_from_memory(buffer, bufSize, size);
  if (font.size == 0) {
    // free(font);
  	TRACEERR("Failed to load font '%s'", filename);
  	return (tc_Font){0};
  }
  // tic_resources_add_font(filename, font);
  free(buffer);
  return font;
}

tc_Font tic_font_load_internal(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tic_filesystem_read_internal_file(filename, &size);

  return tic_font_load_from_memory(buffer, size, fontSize);
}
tc_Font tic_font_load_external(const tc_uint8 *filename, tc_uint8 fontSize) {
  size_t size;
  tc_uint8 *buffer = tic_filesystem_read_external_file(filename, &size);


  return tic_font_load_from_memory(buffer, size, fontSize);
}

tc_Font tic_font_load_from_memory(const void *data, size_t bufSize, int fontSize) {
	tc_Font font = {0};

  tic_font_init(&font, data, bufSize, fontSize);
	return font;
}

void tic_font_destroy(tc_Font *font) {
  tic_texture_destroy(&font->texture);
  free(font->data);
}

void tic_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width) {
	tic_font_get_rect_scale(font, c, x, y, outPos, rect, 1, 1, width);
}

void tic_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width) {
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

int tic_font_get_text_width(tc_Font font, const tc_uint8 *text, int len) {
  tc_uint8 *p = (tc_uint8*)text;
  int width = 0;
  int max_width = 0;
  while (*p && len--) {
    if (*p != '\n') {
      int codepoint;
      p = tic_utf8_codepoint(p, &codepoint);
      width += font.c[codepoint].ax;
    } else {
      max_width = tic_max(width, max_width);
      width = 0;
      p++;
    }
  }
  max_width = tic_max(width, max_width);

  return max_width;
}

int tic_font_get_text_height(tc_Font font, const tc_uint8 *text, int len) {
  tc_uint8 *p = (tc_uint8*)text;
  int height = font.texture.height;
  while (*p) {
    if (*p == '\n') height += font.texture.height;
  }

  return height;
}
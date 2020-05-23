#include "module.h"

static const char *tcWrenGraphicsModule =
"class Render {\n"
" foreign static clear()\n"
" foreign static clear(color)\n"
" foreign static drawRectangle(x, y, w, h)\n"
" foreign static drawRectangle(x, y, w, h, color)\n"
" foreign static drawText(text, x, y, color)\n"
" foreign static drawText(text, x, y, sx, sy, color)\n"
"}\n"
"foreign class ColorF {\n"
" construct new(r,g,b) {}\n"
" construct new(r,g,b,a) {}\n"
" foreign r\n"
" foreign g\n"
" foreign b\n"
" foreign a\n"
"}\n"
"class Color {\n"
" static load() {\n"
"  __white = ColorF.new(255, 255, 255)\n"
"  __black = ColorF.new(0, 0, 0)\n"
"  __red = ColorF.new(255, 0, 0)\n"
" __bg = ColorF.new(75, 90, 90)\n"
"}\n"
" static new(r,g,b) { ColorF.new(r,g,b) }\n"
" static new(r,g,b,a) { ColorF.new(r,g,b,a)}\n"
" static White { __white }\n"
" static Black { __black }\n"
" static Red { __red }\n"
" static BG { __bg }\n"
"}\n"
"Color.load()\n"
"foreign class Texture {\n"
" construct create(data, width, height) {}\n"
" construct load(filename) {}\n"
" foreign width\n"
" foreign height\n"
" foreign draw(x, y, color)\n"
" foreign draw(x, y, sx, sy, color)\n"
" foreign draw(x, y, angle, sx, sy, cx, cy, color)\n"
" foreign drawPart(rect, x, y, color)\n"
" foreign drawPart(rect, x, y, sx, sy, color)\n"
" foreign drawPart(rect, x, y, angle, sx, sy, cx, cy, color)\n"
"}\n"
"foreign class Canvas {\n"
" construct new(width, height) {}\n"
" foreign width\n"
" foreign height\n"
" foreign set()\n"
" foreign unset()\n"
" foreign draw(x, y, color)\n"
" foreign draw(x, y, sx, sy, color)\n"
"}\n";
// "foreign class Shader {\n"
// " construct create(vertexSource, fragmentSource) {}\n"
// " construct load(vertexSource, fragmentSource) {}\n"
// " foreign set()\n"
// " foreign unset()\n"
// "]\n";

/* Graphics module */

static void wren_graphics_clear(WrenVM* vm) {
  tc_color *color = &BLACK;
  if (wrenGetSlotCount(vm) > 1) color = wrenGetSlotForeign(vm, 1);

  tc_clear(*color);
}

static void wren_draw_rectangle(WrenVM *vm) {
  wrenInitSlot(0);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float width = wrenGetSlotDouble(vm, nextSlot());
  float height = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);
  // wrenEnsureSlots(vm, 0);

  tc_draw_rectangle(x, y, width, height, *color);
}

static void wren_draw_text(WrenVM *vm) {
  wrenInitSlot(0);
  const char *text = wrenGetSlotString(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);
  // wrenEnsureSlots(vm, 0);

  tc_draw_text(text, x, y, *color);
}

static void wren_draw_text_scale(WrenVM *vm) {
  wrenInitSlot(0);
  const char *text = wrenGetSlotString(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);
  // wrenEnsureSlots(vm, 0);

  tc_draw_text_scale(text, x, y, sx, sy, *color);
}

tc_wren_lib wrenRenderLib[] = {
  {"s_clear()", wren_graphics_clear},
  {"s_clear(_)", wren_graphics_clear},
  {"s_drawRectangle(_,_,_,_)", wren_draw_rectangle},
  {"s_drawRectangle(_,_,_,_,_)", wren_draw_rectangle},
  {"s_drawText(_,_,_,_)", wren_draw_text},
  {"s_drawText(_,_,_,_,_,_)", wren_draw_text_scale}
};

/* Texture class */

static void wren_texture_allocate(WrenVM *vm) {
  tc_texture *tex = (tc_texture*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_texture));
  if (wrenGetSlotCount(vm) < 3) {
    const char *filename = wrenGetSlotString(vm, 1);
    *tex = tc_load_texture(filename);
  } else {
    void *data = (void *)wrenGetSlotString(vm, 1);
    int width = wrenGetSlotDouble(vm, 2);
    int height = wrenGetSlotDouble(vm, 3);
    *tex = tc_create_texture(data, width, height);
  }
}

static void wren_texture_finalize(void *data) {
  tc_texture *tex = (tc_texture*)data;
  tc_delete_texture(tex);
  free(tex);
}

static void wren_texture_draw(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm ,nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  // tc_color color = WHITE;
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture(*tex, x, y, *color);
}

static void wren_texture_draw_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture_scale(*tex, x, y, sx, sy, *color);
}

static void wren_texture_draw_ex(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float angle = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  float cx = wrenGetSlotDouble(vm, nextSlot());
  float cy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture_ex(*tex, x, y, angle, sx, sy, cx, cy, *color);
}

static void wren_texture_draw_part(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  tc_rectangle rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture_part(*tex, rect, x, y, *color);
}

static void wren_texture_draw_part_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  tc_rectangle rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture_part_scale(*tex, rect, x, y, sx, sy, *color);
}

static void wren_texture_draw_part_ex(WrenVM *vm) {
  wrenInitSlot(0);
  tc_texture *tex = wrenGetSlotForeign(vm, slot);
  tc_rectangle rect = wrenGetSlotRectList(vm, nextSlot());
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float angle = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  float cx = wrenGetSlotDouble(vm, nextSlot());
  float cy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_texture_part_ex(*tex, rect, x, y, angle, sx, sy, cx, cy, *color);
}

static void wren_texture_width(WrenVM *vm) {
  tc_texture *tex = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, tex->width);
}

static void wren_texture_height(WrenVM *vm) {
  tc_texture *tex = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, tex->height);
}

static tc_wren_lib wrenTextureLib[] = {
  {"draw(_,_,_)", wren_texture_draw},
  {"draw(_,_,_,_,_)", wren_texture_draw_scale},
  {"draw(_,_,_,_,_,_,_,_)", wren_texture_draw_ex},
  {"drawPart(_,_,_,_)", wren_texture_draw_part},
  {"drawPart(_,_,_,_,_,_)", wren_texture_draw_part_scale},
  {"drawPart(_,_,_,_,_,_,_,_,_)", wren_texture_draw_part_ex},
  {"width", wren_texture_width},
  {"height", wren_texture_height}
};

/*******************
 * Canvas class
 *******************/

static void wren_canvas_allocate(WrenVM *vm) {
  tc_canvas *canvas = (tc_canvas*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_canvas));
  float width = wrenGetSlotDouble(vm, 1);
  float height = wrenGetSlotDouble(vm, 2);
  *canvas = tc_create_canvas(width, height);
}
static void wren_canvas_finalize(void *data) {
  tc_canvas *canvas = data;
  tc_delete_canvas(canvas);
  free(canvas);
}

static void wren_canvas_set(WrenVM* vm) {
  tc_canvas *canvas = wrenGetSlotForeign(vm, 0);
  tc_set_canvas(*canvas);
}
static void wren_canvas_unset(WrenVM* vm) {
  tc_unset_canvas();
}

static void wren_canvas_width(WrenVM *vm) {
  tc_canvas *canvas = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, canvas->tex.width);
}
static void wren_canvas_height(WrenVM *vm) {
  tc_canvas *canvas = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, canvas->tex.height);
}

static void wren_canvas_draw(WrenVM *vm) {
  wrenInitSlot(0);
  tc_canvas *canvas = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_canvas(*canvas, x, y, *color);
}
static void wren_canvas_draw_scale(WrenVM *vm) {
  wrenInitSlot(0);
  tc_canvas *canvas = wrenGetSlotForeign(vm, slot);
  float x = wrenGetSlotDouble(vm, nextSlot());
  float y = wrenGetSlotDouble(vm, nextSlot());
  float sx = wrenGetSlotDouble(vm, nextSlot());
  float sy = wrenGetSlotDouble(vm, nextSlot());
  tc_color *color = &WHITE;
  if (wrenGetSlotCount(vm) > nextSlot()) color = wrenGetSlotForeign(vm, slot);

  tc_draw_canvas_scale(*canvas, x, y, sx, sy, *color);
}

static tc_wren_lib wrenCanvasLib[] = {
  {"set()", wren_canvas_set},
  {"unset()", wren_canvas_unset},
  {"width", wren_canvas_width},
  {"height", wren_canvas_height},
  {"draw(_,_,_)", wren_canvas_draw},
  {"draw(_,_,_,_,_)", wren_canvas_draw_scale}
};

/* Color class */

static void wren_color_allocate(WrenVM *vm) {
  tc_color *color = (tc_color*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_color));
  *color = WHITE;
  if (wrenGetSlotCount(vm) < 5) {
    color->r = wrenGetSlotDouble(vm, 1);
    color->g = wrenGetSlotDouble(vm, 2);
    color->b = wrenGetSlotDouble(vm, 3);
  } else {
    color->r = wrenGetSlotDouble(vm, 1);
    color->g = wrenGetSlotDouble(vm, 2);
    color->b = wrenGetSlotDouble(vm, 3);
    color->a = wrenGetSlotDouble(vm, 4);
  }
}

static void wren_color_finalize(void *data) {
  free(data);
}

static void wren_color_r(WrenVM *vm) {
  tc_color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->r);
}

static void wren_color_g(WrenVM *vm) {
  tc_color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->g);
}

static void wren_color_b(WrenVM *vm) {
  tc_color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->b);
}

static void wren_color_a(WrenVM *vm) {
  tc_color *color = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, color->a);
}


static tc_wren_lib wrenColorLib[] = {
  {"r", wren_color_r},
  {"g", wren_color_g},
  {"b", wren_color_b},
  {"a", wren_color_a}
};

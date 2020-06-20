#ifndef TICO_LUA_GRAPHICS_H
#define TICO_LUA_GRAPHICS_H

#include "../../tinycoffee.h"

#define TEXTURE_CLASS "Texture"
#define CANVAS_CLASS "Canvas"
#define IMAGE_CLASS "Image"
#define RECTANGLE_CLASS "Rectangle"
#define FONT_CLASS "Font"

static const char *lua_Color =
"local color = {}\n"
"color.White = {255, 255, 255}\n"
"color.Black = {0, 0, 0}\n"
"color.Red = {208, 70, 72}\n"
"function color.rgb(r, g, b) return {r, g, b} end\n"
"return color";


TCDEF int luaopen_graphics(lua_State *L);

#endif // TICO_LUA_GRAPHICS_H

#if defined(TICO_LUA_IMPLEMENTATION)

static tc_Color lua_optcolor(lua_State *l, int index, tc_Color opt) {
  tc_Color color = opt;
  if (lua_istable(l, index)) {
    int i = 0;
    lua_pushnil(l);
    while(lua_next(l, -2)) {
      color.data[i] = lua_tonumber(l, -1);
      lua_pop(l, 1);
      ++i;
    }
  }

  return color;
}

static tc_Rectf lua_optrectf(lua_State *l, int index, tc_Rectf opt) {
  tc_Rectf rect = opt;
  tc_Rectf *ur = luaL_testudata(l, index, RECTANGLE_CLASS);
  if (ur) {
    rect = *ur;
  }
  return rect;
}

/************************
 * Shapes
 ************************/

static int tc_lua_draw_rectangle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int w = luaL_checknumber(L, 3);
  int h = luaL_checknumber(L, 4);
  tc_Color color = lua_optcolor(L, 5, WHITE);

  tc_draw_rectangle(x, y, w, h, color);
  return 0;
}
static int tc_lua_fill_rectangle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int w = luaL_checknumber(L, 3);
  int h = luaL_checknumber(L, 4);
  tc_Color color = lua_optcolor(L, 5, WHITE);

  tc_fill_rectangle(x, y, w, h, color);
  return 0;
}

static int tc_lua_draw_circle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int radius = luaL_checknumber(L, 3);
  tc_Color color = lua_optcolor(L, 4, WHITE);

  tc_draw_circle(x, y, radius, color);
  return 0;
}
static int tc_lua_fill_circle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int radius = luaL_checknumber(L, 3);
  tc_Color color = lua_optcolor(L, 4, WHITE);

  tc_fill_circle(x, y, radius, color);
  return 0;
}
static int tc_lua_draw_triangle(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);
  int x2 = luaL_checknumber(L, 5);
  int y2 = luaL_checknumber(L, 6);

  tc_Color color = lua_optcolor(L, 7, WHITE);

  tc_draw_triangle(x0, y0, x1, y1, x2, y2, color);
  return 0;
}
static int tc_lua_fill_triangle(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);
  int x2 = luaL_checknumber(L, 5);
  int y2 = luaL_checknumber(L, 6);

  tc_Color color = lua_optcolor(L, 7, WHITE);

  tc_fill_triangle(x0, y0, x1, y1, x2, y2, color);
  return 0;
}

static int tc_lua_draw_line(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);

  tc_Color color = lua_optcolor(L, 5, WHITE);
  tc_draw_line(x0, y0, x1, y1, color);
  return 0;
}

/*********************
 * Text
 *********************/

 static int tc_lua_new_font(lua_State *L) {
   const char *filename = luaL_checkstring(L, 1);
   int size = luaL_optinteger(L, 2, 16);
   tc_Font *font = lua_newuserdata(L, sizeof(*font));
   luaL_setmetatable(L, FONT_CLASS);

   *font = tc_load_font(filename, size);
   return 1;
 }

 static int tc_lua_destroy_font(lua_State *L) {
   tc_Font *font = luaL_checkudata(L, 1, FONT_CLASS);
   tc_destroy_font(font);
   return 0;
 }

 static int tc_lua_font_text_width(lua_State *L) {
   tc_Font *font = luaL_checkudata(L, 1, FONT_CLASS);
   const char *text = luaL_checkstring(L, 2);
   int width = tc_font_get_text_width(*font, text, strlen(text));
   lua_pushinteger(L, width);
   return 1;
 }
 static int tc_lua_font_text_height(lua_State *L) {}

static int tc_lua_print(lua_State *L) {
  int index = 0;
  tc_Font *font = &Core.defaultFont;
  if (lua_isuserdata(L, 1)) {
    font = luaL_checkudata(L, ++index, FONT_CLASS);
  }
  const char *text = luaL_checkstring(L, ++index);
  int x = luaL_optnumber(L, ++index, 0);
  int y = luaL_optnumber(L, ++index, 0);
  float sx = 1;
  float sy = 1;
//   int index = 3;
  if (lua_type(L, index+1) == LUA_TNUMBER) {
    sx = luaL_optnumber(L, ++index, 1);
    sy = luaL_optnumber(L, ++index, 1);
  }
  tc_Color color = lua_optcolor(L, ++index, WHITE);

  tc_draw_text_font_scale(*font, text, x, y, sx, sy, color);
}

static int tc_lua_printf(lua_State *L) {
  int index = 0;
  tc_Font *font = &Core.defaultFont;
  if (lua_isuserdata(L, 1)) {
    font = luaL_checkudata(L, ++index, FONT_CLASS);
  }
  const char *text = luaL_checkstring(L, ++index);
  int x = luaL_optnumber(L, ++index, 0);
  int y = luaL_optnumber(L, ++index, 0);
  int width = luaL_optnumber(L, ++index, 0);
  float sx = 1;
  float sy = 1;
//   int index = 4;
  if (lua_type(L, index+1) == LUA_TNUMBER) {
    sx = luaL_optnumber(L, ++index, 1);
    sy = luaL_optnumber(L, ++index, 1);
  }
  tc_Color color = lua_optcolor(L, ++index, WHITE);

  tc_draw_text_font_scale_width(*font, text, x, y, width, 1, 1, color);
}

static int luaopen_font(lua_State *L) {
  luaL_Reg reg[] = {
    {"__call", tc_lua_new_font},
    {"__gc", tc_lua_destroy_font},
    {"textWidth", tc_lua_font_text_width},
    {"textHeight", tc_lua_font_text_height},
    {NULL, NULL}
  };

  luaL_newmetatable(L, FONT_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}

/*********************
 * Image
 *********************/

static int tc_lua_image_height(lua_State *L) {
  tc_Image *tex = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, tex->height);

  return 1;
}
static int tc_lua_image_width(lua_State *L) {
  tc_Image *tex = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, tex->width);

  return 1;
}

static int tc_lua_new_image(lua_State *L) {
  tc_Image *tex = (tc_Image*)lua_newuserdata(L, sizeof(tc_Image));
  luaL_setmetatable(L, IMAGE_CLASS);
  const char *path = luaL_checkstring(L, 1);
  *tex = tc_load_image(path);
  return 1;
}

static int tc_lua_draw_image(lua_State *L) {
  int index = 0;
  tc_Image *image = luaL_checkudata(L, ++index, IMAGE_CLASS);
  tc_Rectf rect = tc_rectf(0, 0, image->width, image->height);
  tc_Color color = WHITE;
  if (luaL_testudata(L, 2, RECTANGLE_CLASS)) {
    rect = lua_optrectf(L, ++index, rect);
  }
  // exit(-1);
  int x = luaL_optnumber(L, ++index, 0);
  int y = luaL_optnumber(L, ++index, 0);
  if (lua_type(L, index+1) != LUA_TNUMBER) {
    color = lua_optcolor(L, ++index, WHITE);
    tc_draw_image_part(*image, rect, x, y, color);
    return 0;
  }
  float angle = luaL_optnumber(L, ++index, 0);
  float sx = luaL_optnumber(L, ++index, 1);
  float sy = luaL_optnumber(L, ++index, 1);
  int cx = luaL_optnumber(L, ++index, 0);
  int cy = luaL_optnumber(L, ++index, 0);
  color = lua_optcolor(L, ++index, WHITE);
  tc_draw_image_part_ex(*image, rect, x, y, angle, sx, sy, cx, cy, color);
  return 0;
}

static int tc_lua_destroy_image(lua_State *L) {
  tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
  tc_destroy_image(image);
  return 0;
}

static int tc_lua_image_dimensions(lua_State *L) {
  tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, image->width);
  lua_pushnumber(L, image->height);
  return 2;
}

static int luaopen_image(lua_State *L) {
//   luaL_Reg methods[] = {
//     {"new", tc_lua_new_image},
//     {NULL, NULL}
//   };
//   luaL_newlib(L, methods);

  luaL_Reg reg[] = {
    {"width", tc_lua_image_width},
    {"height",  tc_lua_image_height},
    {"draw", tc_lua_draw_image},
    {"getDimensions", tc_lua_image_dimensions},
    {"__gc", tc_lua_destroy_image},
    {NULL, NULL}
  };

  luaL_newmetatable(L, IMAGE_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
//   luaL_openlib(L, NULL, reg, 0);

  return 1;
}

/********************
 * Canvas
 ********************/

static int tc_lua_new_canvas(lua_State *L) {
  int width = luaL_checknumber(L, 1);
  int height = luaL_checknumber(L, 2);
  tc_Canvas *canvas = lua_newuserdata(L, sizeof(*canvas));
  luaL_setmetatable(L, CANVAS_CLASS);
  *canvas = tc_create_canvas(width, height);

  return 1;
}

static int tc_lua_set_canvas(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tc_attach_canvas(*canvas);

  return 0;
}

static int tc_lua_unset_canvas(lua_State *L) {
  tc_detach_canvas();
  return 0;
}

static int tc_lua_draw_canvas(lua_State *L) {
  int index = 0;
  tc_Canvas *canvas = luaL_checkudata(L, ++index, CANVAS_CLASS);
  tc_Rectf rect = tc_rectf(0, 0, canvas->width, canvas->height);
  tc_Color color = WHITE;
  if (luaL_testudata(L, 2, RECTANGLE_CLASS)) {
    rect = lua_optrectf(L, ++index, rect);
  }

  int x = luaL_optnumber(L, ++index, 0);
  int y = luaL_optnumber(L, ++index, 0);
  if (lua_type(L, index+1) != LUA_TNUMBER) {
    color = lua_optcolor(L, ++index, WHITE);
    tc_draw_canvas_part(*canvas, rect, x, y, color);
    return 0;
  }
  float angle = luaL_optnumber(L, ++index, 0);
  float sx = luaL_optnumber(L, ++index, 1);
  float sy = luaL_optnumber(L, ++index, 1);
  int cx = luaL_optnumber(L, ++index, 0);
  int cy = luaL_optnumber(L, ++index, 0);
  color = lua_optcolor(L, ++index, WHITE);

  tc_draw_canvas_part_ex(*canvas, rect, x, y, angle, sx, sy, cx, cy, color);
  return 0;
}

static int tc_lua_canvas_auto(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tc_canvas_auto(*canvas);
  return 0;
}

static int tc_lua_destroy_canvas(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tc_destroy_canvas(canvas);
  return 0;
}

static int tc_lua_canvas_width(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  lua_pushinteger(L, canvas->width);
  return 1;
}

static int tc_lua_canvas_height(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  lua_pushinteger(L, canvas->height);
  return 1;
}

static int tc_lua_canvas_size(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  lua_pushinteger(L, canvas->width);
  lua_pushinteger(L, canvas->height);
  return 2;
}

static int luaopen_canvas(lua_State *L) {
//   luaL_Reg methods[] = {
//     {"new", tc_lua_new_canvas},
//     {NULL, NULL}
//   };
//   luaL_newlib(L, methods);

  luaL_Reg reg[] = {
    {"attach", tc_lua_set_canvas},
    {"detach", tc_lua_unset_canvas},
    {"draw", tc_lua_draw_canvas},
    {"auto", tc_lua_canvas_auto},
    {"getWidth", tc_lua_canvas_width},
    {"getHeight", tc_lua_canvas_height},
    {"getSize", tc_lua_canvas_size},
    {"__gc", tc_lua_destroy_canvas},
    {NULL, NULL}
  };
  luaL_newmetatable(L, CANVAS_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
//   luaL_openlib(L, NULL, reg, 0);
//   lua_setfield(L, -2, "object");
  return 1;
}

/********************
 * Rectangle
 ********************/

static int tc_lua_new_rectangle(lua_State *L) {
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  float w = luaL_checknumber(L, 3);
  float h = luaL_checknumber(L, 4);
  tc_Rectf *rect = lua_newuserdata(L, sizeof(tc_Rectf));
  luaL_setmetatable(L, RECTANGLE_CLASS);
//   lua_setmetatable(L, -2);
  *rect = tc_rectf(x, y, w, h);
  return 1;
}
static int tc_lua_rectangle_width(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->w = luaL_optnumber(L, 2, rect->w);
  lua_pushnumber(L, rect->w);
  return 1;
}
static int tc_lua_rectangle_height(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->h = luaL_optnumber(L, 2, rect->h);
  lua_pushnumber(L, rect->h);
  return 1;
}
static int tc_lua_rectangle_x(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->x = luaL_optnumber(L, 2, rect->x);
  lua_pushnumber(L, rect->x);
  return 1;
}
static int tc_lua_rectangle_y(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->y = luaL_optnumber(L, 2, rect->y);
  lua_pushnumber(L, rect->y);
  return 1;
}

static int luaopen_rectangle(lua_State *L) {
  luaL_Reg reg[] = {
    {"width", tc_lua_rectangle_width},
    {"height", tc_lua_rectangle_height},
    {"x", tc_lua_rectangle_x},
    {"y", tc_lua_rectangle_y},
    {NULL, NULL}
  };

  luaL_newmetatable(L, RECTANGLE_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
//   luaL_openlib(L, NULL, reg, 0);
  // lua_setfield(L, -2, "object");

  return 1;
}

/*********************
 * Graphics
 *********************/

 static int tc_lua_graphics_clear(lua_State *L) {
   tc_Color color = lua_optcolor(L, 1, BLACK);
   tc_clear(color);
   return 0;
 }

 static int tc_lua_graphics_draw(lua_State *L) {
  tc_Canvas *canvas = luaL_testudata(L, 1, CANVAS_CLASS);

  if (luaL_testudata(L, 1, IMAGE_CLASS)) {
    tc_lua_draw_image(L);
  } else if (canvas) {
    tc_lua_draw_canvas(L);
  }
  return 0;
}

static int tc_lua_scissor(lua_State *L) {
  int n = lua_gettop(L);
  if (n < 4) {
    tc_scissor(0, 0, Core.window.width, Core.window.height);
    return 0;
  }
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int w = luaL_checknumber(L, 3);
  int h = luaL_checknumber(L, 4);
  tc_scissor(x, y, w, h);

  return 0;
}

static int tc_lua_graphics_dimensions(lua_State *L) {
  lua_pushinteger(L, Core.window.width);
  lua_pushinteger(L, Core.window.height);
  return 2;
}

static int tc_lua_graphics_width(lua_State *L) {
  lua_pushinteger(L, Core.window.width);
  return 1;
}

static int tc_lua_graphics_height(lua_State *L) {
  lua_pushinteger(L, Core.window.height);
  return 1;
}

static int tc_lua_text_width(lua_State *L) {
  size_t size;
  const char *text = luaL_checklstring(L, 1, &size);
  int width = tc_font_get_text_width(Core.defaultFont, text, size);

  lua_pushinteger(L, width);
  return 1;
}

static int tc_lua_graphics_push(lua_State *L) {
  tc_graphics_push();
  return 0;
}

static int tc_lua_graphics_pop(lua_State *L) {
  tc_graphics_pop();
  return 0;
}

static int tc_lua_graphics_translate(lua_State *L) {
  float x = luaL_optnumber(L, 1, 0);
  float y = luaL_optnumber(L, 2, 0);
  tc_graphics_translate(x, y);
  return 0;
}

static int tc_lua_graphics_rotate(lua_State *L) {
  float angle = luaL_optnumber(L, 1, 0);
  tc_graphics_rotate(angle);
  return 0;
}

static int tc_lua_graphics_scale(lua_State *L) {
  float x = luaL_optnumber(L, 1, 0);
  float y = luaL_optnumber(L, 2, x);
  tc_graphics_scale(x, y);
  return 0;
}

TCDEF int luaopen_graphics(lua_State *L) {
  tc_lua_preload("tico.color", lua_Color);
  tc_lua_preload("tico.sprite", lua_Sprite);

  luaL_Reg reg[] = {
    {"drawRectangle", tc_lua_draw_rectangle},
    {"fillRectangle", tc_lua_fill_rectangle},
    {"drawCircle", tc_lua_draw_circle},
    {"fillCircle", tc_lua_fill_circle},
    {"drawTriangle", tc_lua_draw_triangle},
    {"fillTriangle", tc_lua_fill_triangle},
    {"line", tc_lua_draw_line},
    {"newImage", tc_lua_new_image},
    {"newCanvas", tc_lua_new_canvas},
    {"newRectangle", tc_lua_new_rectangle},
    {"newFont", tc_lua_new_font},
    {"scissor", tc_lua_scissor},
    {"clear", tc_lua_graphics_clear},
//     {"draw", tc_lua_graphics_draw},
    {"print", tc_lua_print},
    {"printf", tc_lua_printf},
    {"textWidth", tc_lua_text_width},
    {"getWidth", tc_lua_graphics_width},
    {"getHeight", tc_lua_graphics_height},
    {"getDimensions", tc_lua_graphics_dimensions},
    {"push", tc_lua_graphics_push},
    {"pop", tc_lua_graphics_pop},
    {"translate", tc_lua_graphics_translate},
    {"rotate", tc_lua_graphics_rotate},
    {"scale", tc_lua_graphics_scale},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

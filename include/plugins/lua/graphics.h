#ifndef TICO_LUA_GRAPHICS_H
#define TICO_LUA_GRAPHICS_H

#include "../../tico.h"


int luaopen_graphics(lua_State *L);

#endif // TICO_LUA_GRAPHICS_H

#if defined(TICO_LUA_IMPLEMENTATION)

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

static int tico_lua_draw_rectangle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int w = luaL_checknumber(L, 3);
  int h = luaL_checknumber(L, 4);
  tc_Color color = lua_optcolor(L, 5, WHITE);

  tico_graphics_draw_rectangle(x, y, w, h, color);
  return 0;
}
static int tico_lua_fill_rectangle(lua_State *L) {
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  float w = luaL_checknumber(L, 3);
  float h = luaL_checknumber(L, 4);
  tc_Color color = lua_optcolor(L, 5, WHITE);

  tico_graphics_fill_rectangle(x, y, w, h, color);
  return 0;
}

static int tico_lua_draw_circle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int radius = luaL_checknumber(L, 3);
  tc_Color color = lua_optcolor(L, 4, WHITE);

  tico_graphics_draw_circle(x, y, radius, color);
  return 0;
}
static int tico_lua_fill_circle(lua_State *L) {
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int radius = luaL_checknumber(L, 3);
  tc_Color color = lua_optcolor(L, 4, WHITE);

  tico_graphics_fill_circle(x, y, radius, color);
  return 0;
}
static int tico_lua_draw_triangle(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);
  int x2 = luaL_checknumber(L, 5);
  int y2 = luaL_checknumber(L, 6);

  tc_Color color = lua_optcolor(L, 7, WHITE);

  tico_graphics_draw_triangle(x0, y0, x1, y1, x2, y2, color);
  return 0;
}
static int tico_lua_fill_triangle(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);
  int x2 = luaL_checknumber(L, 5);
  int y2 = luaL_checknumber(L, 6);

  tc_Color color = lua_optcolor(L, 7, WHITE);

  tico_graphics_fill_triangle(x0, y0, x1, y1, x2, y2, color);
  return 0;
}

static int tico_lua_draw_line(lua_State *L) {
  int x0 = luaL_checknumber(L, 1);
  int y0 = luaL_checknumber(L, 2);
  int x1 = luaL_checknumber(L, 3);
  int y1 = luaL_checknumber(L, 4);

  tc_Color color = lua_optcolor(L, 5, WHITE);
  tico_graphics_draw_line(x0, y0, x1, y1, color);
  return 0;
}

/*********************
 * Text
 *********************/

 static int tico_lua_new_font(lua_State *L) {
   const char *filename = luaL_checkstring(L, 1);
   int size = luaL_optinteger(L, 2, 16);
   tc_Font *font = lua_newuserdata(L, sizeof(*font));
   luaL_setmetatable(L, FONT_CLASS);

   *font = tico_font_load(filename, size);
   return 1;
 }

 static int tico_lua_destroy_font(lua_State *L) {
   tc_Font *font = luaL_checkudata(L, 1, FONT_CLASS);
   tico_font_destroy(font);
   return 0;
 }

 static int tico_lua_font_text_width(lua_State *L) {
   tc_Font *font = luaL_checkudata(L, 1, FONT_CLASS);
   const char *text = luaL_checkstring(L, 2);
   int width = tico_font_get_text_width(*font, text, strlen(text));
   lua_pushinteger(L, width);
   return 1;
 }
 static int tico_lua_font_text_height(lua_State *L) {}

static int tico_lua_print(lua_State *L) {
  int index = 0;
  tc_Font *font = &Core.graphics.default_font;
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

  tico_graphics_draw_text_font_scale(*font, text, x, y, sx, sy, color);
}

static int tico_lua_printf(lua_State *L) {
  int index = 0;
  tc_Font *font = &Core.graphics.default_font;
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

  tico_graphics_draw_text_font_scale_width(*font, text, x, y, width, 1, 1, color);
}

static int luaopen_font(lua_State *L) {
  luaL_Reg reg[] = {
    {"__call", tico_lua_new_font},
    {"__gc", tico_lua_destroy_font},
    {"textWidth", tico_lua_font_text_width},
    {"textHeight", tico_lua_font_text_height},
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

static int tico_lua_image_height(lua_State *L) {
  tc_Image *tex = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, tex->height);

  return 1;
}
static int tico_lua_image_width(lua_State *L) {
  tc_Image *tex = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, tex->width);

  return 1;
}

static int tico_lua_new_image(lua_State *L) {
  tc_Image *tex = (tc_Image*)lua_newuserdata(L, sizeof(tc_Image));
  luaL_setmetatable(L, IMAGE_CLASS);
  const char *path = luaL_checkstring(L, 1);
  *tex = tico_image_load(path);
  return 1;
}

static int tico_lua_draw_image(lua_State *L) {
  int index = 0;
  tc_Image *image = luaL_checkudata(L, ++index, IMAGE_CLASS);

  tc_Rectf rect = tico_rectf(0, 0, image->width, image->height);
  tc_Color color = WHITE;
  if (luaL_testudata(L, 2, RECTANGLE_CLASS)) {
    rect = lua_optrectf(L, ++index, rect);
  }

  // exit(-1);
  float x = luaL_optnumber(L, ++index, 0);
  float y = luaL_optnumber(L, ++index, 0);
  if (lua_type(L, index+1) != LUA_TNUMBER) {
    color = lua_optcolor(L, ++index, WHITE);
    tico_image_draw_part(*image, rect, x, y, color);
    return 0;
  }
  float angle = luaL_optnumber(L, ++index, 0);
  float sx = luaL_optnumber(L, ++index, 1);
  float sy = luaL_optnumber(L, ++index, 1);
  float cx = luaL_optnumber(L, ++index, 0);
  float cy = luaL_optnumber(L, ++index, 0);
  color = lua_optcolor(L, ++index, WHITE);
  tico_image_draw_part_ex(*image, rect, x, y, angle, sx, sy, cx, cy, color);
  return 0;
}

static int tico_lua_destroy_image(lua_State *L) {
  // tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
  // tico_image_destroy(image);
  return 0;
}

static int tico_lua_image_size(lua_State *L) {
  tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
  lua_pushnumber(L, image->width);
  lua_pushnumber(L, image->height);
  return 2;
}

static int tico_lua_image_wrap(lua_State *L) {
  tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);

  const char *minWrapName = luaL_checkstring(L, 2);
  const char *magWrapName = luaL_checkstring(L, 3);

  int minWrap, magWrap;

  tico_texture_get_wrap(image->texture, &minWrap, &magWrap);

  if (!strcmp(minWrapName, "repeat")) minWrap = GL_REPEAT;
  else if (!strcmp(minWrapName, "clamp")) minWrap = GL_CLAMP_TO_BORDER;

  if (!strcmp(magWrapName, "repeat")) magWrap = GL_REPEAT;
  else if (!strcmp(magWrapName, "clamp")) magWrap = GL_CLAMP_TO_BORDER;

  tico_texture_set_wrap(&image->texture, minWrap, magWrap);
}

static int tico_lua_image_filter(lua_State *L) {}


static int luaopen_image(lua_State *L) {
//   luaL_Reg methods[] = {
//     {"new", tc_lua_new_image},
//     {NULL, NULL}
//   };
//   luaL_newlib(L, methods);

  luaL_Reg reg[] = {
    {"getWidth", tico_lua_image_width},
    {"getHeight",  tico_lua_image_height},
    {"draw", tico_lua_draw_image},
    {"getSize", tico_lua_image_size},
    {"setWrap", tico_lua_image_wrap},
    {"setFilter", tico_lua_image_filter},
    {"__gc", tico_lua_destroy_image},
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

static int tico_lua_new_canvas(lua_State *L) {
  int width = luaL_checknumber(L, 1);
  int height = luaL_checknumber(L, 2);
  tc_Canvas *canvas = lua_newuserdata(L, sizeof(*canvas));
  luaL_setmetatable(L, CANVAS_CLASS);
  *canvas = tico_canvas_create(width, height);

  return 1;
}

static int tico_lua_set_canvas(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tico_canvas_attach(*canvas);

  return 0;
}

static int tico_lua_unset_canvas(lua_State *L) {
  tico_canvas_detach();
  return 0;
}

static int tico_lua_draw_canvas(lua_State *L) {
  int index = 0;
  tc_Canvas *canvas = luaL_checkudata(L, ++index, CANVAS_CLASS);
  tc_Rectf rect = tico_rectf(0, 0, canvas->width, canvas->height);
  tc_Color color = WHITE;
  if (luaL_testudata(L, 2, RECTANGLE_CLASS)) {
    rect = lua_optrectf(L, ++index, rect);
  }

  float x = luaL_optnumber(L, ++index, 0);
  float y = luaL_optnumber(L, ++index, 0);
  if (lua_type(L, index+1) != LUA_TNUMBER) {
    color = lua_optcolor(L, ++index, WHITE);
    tico_canvas_draw_part(*canvas, rect, x, y, color);
    return 0;
  }
  float angle = luaL_optnumber(L, ++index, 0);
  float sx = luaL_optnumber(L, ++index, 1);
  float sy = luaL_optnumber(L, ++index, 1);
  int cx = luaL_optnumber(L, ++index, 0);
  int cy = luaL_optnumber(L, ++index, 0);
  color = lua_optcolor(L, ++index, WHITE);

  tico_canvas_draw_part_ex(*canvas, rect, x, y, angle, sx, sy, cx, cy, color);
  return 0;
}

static int tico_lua_canvas_auto(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tico_canvas_draw_auto(*canvas);
  return 0;
}

static int tico_lua_destroy_canvas(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  tico_canvas_destroy(canvas);
  return 0;
}

static int tico_lua_canvas_width(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  lua_pushinteger(L, canvas->width);
  return 1;
}

static int tico_lua_canvas_height(lua_State *L) {
  tc_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_CLASS);
  lua_pushinteger(L, canvas->height);
  return 1;
}

static int tico_lua_canvas_size(lua_State *L) {
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
    {"attach", tico_lua_set_canvas},
    {"detach", tico_lua_unset_canvas},
    {"draw", tico_lua_draw_canvas},
    {"auto", tico_lua_canvas_auto},
    {"getWidth", tico_lua_canvas_width},
    {"getHeight", tico_lua_canvas_height},
    {"getSize", tico_lua_canvas_size},
    {"__gc", tico_lua_destroy_canvas},
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

static int tico_lua_new_rectangle(lua_State *L) {
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  float w = luaL_checknumber(L, 3);
  float h = luaL_checknumber(L, 4);
  tc_Rectf *rect = lua_newuserdata(L, sizeof(tc_Rectf));
  luaL_setmetatable(L, RECTANGLE_CLASS);
//   lua_setmetatable(L, -2);
  *rect = tico_rectf(x, y, w, h);
  return 1;
}
static int tico_lua_rectangle_width(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->w = luaL_optnumber(L, 2, rect->w);
  lua_pushnumber(L, rect->w);
  return 1;
}
static int tico_lua_rectangle_height(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->h = luaL_optnumber(L, 2, rect->h);
  lua_pushnumber(L, rect->h);
  return 1;
}
static int tico_lua_rectangle_x(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->x = luaL_optnumber(L, 2, rect->x);
  lua_pushnumber(L, rect->x);
  return 1;
}
static int tico_lua_rectangle_y(lua_State *L) {
  tc_Rectf *rect = luaL_checkudata(L, 1, RECTANGLE_CLASS);
  rect->y = luaL_optnumber(L, 2, rect->y);
  lua_pushnumber(L, rect->y);
  return 1;
}

static int luaopen_rectangle(lua_State *L) {
  luaL_Reg reg[] = {
    {"width", tico_lua_rectangle_width},
    {"height", tico_lua_rectangle_height},
    {"x", tico_lua_rectangle_x},
    {"y", tico_lua_rectangle_y},
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
 * Shader
 *********************/
static int tico_lua_shader_new(lua_State *L) {
  const char *vertShader = luaL_checkstring(L, 1);
  const char *fragShader = luaL_checkstring(L, 2);

  tc_Shader *shader = lua_newuserdata(L, sizeof(*shader));
  luaL_setmetatable(L, SHADER_CLASS);
  if (lua_gettop(L) > 1) {
    *shader = tico_shader_create_effect(vertShader, fragShader);
  } else {
    *shader = tico_shader_create_effect(fragShader, vertShader);
  }



  return 1;
}

static int tico_lua_shader_new_effect(lua_State *L) {
  const char *defaultVert =
    "vec4 position(vec4 pos, mat4 world, mat4 view) { return world * view * pos; }\n";
  const char *vertShader = luaL_checkstring(L, 1);
  const char *fragShader = luaL_optstring(L, 2, defaultVert);

  tc_Shader *shader = lua_newuserdata(L, sizeof(*shader));
  luaL_setmetatable(L, SHADER_CLASS);
  if (lua_gettop(L) > 2) {
    *shader = tico_shader_create_effect(vertShader, fragShader);
  } else {
    *shader = tico_shader_create_effect(fragShader, vertShader);
  }

  return 1;
}

static int tico_lua_shader_attach(lua_State *L) {
  tc_Shader *shader = luaL_checkudata(L, 1, SHADER_CLASS);

  tico_shader_attach(*shader);
  return 0;
}

static int tico_lua_shader_send(lua_State *L) {
  tc_Shader *shader = luaL_checkudata(L, 1, SHADER_CLASS);
  const char *name = luaL_checkstring(L, 2);

  int count = lua_gettop(L) - 2;

  int type = lua_type(L, 3);
  switch(type) {
    case LUA_TNUMBER:
    {
      float n[count];
      for (int i = 0; i < count; i++) n[i] = lua_tonumber(L, i+3);
      tico_shader_send_count(*shader, name, count, n, TIC_UNIFORM_FLOAT);
      break;
    }
    case LUA_TTABLE:
    {
      lua_len(L, 3);
      int size = lua_tonumber(L, -1);
      lua_pop(L, 1);
      int utype = size == 2 ? TIC_UNIFORM_VEC2 : size == 3 ? TIC_UNIFORM_VEC3 : TIC_UNIFORM_VEC4;
      float data[count][size];
      for (int i = 0; i < count; i++) {
        for (int j = 0; j < size; j++) {
          lua_rawgeti(L, i+3, j+1);
          data[i][j] = lua_tonumber(L, -1);
          lua_pop(L, 1);
        }
      }

      tico_shader_send_count(*shader, name, count, data, utype);
      break;
    }
  }
  return 0;
}

// static int tico_lua_shader_new_gba(lua_State *L) {
//   tc_Shader *shader = lua_newuserdata(L, sizeof(*shader));
//   luaL_setmetatable(L, SHADER_CLASS);

//   tico_shader_new_gba(shader);

//   return 1;
// }

// static int tico_lua_shader_new_outline(lua_State *L) {
//   tc_Shader *shader = lua_newuserdata(L, sizeof(*shader));
//   luaL_setmetatable(L, SHADER_CLASS);

//   tico_shader_new_outline(shader);

//   return 1;
// }

static int tico_lua_shader__gc(lua_State *L) {
  tc_Shader *shader = luaL_checkudata(L, 1, SHADER_CLASS);
  tico_shader_destroy(shader);
  return 0;
}

static int tico_lua_shader_detach(lua_State *L) {
  tico_shader_detach();
  return 0;
}

static int luaopen_shader(lua_State *L) {
  luaL_Reg reg[] = {
    {"attach", tico_lua_shader_attach},
    {"detach", tico_lua_shader_detach},
    {"send", tico_lua_shader_send},
    {"__gc", tico_lua_shader__gc},
    {NULL, NULL}
  };

  luaL_newmetatable(L, SHADER_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  return 1;
}

/*********************
 * Graphics
 *********************/

static int tico_lua_graphics_clear(lua_State *L) {
  tc_Color color = BLACK;
  int type = lua_type(L, 1);
  if (type == LUA_TNUMBER) {
    color.r = luaL_checknumber(L, 1);
    color.g = luaL_checknumber(L, 2);
    color.b = luaL_checknumber(L, 3);
    color.a = luaL_optnumber(L, 4, 255);

  } else if (type == LUA_TTABLE) {
    color = lua_optcolor(L, 1, BLACK);
  }

  tico_graphics_clear(color);
  return 0;
}

 static int tico_lua_graphics_draw(lua_State *L) {
  tc_Canvas *canvas = luaL_testudata(L, 1, CANVAS_CLASS);

  if (luaL_testudata(L, 1, IMAGE_CLASS)) {
    tico_lua_draw_image(L);
  } else if (canvas) {
    tico_lua_draw_canvas(L);
  }
  return 0;
}

static int tico_lua_scissor(lua_State *L) {
  int n = lua_gettop(L);
  if (n < 4) {
    tico_graphics_scissor(0, 0, Core.window.width, Core.window.height);
    return 0;
  }
  int x = luaL_checknumber(L, 1);
  int y = luaL_checknumber(L, 2);
  int w = luaL_checknumber(L, 3);
  int h = luaL_checknumber(L, 4);
  tico_graphics_scissor(x, y, w, h);

  return 0;
}

static int tico_lua_graphics_dimensions(lua_State *L) {
  lua_pushinteger(L, Core.window.width);
  lua_pushinteger(L, Core.window.height);
  return 2;
}

static int tico_lua_graphics_width(lua_State *L) {
  lua_pushinteger(L, Core.window.width);
  return 1;
}

static int tico_lua_graphics_height(lua_State *L) {
  lua_pushinteger(L, Core.window.height);
  return 1;
}

static int tico_lua_text_width(lua_State *L) {
  size_t size;
  const char *text = luaL_checklstring(L, 1, &size);
  int width = tico_font_get_text_width(Core.graphics.default_font, text, size);

  lua_pushinteger(L, width);
  return 1;
}

static int tico_lua_graphics_push(lua_State *L) {
  tico_graphics_push();
  return 0;
}

static int tico_lua_graphics_pop(lua_State *L) {
  tico_graphics_pop();
  return 0;
}

static int tico_lua_graphics_origin(lua_State *L) {
  tico_graphics_origin();
  return 0;
}

static int tico_lua_graphics_translate(lua_State *L) {
  float x = luaL_optnumber(L, 1, 0);
  float y = luaL_optnumber(L, 2, 0);
  tico_graphics_translate(x, y);
  return 0;
}

static int tico_lua_graphics_rotate(lua_State *L) {
  float angle = luaL_optnumber(L, 1, 0);
  tico_graphics_rotate(angle);
  return 0;
}

static int tico_lua_graphics_scale(lua_State *L) {
  float x = luaL_optnumber(L, 1, 0);
  float y = luaL_optnumber(L, 2, x);
  tico_graphics_scale(x, y);
  return 0;
}

static int tico_lua_blend_mode(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  // tico_batch_draw_reset(&Core.render.batch);
  tico_render_draw_reset(&Core.graphics.render);
  if (!strcmp(name, "alpha")) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else if (!strcmp(name, "subtract")) {
    glBlendEquation(GL_FUNC_SUBTRACT);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
  } else if (!strcmp(name, "multiply")) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
  } else if (!strcmp(name, "add")) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  } else if (!strcmp(name, "replace")) {
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
  }
}

int luaopen_graphics(lua_State *L) {

  luaL_Reg reg[] = {
    {"drawRectangle", tico_lua_draw_rectangle},
    {"fillRectangle", tico_lua_fill_rectangle},
    {"drawCircle", tico_lua_draw_circle},
    {"fillCircle", tico_lua_fill_circle},
    {"drawTriangle", tico_lua_draw_triangle},
    {"fillTriangle", tico_lua_fill_triangle},
    {"line", tico_lua_draw_line},
    {"newImage", tico_lua_new_image},
    {"newCanvas", tico_lua_new_canvas},
    {"newRectangle", tico_lua_new_rectangle},
    {"newFont", tico_lua_new_font},
    {"newShader", tico_lua_shader_new},
    {"newEffect", tico_lua_shader_new_effect},
    {"blendMode", tico_lua_blend_mode},
    {"scissor", tico_lua_scissor},
    {"clear", tico_lua_graphics_clear},
    {"draw", tico_lua_graphics_draw},
    {"print", tico_lua_print},
    {"printf", tico_lua_printf},
    {"textWidth", tico_lua_text_width},
    {"getWidth", tico_lua_graphics_width},
    {"getHeight", tico_lua_graphics_height},
    {"getSize", tico_lua_graphics_dimensions},
    {"push", tico_lua_graphics_push},
    {"pop", tico_lua_graphics_pop},
    {"origin", tico_lua_graphics_origin},
    {"translate", tico_lua_graphics_translate},
    {"rotate", tico_lua_graphics_rotate},
    {"scale", tico_lua_graphics_scale},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

#ifndef TICO_LUA_UI_H
#define TICO_LUA_UI_H

#include "../../tico.h"

int luaopen_ui(lua_State *L);

#endif

// #define TICO_LUA_IMPLEMENTATION

#if defined(TICO_LUA_IMPLEMENTATION)


static int tic_lua_ui_begin_window(lua_State *L) {
  const char *title = luaL_checkstring(L, 1);
  tc_Rect rect = lua_optrect_table(L, 2, tic_rect(0, 0, 240, 90));
  int window = tic_ui_begin_window(title, rect);

  lua_pushboolean(L, window);
  return 1;
}

static int tic_lua_ui_end_window(lua_State *L) {
  tic_ui_end_window();
  return 0;
}

static int tic_lua_ui_button(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  int pressed = tic_ui_button(label);
  lua_pushboolean(L, pressed);
  return 1;
}

static int tic_lua_ui_text(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  tic_ui_text(text);

  return 0;
}

static int tic_lua_ui_label(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  tic_ui_label(label);

  return 0;
}

static int tic_lua_ui_textbox(lua_State *L) {
  mu_Id id = luaL_checknumber(L, 1);
  char *text = (char*)luaL_checkstring(L, 2);
  int size = luaL_optnumber(L, 3, 128);

  char *str = TIC_CALLOC(1, size);
  strcpy(str, text);
  tc_Rect r = mu_layout_next(Core.ui.ctx);
  int send = tic_ui_textbox_raw(id, str, size, r);

  lua_pushstring(L, str);
  lua_pushinteger(L, send);
  free(str);
  return 2;
}

static int tic_lua_ui_image(lua_State *L) {
  tc_Canvas *canvas = luaL_testudata(L, 1, "Canvas");
  tc_Image *image = luaL_testudata(L, 1, "Image");
  if (canvas) {
    int w = luaL_optnumber(L, 2, canvas->width);
    int h = luaL_optnumber(L, 3, canvas->height);
    mu_Vec2 v = {w, h};
    tc_Rect rect = tic_rect(0, canvas->height, canvas->width, -canvas->height);
    tic_ui_image(canvas->texture, v, rect, WHITE);
  } else if (image) {
    int w = luaL_optnumber(L, 2, image->height);
    int h = luaL_optnumber(L, 3, image->height);
    mu_Vec2 v = {w, h};
    tc_Rect rect = tic_rect(0, 0, image->width, image->height);
    tic_ui_image(image->texture, v, rect, WHITE);
  }

  return 0;
}

static int tic_lua_ui_window_size(lua_State *L) {
  mu_Container *cnt = tic_ui_get_current_container();

  lua_pushnumber(L, cnt->body.w);
  lua_pushnumber(L, cnt->body.h);

  return 2;
}

static int tic_lua_ui_window_pos(lua_State *L) {
  mu_Container *cnt = tic_ui_get_current_container();

  lua_pushnumber(L, cnt->body.x);
  lua_pushnumber(L, cnt->body.y);

  return 2;
}

static int tic_lua_ui_header(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  int open = tic_ui_header(label);

  lua_pushboolean(L, open);
  return 1;
}

static int tic_lua_ui_begin_panel(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  tic_ui_begin_panel(label);
  return 0;
}

static int tic_lua_ui_end_panel(lua_State *L) {
  tic_ui_end_panel();
  return 0;
}

static int tic_lua_layout_row(lua_State *L) {
  int top = lua_gettop(L);
  int n = luaL_checknumber(L, 1);
  int width[n];
  if (lua_type(L, 2) == LUA_TTABLE) {
    for (int i = 0; i < n; i++) {
      lua_rawgeti(L, 2, (i+1));
      width[i] = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }
  } else {
    lua_pushstring(L, "argument #2 of tico.ui.layoutRow must be a table");
    lua_error(L);
    return 1;
  }
  int height = luaL_optnumber(L, 3, 0);

  tic_ui_layout_row(n, width, height);

  return 0;
}

static int tic_lua_layout_width(lua_State *L) {
  int width = luaL_checknumber(L, 1);
  tic_ui_layout_width(width);

  return 0;
}

static int tic_lua_layout_height(lua_State *L) {
  int height = luaL_checknumber(L, 1);
  tic_ui_layout_height(height);

  return 0;
}

static int tic_lua_begin_column(lua_State *L) {
  tic_ui_begin_column();
  return 0;
}

static int tic_lua_end_column(lua_State *L) {
  tic_ui_end_column();
  return 0;
}

static int tic_lua_ui_checkbox(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  if (!lua_isboolean(L, 2)) {
    lua_pushstring(L, "tico.ui.checkbox argument #1 expects a boolean");
    lua_error(L);
    return 1;
  }
  tc_bool value = lua_toboolean(L, 2);
  int state = tic_ui_checkbox(label, &value);

  lua_pushboolean(L, value);
  lua_pushnumber(L, state);
  return 2;
}

static int tic_lua_ui_slider(lua_State *L) {
  float value = luaL_checknumber(L, 1);
  float lo = luaL_optnumber(L, 2, 0);
  float hi = luaL_optnumber(L, 3, 1);

  float *val = TIC_MALLOC(sizeof(float));
  *val = value;

//   mu_get_id(ui.ctx, &val, sizeof(float));
  int state = tic_ui_slider(val, lo, hi);
  value = *val;
//   mu_pop_id(ui.ctx);
  TIC_FREE(val);

  lua_pushnumber(L, value);
  lua_pushnumber(L, state);
  return 2;
}

static int tic_lua_ui_number(lua_State *L) {
  float number = luaL_checknumber(L, 1);
  float step = luaL_optnumber(L, 2, 1);
  int state = tic_ui_number(&number, step);

  lua_pushnumber(L, number);
  lua_pushnumber(L, state);
  return 2;
}

static int tic_lua_begin_tree_node(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  int open = tic_ui_begin_treenode(label);

  lua_pushboolean(L, open);
  return 1;
}

static int tic_lua_end_tree_node(lua_State *L) {
  tic_ui_end_treenode();
  return 0;
}

static int tic_lua_ui_is_window_focused(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  mu_Container *cnt = tic_ui_get_container(str);
  int last = tic_ui_last_zindex();
  lua_pushboolean(L, cnt->zindex == last);
  return 1;
}

int luaopen_ui(lua_State *L) {
  luaL_Reg reg[] = {
    {"beginWindow", tic_lua_ui_begin_window},
    {"endWindow", tic_lua_ui_end_window},
    {"beginTree", tic_lua_begin_tree_node},
    {"endTree", tic_lua_end_tree_node},
    {"windowSize", tic_lua_ui_window_size},
    {"windowPos", tic_lua_ui_window_pos},
    {"button", tic_lua_ui_button},
    {"image", tic_lua_ui_image},
    {"label", tic_lua_ui_label},
    {"text", tic_lua_ui_text},
    {"slider", tic_lua_ui_slider},
    {"number", tic_lua_ui_number},
    {"checkbox", tic_lua_ui_checkbox},
    {"textbox", tic_lua_ui_textbox},
    {"header", tic_lua_ui_header},
    {"layoutRow", tic_lua_layout_row},
    {"layoutWidth", tic_lua_layout_width},
    {"layoutHeight", tic_lua_layout_height},
    {"beginColumn", tic_lua_begin_column},
    {"endColumn", tic_lua_end_column},
    {"beginPanel", tic_lua_ui_begin_panel},
    {"endPanel", tic_lua_ui_end_panel},
    {"isWindowFocused", tic_lua_ui_is_window_focused},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

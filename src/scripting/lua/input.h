#ifndef TICO_LUA_INPUT_H
#define TICO_LUA_INPUT_H

#include "../../tico.h"

int luaopen_input(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tic_lua_key_down(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tic_input_get_key_code(key);
  int down = tic_input_key_down(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_key_pressed(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tic_input_get_key_code(key);
  int down = tic_input_key_pressed(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_key_up(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tic_input_get_key_code(key);
  int down = tic_input_key_up(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_key_released(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tic_input_get_key_code(key);
  int down = tic_input_key_released(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_mouse_down(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tic_input_mouse_down(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tic_lua_mouse_pressed(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tic_input_mouse_pressed(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tic_lua_mouse_up(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tic_input_mouse_up(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tic_lua_mouse_released(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tic_input_mouse_released(button - 1);

  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_get_mouse_x(lua_State *L) {
  int x;
  tic_input_get_mouse_pos(&x, NULL);
  lua_pushnumber(L, x);
  return 1;
}

static int tic_lua_get_mouse_y(lua_State *L) {
  int y;
  tic_input_get_mouse_pos(NULL, &y);
  lua_pushnumber(L, y);
  return 1;
}

static int tic_lua_get_mouse_position(lua_State *L) {
  int x, y;
  tic_input_get_mouse_pos(&x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int tic_lua_get_mouse_delta(lua_State *L) {
  int x, y;
  tic_input_get_mouse_delta(&x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int tic_lua_fix_mouse(lua_State *L) {
  int fix = tc_true;
  if (lua_gettop(L) > 0) fix = lua_toboolean(L, 1);
  if (fix) tic_input_fix_mouse();
  else tic_input_release_mouse();
  return 0;
}

static int tic_lua_down(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int down = -1;
  if (strstr(name, "mouse")) {
    // hashmap_item *item = hashmap_get(Core.input.names.mouseButtonNames, name);
    int *val = map_get(&Core.input.names.mouseButtonNames, name);
    if (val) down = tic_input_mouse_down(*val);
  } else if (strstr(name, "jp")) {
    int *val = map_get(&Core.input.names.joyButtonNames, name);
    if (val) down = tic_input_joy_down(0, *val);
  } else {
    // hashmap_item *item = hashmap_get(Core.input.names.keyNames, name);
    int *val = map_get(&Core.input.names.keyNames, name);
    if (val) down = tic_input_key_down(*val);
  }

  if (down < 0) {
    char msg[64];
    sprintf(msg, "key %s not found", name);
    lua_pushstring(L, msg);
    lua_error(L);
  }
  else lua_pushboolean(L, down);

  return 1;
}

static int tic_lua_pressed(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int down = -1;
  if (strstr(name, "mouse")) {
    // hashmap_item *item = hashmap_get(Core.input.names.mouseButtonNames, name);
    int *val = map_get(&Core.input.names.mouseButtonNames, name);
    if (val) down = tic_input_mouse_pressed(*val);
  } else if (strstr(name, "jp")) {
    int *val = map_get(&Core.input.names.joyButtonNames, name);
    if (val) down = tic_input_joy_pressed(0, *val);
  } else {
    // hashmap_item *item = hashmap_get(Core.input.names.keyNames, name);
    int *val = map_get(&Core.input.names.keyNames, name);
    if (val) down = tic_input_key_pressed(*val);
  }

  if (down < 0) {
    char msg[64];
    sprintf(msg, "key %s not found", name);
    lua_pushstring(L, msg);
    lua_error(L);
  }
  else lua_pushboolean(L, down);

  return 1;
}

static int tic_lua_up(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int down = -1;
  if (strstr(name, "mouse")) {
    // hashmap_item *item = hashmap_get(Core.input.names.mouseButtonNames, name);
    int *val = map_get(&Core.input.names.mouseButtonNames, name);
    if (val) down = tic_input_mouse_up(*val);
  } else if (strstr(name, "jp")) {
    int *val = map_get(&Core.input.names.joyButtonNames, name);
    if (val) down = tic_input_joy_up(0, *val);
  } else {
    // hashmap_item *item = hashmap_get(Core.input.names.keyNames, name);
    int *val = map_get(&Core.input.names.keyNames, name);
    if (val) down = tic_input_key_up(*val);
  }

  if (down < 0) {
    char msg[64];
    sprintf(msg, "key %s not found", name);
    lua_pushstring(L, msg);
    lua_error(L);
  }
  else lua_pushboolean(L, down);

  return 1;
}

static int tic_lua_released(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int down = -1;
  if (strstr(name, "mouse")) {
    // hashmap_item *item = hashmap_get(Core.input.names.mouseButtonNames, name);
    int *val = map_get(&Core.input.names.mouseButtonNames, name);
    if (val) down = tic_input_mouse_released(*val);
  } else if (strstr(name, "jp")) {
    int *val = map_get(&Core.input.names.joyButtonNames, name);
    if (val) down = tic_input_joy_released(0, *val);
  } else {
    // hashmap_item *item = hashmap_get(Core.input.names.keyNames, name);
    int *val = map_get(&Core.input.names.keyNames, name);
    if (val) down = tic_input_key_released(*val);
  }

  if (down < 0) {
    char msg[64];
    sprintf(msg, "key %s not found", name);
    lua_pushstring(L, msg);
    lua_error(L);
  }
  else lua_pushboolean(L, down);

  return 1;
}

static int tic_lua_get_mouse_scroll(lua_State *L) {
  float x, y;
  tic_input_get_mouse_scroll(&x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int tic_lua_joy_down(lua_State *L) {
  int jid = luaL_checkinteger(L, 1);
  const char *key = luaL_checkstring(L, 2);
  int btn = tic_input_get_joy_btncode(key);

  int down = tic_input_joy_down(jid, btn);
  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_joy_pressed(lua_State *L) {
  int jid = luaL_checkinteger(L, 1);
  const char *key = luaL_checkstring(L, 2);
  int btn = tic_input_get_joy_btncode(key);

  int down = tic_input_joy_pressed(jid, btn);
  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_joy_up(lua_State *L) {
  int jid = luaL_checkinteger(L, 1);
  const char *key = luaL_checkstring(L, 2);
  int btn = tic_input_get_joy_btncode(key);

  int down = tic_input_joy_up(jid, btn);
  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_joy_released(lua_State *L) {
  int jid = luaL_checkinteger(L, 1);
  const char *key = luaL_checkstring(L, 2);
  int btn = tic_input_get_joy_btncode(key);

  int down = tic_input_joy_released(jid, btn);
  lua_pushboolean(L, down);
  return 1;
}

static int tic_lua_get_joy_name(lua_State *L) {
  int jid = luaL_checkinteger(L, 1);
  const char *name = glfwGetGamepadName(jid);
  if (name) lua_pushstring(L, name);
  else lua_pushstring(L, "none");

  return 1;
}

int luaopen_input(lua_State *L) {
//   tc_lua_preload("tico.keys", tc_Keys);

  luaL_Reg reg[] = {
    {"isDown", tic_lua_down},
    {"isPressed", tic_lua_pressed},
    {"isUp", tic_lua_up},
    {"isReleased", tic_lua_released},
    {"keyDown", tic_lua_key_down},
    {"keyPressed", tic_lua_key_pressed},
    {"keyUp", tic_lua_key_up},
    {"keyReleased", tic_lua_key_released},
    {"mouseDown", tic_lua_mouse_down},
    {"mousePressed", tic_lua_mouse_pressed},
    {"mouseUp", tic_lua_mouse_up},
    {"mouseReleased", tic_lua_mouse_released},
    {"mouseX", tic_lua_get_mouse_x},
    {"mouseY", tic_lua_get_mouse_y},
    {"mousePos", tic_lua_get_mouse_position},
    {"mouseDelta", tic_lua_get_mouse_delta},
    {"fixMouse", tic_lua_fix_mouse},
    {"mouseScroll", tic_lua_get_mouse_scroll},
    {"joyDown", tic_lua_joy_down},
    {"joyPressed", tic_lua_joy_pressed},
    {"joyUp", tic_lua_joy_up},
    {"joyReleased", tic_lua_joy_released},
    {"joystickName", tic_lua_get_joy_name},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

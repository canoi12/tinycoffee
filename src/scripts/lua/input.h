#ifndef TICO_LUA_INPUT_H
#define TICO_LUA_INPUT_H

#include "../../tinycoffee.h"

TCDEF int luaopen_input(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static const char *tc_Keys =
"local MouseButton = {}\n"
"MouseButton.Left   = 1\n"
"MouseButton.Right  = 2\n"
"MouseButton.Middle = 3\n"
"local Key = {}\n"
"Key.Unknown =       -1\n"
"Key.Space =         32\n"
"Key.Apostrophe =    39\n"
"Key.Comma =         44\n"
"Key.Minus =         45\n"
"Key.Period =        46\n"
"Key.Slash =         47\n"
"Key.D0 =             48\n"
"Key.D1 =             49\n"
"Key.D2 =             50\n"
"Key.D3 =             51\n"
"Key.D4 =             52\n"
"Key.D5 =             53\n"
"Key.D6 =             54\n"
"Key.D7 =             55\n"
"Key.D8 =             56\n"
"Key.D9 =             57\n"
"Key.Semicolon =     59\n"
"Key.Equal =         61\n"
"Key.A =             65\n"
"Key.B =             66\n"
"Key.C =             67\n"
"Key.D =             68\n"
"Key.E =             69\n"
"Key.F =             70\n"
"Key.G =             71\n"
"Key.H =             72\n"
"Key.I =             73\n"
"Key.J =             74\n"
"Key.K =             75\n"
"Key.L =             76\n"
"Key.M =             77\n"
"Key.N =             78\n"
"Key.O =             79\n"
"Key.P =             80\n"
"Key.Q =             81\n"
"Key.R =             82\n"
"Key.S =             83\n"
"Key.T =             84\n"
"Key.U =             85\n"
"Key.V =             86\n"
"Key.W =             87\n"
"Key.X =             88\n"
"Key.Y =             89\n"
"Key.Z =             90\n"
"Key.Left_Bracket =  91\n"
"Key.Backslash =     92\n"
"Key.Right_Bracket = 93\n"
"Key.Grave_Accent =  96\n"
"Key.World_1 =       161\n"
"Key.World_2 =       162\n"
"Key.Escape =        256\n"
"Key.Enter =         257\n"
"Key.Tab =           258\n"
"Key.Backspace =     259\n"
"Key.Insert =        260\n"
"Key.Delete =        261\n"
"Key.Right =         262\n"
"Key.Left =          263\n"
"Key.Down =          264\n"
"Key.Up =            265\n"
"Key.Page_Up =       266\n"
"Key.Page_Down =     267\n"
"Key.Home =          268\n"
"Key.End =           269\n"
"Key.Caps_Lock =     280\n"
"Key.Scroll_Lock =   281\n"
"Key.Num_Lock =      282\n"
"Key.Print_Screen =  283\n"
"Key.Pause =         284\n"
"Key.F1 =            290\n"
"Key.F2 =            291\n"
"Key.F3 =            292\n"
"Key.F4 =            293\n"
"Key.F5 =            294\n"
"Key.F6 =            295\n"
"Key.F7 =            296\n"
"Key.F8 =            297\n"
"Key.F9 =            298\n"
"Key.F10 =           299\n"
"Key.F11 =           300\n"
"Key.F12 =           301\n"
"Key.F13 =           302\n"
"Key.F14 =           303\n"
"Key.F15 =           304\n"
"Key.F16 =           305\n"
"Key.F17 =           306\n"
"Key.F18 =           307\n"
"Key.F19 =           308\n"
"Key.F20 =           309\n"
"Key.F21 =           310\n"
"Key.F22 =           311\n"
"Key.F23 =           312\n"
"Key.F24 =           313\n"
"Key.F25 =           314\n"
"Key.KP_0 =          320\n"
"Key.KP_1 =          321\n"
"Key.KP_2 =          322\n"
"Key.KP_3 =          323\n"
"Key.KP_4 =          324\n"
"Key.KP_5 =          325\n"
"Key.KP_6 =          326\n"
"Key.KP_7 =          327\n"
"Key.KP_8 =          328\n"
"Key.KP_9 =          329\n"
"Key.KP_Decimal =    330\n"
"Key.KP_Divide =     331\n"
"Key.KP_Multiply =   332\n"
"Key.KP_Subtract =   333\n"
"Key.KP_Add =        334\n"
"Key.KP_Enter =      335\n"
"Key.KP_Equal =      336\n"
"Key.Left_Shift =    340\n"
"Key.Left_Control =  341\n"
"Key.Left_Alt =      342\n"
"Key.Left_Super =    343\n"
"Key.Right_Shift =   344\n"
"Key.Right_Control = 345\n"
"Key.Right_Alt =     346\n"
"Key.Right_Super =   347\n"
"Key.Menu =          348\n"
"return Key";

static int tc_lua_is_key_down(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tc_get_keycode(key);
  int down = tc_is_key_down(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tc_lua_is_key_pressed(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tc_get_keycode(key);
  int down = tc_is_key_pressed(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tc_lua_is_key_up(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tc_get_keycode(key);
  int down = tc_is_key_up(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tc_lua_is_key_released(lua_State *L) {
  const char *key = luaL_checkstring(L, 1);
  int code = tc_get_keycode(key);
  int down = tc_is_key_released(code);

  lua_pushboolean(L, down);
  return 1;
}

static int tc_lua_is_mouse_down(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tc_is_mouse_down(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tc_lua_is_mouse_pressed(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tc_is_mouse_pressed(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tc_lua_is_mouse_up(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tc_is_mouse_up(button - 1);

  lua_pushboolean(L, down);
  return 1;
}
static int tc_lua_is_mouse_released(lua_State *L) {
  int button = luaL_checknumber(L, 1);
  int down = tc_is_mouse_released(button - 1);

  lua_pushboolean(L, down);
  return 1;
}

static int tc_lua_get_mouse_x(lua_State *L) {
  int x;
  tc_get_mouse(&x, NULL);
  lua_pushnumber(L, x);
  return 1;
}

static int tc_lua_get_mouse_y(lua_State *L) {
  int y;
  tc_get_mouse(NULL, &y);
  lua_pushnumber(L, y);
  return 1;
}

static int tc_lua_get_mouse_position(lua_State *L) {
  int x, y;
  tc_get_mouse(&x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int tc_lua_get_mouse_delta(lua_State *L) {
  int x, y;
  tc_get_mouse_delta(&x, &y);
  lua_pushnumber(L, x);
  lua_pushnumber(L, y);
  return 2;
}

static int tc_lua_fix_mouse(lua_State *L) {
  int fix = tc_true;
  if (lua_gettop(L) > 0) fix = lua_toboolean(L, 1);
  if (fix) tc_fix_mouse();
  else tc_release_mouse();
  return 0;
}

TCDEF int luaopen_input(lua_State *L) {
//   tc_lua_preload("tico.keys", tc_Keys);

  luaL_Reg reg[] = {
    {"isKeyDown", tc_lua_is_key_down},
    {"isKeyPressed", tc_lua_is_key_pressed},
    {"isKeyUp", tc_lua_is_key_up},
    {"isKeyReleased", tc_lua_is_key_released},
    {"isMouseDown", tc_lua_is_mouse_down},
    {"isMousePressed", tc_lua_is_mouse_pressed},
    {"isMouseUp", tc_lua_is_mouse_up},
    {"isMouseReleased", tc_lua_is_mouse_released},
    {"mouseX", tc_lua_get_mouse_x},
    {"mouseY", tc_lua_get_mouse_y},
    {"mousePosition", tc_lua_get_mouse_position},
    {"mouseDelta", tc_lua_get_mouse_delta},
    {"fixMouse", tc_lua_fix_mouse},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

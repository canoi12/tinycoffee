#ifndef TICO_LUA_MATH_H
#define TICO_LUA_MATH_H

#include "../../tinycoffee.h"

TCDEF int luaopen_tcmath(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tc_lua_math_lerp(lua_State *L) {
  float a = luaL_checknumber(L, 1);
  float b = luaL_checknumber(L, 2);
  float t = luaL_checknumber(L, 3);

  float r = tc_lerp(a, b, t);
  lua_pushnumber(L, r);
  return 1;
}

static int tc_lua_math_round(lua_State *L) {
  float n = luaL_checknumber(L, 1);
  float round = tc_round(n);
  lua_pushnumber(L, round);
  return 1;
}

static int tc_lua_math_distance(lua_State *L) {
  float n = luaL_checknumber(L, 1);
  float m = luaL_checknumber(L, 2);
  float dist = tc_distance(n, m);
  lua_pushnumber(L, dist);
  return 1;
}

static int tc_lua_math_sign(lua_State *L) {
  float n = luaL_checknumber(L, 1);
  float s = tc_sign(n);
  lua_pushnumber(L, s);
  return 1;
}

static int tc_lua_math_clamp(lua_State *L) {
  float v = luaL_checknumber(L, 1);
  float lo = luaL_checknumber(L, 2);
  float hi = luaL_checknumber(L, 3);
  float val = tc_clamp(v, lo, hi);
  lua_pushnumber(L, val);
  return 1;
}

static int tc_lua_math_angle(lua_State *L) {
  float x0 = luaL_checknumber(L, 1);
  float y0 = luaL_checknumber(L, 2);
  float x1 = luaL_checknumber(L, 3);
  float y1 = luaL_checknumber(L, 4);
  float angle = tc_angle(x0, y0, x1, y1);
  lua_pushnumber(L, angle);
  return 1;
}

TCDEF int luaopen_tcmath(lua_State *L) {
  tc_lua_preload("tico.vector", lua_Vector);
  luaL_Reg reg[] = {
    {"lerp", tc_lua_math_lerp},
    {"round", tc_lua_math_round},
    {"distance", tc_lua_math_distance},
    {"sign", tc_lua_math_sign},
    {"clamp", tc_lua_math_clamp},
    {"angle", tc_lua_math_angle},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

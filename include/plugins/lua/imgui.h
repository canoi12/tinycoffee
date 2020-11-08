#ifndef TICO_LUA_IMGUI_H
#define TICO_LUA_IMGUI_H

#include "../../tico.h"
#include "../../tico_imgui.h"


int luaopen_imgui(lua_State *L);

#endif // TICO_LUA_IMGUI_H

#if defined(TICO_LUA_IMPLEMENTATION)

int tico_lua_imgui_Begin(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int open = lua_toboolean(L, 2);
  int flags = 0;

  int ret = tico_imgui_Begin(name, &open, flags);
  lua_pushboolean(L, ret);

  return 1;
}

int tico_lua_imgui_End(lua_State *L) {
  tico_imgui_End();
  return 0;
}

int luaopen_imgui(lua_State *L) {
  luaL_Reg reg[] = {
    {"Begin", tico_lua_imgui_Begin},
    {"End", tico_lua_imgui_End},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
  
  return 1;
}

#endif
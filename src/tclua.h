#ifndef TC_LUA_H
#define TC_LUA_H

#include "tinycoffee.h"

#include "external/lua/src/lauxlib.h"
#include "external/lua/src/lua.h"
#include "external/lua/src/lualib.h"


typedef struct {
  lua_State *L;
} tc_lua;

TCDEF tc_lua tc_init_lua();

TCDEF int luaopen_tinycoffee(lua_State *L);

TCDEF void tc_lua_update(tc_lua lua);
TCDEF void tc_lua_draw(tc_lua lua);

#endif /* TC_LUA_H */

#if defined(TC_LUA_IMPLEMENTATION)

TCDEF tc_lua tc_init_lua() {
  tc_lua lua;
  lua.L = luaL_newstate();

  return lua;
}

TCDEF void tc_lua_update(tc_lua lua) {}
TCDEF void tc_lua_draw(tc_lua lua) {}

TCDEF int luaopen_tinycoffee(lua_State *L) {
  
}

#endif
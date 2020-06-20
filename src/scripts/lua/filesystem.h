#ifndef TICO_LUA_FILESYSTEM_H
#define TICO_LUA_FILESYSTEM_H

#include "../../tinycoffee.h"

TCDEF int luaopen_filesystem(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tc_lua_mkdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tc_mkdir(str);
  return 0;
}

static int tc_lua_rmdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tc_rmdir(str);
  return 0;
}

static int tc_lua_read_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  char *buffer = tc_read_file(filename, NULL);

  lua_pushstring(L, buffer);
  return 1;
}

static int tc_lua_write_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  const char *text = luaL_checkstring(L, 2);
  const char *mode = luaL_optstring(L, 3, "w");
  int writeMode = TC_WRITE;
  if (!strcmp(mode, "a")) writeMode = TC_APPEND;
  if (strchr(mode, 'b')) writeMode |= TC_BINARY;
  tc_write_file(filename, text, strlen(text), writeMode);

  return 0;
}

static int tc_lua_file_exists(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  int exists = tc_file_exists(filename);

  lua_pushboolean(L, exists);
  return 1;
}

static int tc_lua_filesystem_set_path(lua_State* L) {
  const char *path = luaL_checkstring(L, 1);
  tc_filesystem_set_path(path);
  return 0;
}

static int tc_lua_filesystem_get_path(lua_State *L) {
  lua_pushstring(L, tc_filesystem_get_path());
  return 1;
}

static int tc_lua_filesystem_resolve_path(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  char out[512];
  tc_resolve_path(out, str);
  lua_pushstring(L, out);
  return 1;
}


TCDEF int luaopen_filesystem(lua_State *L) {
  luaL_Reg reg[] = {
    {"read", tc_lua_read_file},
    {"write", tc_lua_write_file},
    {"mkdir", tc_lua_mkdir},
    {"rmdir", tc_lua_rmdir},
    {"fileExists", tc_lua_file_exists},
    {"setPath", tc_lua_filesystem_set_path},
    {"getPath", tc_lua_filesystem_get_path},
    {"resolvePath", tc_lua_filesystem_resolve_path},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

#ifndef TICO_LUA_FILESYSTEM_H
#define TICO_LUA_FILESYSTEM_H

#include "../../tico.h"
#include "cjson/cJSON.h"

int luaopen_filesystem(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

// #define RECTANGLE_CLASS LUA_CLASS(tc_Rectf)

static int tico_lua_mkdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tico_filesystem_mkdir(str);
  return 0;
}

static int tico_lua_rmdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tico_filesystem_rmdir(str);
  return 0;
}

static int tico_lua_read_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  char *buffer = tico_filesystem_read_file(filename, NULL);

  lua_pushstring(L, buffer);
  return 1;
}

static int tico_lua_write_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  const char *text = luaL_checkstring(L, 2);
  const char *mode = luaL_optstring(L, 3, "w");
  int writeMode = TIC_WRITE;
  if (!strcmp(mode, "a")) writeMode = TIC_APPEND;
  if (strchr(mode, 'b')) writeMode |= TIC_BINARY;
  tico_filesystem_write_file(filename, text, strlen(text), writeMode);

  return 0;
}

static int tico_lua_file_exists(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  // TRACELOG("exists: %d", 0);
  int exists = tico_filesystem_file_exists(filename);

  lua_pushboolean(L, exists);
  return 1;
}

static int tico_lua_filesystem_set_path(lua_State* L) {
  const char *path = luaL_checkstring(L, 1);
  tico_filesystem_set_path(path);
  return 0;
}

static int tico_lua_filesystem_get_path(lua_State *L) {
  lua_pushstring(L, tico_filesystem_get_path());
  return 1;
}

static int tico_lua_filesystem_get_exe_path(lua_State *L) {
  lua_pushstring(L, tico_filesystem_get_exe_path());
  return 1;
}

static int tico_lua_filesystem_resolve_path(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  char out[512];
  tico_filesystem_resolve_path(out, str);
  lua_pushstring(L, out);
  return 1;
}


int luaopen_filesystem(lua_State *L) {
  luaL_Reg reg[] = {
    {"read", tico_lua_read_file},
    {"write", tico_lua_write_file},
    {"mkdir", tico_lua_mkdir},
    {"rmdir", tico_lua_rmdir},
    {"fileExists", tico_lua_file_exists},
    {"setPath", tico_lua_filesystem_set_path},
    {"getPath", tico_lua_filesystem_get_path},
    {"exePath", tico_lua_filesystem_get_exe_path},
    {"resolvePath", tico_lua_filesystem_resolve_path},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

#ifndef TICO_LUA_FILESYSTEM_H
#define TICO_LUA_FILESYSTEM_H

#include "../../tico.h"

int luaopen_filesystem(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tic_lua_mkdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tic_filesystem_mkdir(str);
  return 0;
}

static int tic_lua_rmdir(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  tic_filesystem_rmdir(str);
  return 0;
}

static int tic_lua_read_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  char *buffer = tic_filesystem_read_file(filename, NULL);

  lua_pushstring(L, buffer);
  return 1;
}

static int tic_lua_write_file(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  const char *text = luaL_checkstring(L, 2);
  const char *mode = luaL_optstring(L, 3, "w");
  int writeMode = TIC_WRITE;
  if (!strcmp(mode, "a")) writeMode = TIC_APPEND;
  if (strchr(mode, 'b')) writeMode |= TIC_BINARY;
  tic_filesystem_write_file(filename, text, strlen(text), writeMode);

  return 0;
}

static int tic_lua_file_exists(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  int exists = tic_filesystem_file_exists(filename);

  lua_pushboolean(L, exists);
  return 1;
}

static int tic_lua_filesystem_set_path(lua_State* L) {
  const char *path = luaL_checkstring(L, 1);
  tic_filesystem_set_path(path);
  return 0;
}

static int tic_lua_filesystem_get_path(lua_State *L) {
  lua_pushstring(L, tic_filesystem_get_path());
  return 1;
}

static int tic_lua_filesystem_get_exe_path(lua_State *L) {
  lua_pushstring(L, tic_filesystem_get_exe_path());
  return 1;
}

static int tic_lua_filesystem_resolve_path(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  char out[512];
  tic_filesystem_resolve_path(out, str);
  lua_pushstring(L, out);
  return 1;
}


int luaopen_filesystem(lua_State *L) {
  luaL_Reg reg[] = {
    {"read", tic_lua_read_file},
    {"write", tic_lua_write_file},
    {"mkdir", tic_lua_mkdir},
    {"rmdir", tic_lua_rmdir},
    {"fileExists", tic_lua_file_exists},
    {"setPath", tic_lua_filesystem_set_path},
    {"getPath", tic_lua_filesystem_get_path},
    {"exePath", tic_lua_filesystem_get_exe_path},
    {"resolvePath", tic_lua_filesystem_resolve_path},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

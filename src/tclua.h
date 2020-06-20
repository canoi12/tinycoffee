#ifndef TICO_LUA_H
#define TICO_LUA_H

#include "tinycoffee.h"

#ifndef LUAJIT
  #include "external/lua/src/lua.h"
  #include "external/lua/src/lauxlib.h"
  #include "external/lua/src/lualib.h"
#else
  #include "external/luajit/src/lua.h"
  #include "external/luajit/src/lauxlib.h"
  #include "external/luajit/src/lualib.h"
  #include "external/luajit/src/luaconf.h"
  #include "external/luajit/src/luajit.h"
#endif

typedef struct {
  lua_State *L;
  tc_bool mainLoaded;
} tc_Lua;

#ifdef LUAJIT
  void *luaL_testudata(lua_State *L, int index, const char *name);
  int luaL_setmetatable(lua_State *L, const char *name);
  int luaL_setfuncs(lua_State *L, luaL_Reg *reg, int n);
#endif

TCDEF tc_bool tc_init_lua(tc_Config *config);

TCDEF void tc_lua_load();
TCDEF void tc_lua_step();

TCDEF int tc_lua_preload(const char *modName, const char *modCode);
TCDEF int tc_lua_gc(int what, int data);
TCDEF int tc_lua_traceback(lua_State *L);


#include "scripts/lua/graphics.h"

TCDEF int tc_luaopen_tico(lua_State *L);

#endif /* TICO_LUA_H */

#if defined(TICO_LUA_IMPLEMENTATION)

static tc_Rect lua_optrect_table(lua_State *l, int index, tc_Rect opt) {
  tc_Rect rect = opt;
  if (lua_istable(l, index)) {
    int i = 0;
    lua_pushnil(l);
    while(lua_next(l, -2)) {
      rect.data[i] = lua_tonumber(l, -1);
      lua_pop(l, 1);
      ++i;
    }
  }

  return rect;
}

#include "libs/lua_libs.h"

#include "scripts/lua/graphics.h"
#include "scripts/lua/audio.h"
#include "scripts/lua/input.h"
#include "scripts/lua/math.h"
#include "scripts/lua/filesystem.h"
#include "scripts/lua/ui.h"
#include "scripts/lua/timer.h"

#ifdef LUAJIT
void *luaL_testudata(lua_State *L, int index, const char *name) {
  if (!lua_isuserdata(L, 1)) return NULL;
  void *data = lua_touserdata(L, 1);
  int er = lua_getmetatable(L, 1);
  if (er == 0) return NULL;
  luaL_getmetatable(L, name);
  if (lua_equal(L, -1, -2)) return data;
  return NULL;
}

int luaL_setmetatable(lua_State *L, const char *name) {
  luaL_getmetatable(L, name);
  lua_setmetatable(L, -2);
  return 0;
}
#endif

TCDEF int tc_lua_preload(const char *modName, const char *modCode) {
  size_t size = strlen(modCode) + 100;
  char mod[size];
  sprintf(mod, "package.preload['%s'] = function() %s end", modName, modCode);
  return luaL_dostring(Core.lua.L, mod);
}

TCDEF tc_bool tc_init_lua(tc_Config *config) {
//   tc_Lua lua = {0};
  Core.lua.L = luaL_newstate();
  Core.lua.mainLoaded = tc_false;

  luaL_openlibs(Core.lua.L);
  luaL_requiref(Core.lua.L, "tico", tc_luaopen_tico, 1);
//   tc_luaopen_tico(Core.lua.L);

  int lerr = luaL_loadbuffer(Core.lua.L, lua_Init, sizeof(lua_Init), "init.lua");
  int err = lua_pcall(Core.lua.L, 0, 0, 0);
  if (lerr || err) {
    const char *error = lua_tostring(Core.lua.L, -1);
    ERRLOG("Failed to init tico, error: %s", error);
    return tc_false;
  }
  Core.lua.mainLoaded = tc_true;

//   if (tc_file_exists("main.lua")) {
//     size_t size;
//     char *buffer = tc_read_file("main.lua", &size);

//     int err = luaL_loadbuffer(Core.lua.L, buffer, size, "main.lua");
//     int cerr = lua_pcall(Core.lua.L, 0, 0, 0);
//     if (err || cerr) {
//       lua_replace(Core.lua.L, 1);
//       TRACELOG("%d", lua_type(Core.lua.L, 1));
//       tc_lua_traceback(Core.lua.L);
//       const char *error = lua_tostring(Core.lua.L, -1);
//       ERRLOG("Failed to load 'main.lua', error: %s", error);
//       free(buffer);
//       return tc_false;
//     }
//     Core.lua.mainLoaded = tc_true;

//     TRACELOG("lua module loaded");
//   } else {
//     TRACEERR("main.lua not found");
//     return tc_false;
//   }

  return tc_true;
}

TCDEF void tc_lua_load() {
//   if (!Core.lua.mainLoaded) return;
  lua_State *L = Core.lua.L;
  lua_getglobal(L, "tico");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, "_load");
    if (!lua_isnil(L, -1)) {
      int err = lua_pcall(L, 0, 0, 0);
      if (err) {
//         tc_lua_traceback(L);
        const char *str = lua_tostring(L, -1);
        ERRLOG("Failed to run tico.load, error: %s", str);
        exit(1);
      }
      lua_pop(L, 1);
    }
  }
}

TCDEF void tc_lua_step() {
  lua_State *L = Core.lua.L;
  lua_getglobal(L, "tico");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, "_step");
    if (!lua_isnil(L, -1)) {
      int err = lua_pcall(L, 0, 0, 0);
      if (err) {
//         tc_lua_traceback(L);
        const char *str = lua_tostring(L, -1);
        ERRLOG("error in tico: %s", str);
      }
    }
    lua_pop(L, 1);
  }
}

static int lua_get_tico_version(lua_State *L) {
  lua_pushstring(L, TICO_VERSION);
  return 1;
}

static int tc_lua_load_module(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);

  char rname[512];
  strcpy(rname, name);
  tc_replace_char(rname, '.', '/');
  strcat(rname, ".lua");
  size_t size;
  unsigned char *buffer;
  if (tc_file_exists(rname)) {
    buffer = tc_read_file(rname, &size);
  } else {
    strcpy(rname, name);
    tc_replace_char(rname, '.', '/');
    strcat(rname, "/init.lua");
    buffer = tc_read_file(rname, &size);
  }
  if (buffer) {
    luaL_loadbuffer(L, buffer, size, name);
    free(buffer);
    return 1;
  }
  strcpy(rname, name);
  tc_replace_char(rname, '.', '/');
  char error[2048];
  sprintf(error, "\n\tno file '%s/%s.lua'\n\tno file '%s/%s/init.lua'", Core.config.path, rname, Core.config.path, rname);
  lua_pushstring(L, error);
//   tc_lua_traceback(L);
  return 1;
}

TCDEF int tc_lua_gc(int what, int data) {
  lua_gc(Core.lua.L, what, data);
  return 0;
}

TCDEF int tc_lua_traceback(lua_State *L) {
//   const char *str = lua_tostring(L, 1);
  if (!lua_isstring(L, 1)) return 1;
  lua_getglobal(L, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }

  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 1);
  lua_call(L, 2, 1);
  return 1;
}

TCDEF int tc_luaopen_tico(lua_State *L) {
  tc_lua_preload("tico.class", lua_Class);
  tc_lua_preload("tico.utils", lua_Utils);

  lua_getglobal(L, "package");
  lua_getfield(L, -1, "searchers");
//   lua_getfield(L, -1, "loaders");
  lua_remove(L, -2);

  lua_pushinteger(L, 5);
  lua_pushcfunction(L, tc_lua_load_module);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  luaL_Reg reg[] = {
    {"getVersion", lua_get_tico_version},
    {NULL, NULL}
  };
  luaL_newlib(L, reg);
//   luaL_register(L, "tico", reg);

  struct { char *name; int (*fn)(lua_State*); } libs[] = {
    {"Image", luaopen_image},
    {"Canvas", luaopen_canvas},
    {"Sound", luaopen_sound},
    {"Rectangle", luaopen_rectangle},
    {"Font", luaopen_font},
    {"graphics", luaopen_graphics},
    {"filesystem", luaopen_filesystem},
    {"input", luaopen_input},
    {"audio", luaopen_audio},
    {"math", luaopen_tcmath},
    {"timer", luaopen_timer},
    {"ui", luaopen_ui},
    {NULL, NULL}
  };

  for (int i = 0; libs[i].name; i++) {
    libs[i].fn(L);
    lua_setfield(L, -2, libs[i].name);
  }

  return 1;
}

#endif /* TICO_LUA_IMPLEMENTATION */

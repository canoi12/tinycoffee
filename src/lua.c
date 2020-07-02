#include "tico.h"

// #ifndef LUAJIT
//   #define lua_newuserdata lua_newuserdata
// #endif

#include "scripting/lua/init.lua.h"
#include "scripting/lua/class.lua.h"
#include "scripting/lua/utils.lua.h"
#include "scripting/lua/sprite.lua.h"
#include "scripting/lua/vector.lua.h"
#include "scripting/lua/color.lua.h"

// static const char *color_lua =
// "local color = {}\n"
// "color.White = {255, 255, 255}\n"
// "color.Black = {0, 0, 0}\n"
// "color.Red = {208, 70, 72}\n"
// "function color.rgb(r, g, b) return {r, g, b} end\n"
// "return color";

#define TICO_LUA_IMPLEMENTATION
#include "scripting/lua/audio.h"
#include "scripting/lua/filesystem.h"
#include "scripting/lua/graphics.h"
#include "scripting/lua/input.h"
#include "scripting/lua/math.h"
#include "scripting/lua/timer.h"
#include "scripting/lua/window.h"

#ifdef LUAJIT
void luaL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb) {
  luaL_checkstack(L, 3, "not enought space remaining");
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  lua_getfield(L, -1, modname);
  if (lua_type(L, -1) == LUA_TNIL) {
    lua_pop(L, 1);
    lua_pushcfunction(L, openf);
    lua_pushstring(L, modname);
    lua_call(L, 1, 1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, modname);
  }
  if (glb) {
    lua_pushvalue(L, -1);
    lua_setglobal(L, modname);
  }
  lua_replace(L, -2);
}

#endif

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

int tic_lua_preload(const char *modName, const char *modCode, int bufSize) {
  size_t size = bufSize + 100;
  char sz[bufSize + 1];
  strcpy(sz, modCode);
  sz[bufSize] = '\0';
  char mod[size];
  sprintf(mod, "package.preload['%s'] = function() %s end", modName, sz);
  return luaL_dostring(Core.lua.L, mod);
}

tc_bool tic_lua_init(tc_Config *config) {
//   tc_Lua lua = {0};
  Core.lua.L = luaL_newstate();
  Core.lua.mainLoaded = tc_false;

  luaL_openlibs(Core.lua.L);
#ifdef LUAJIT
  luaopen_jit(Core.lua.L);
#endif
  luaopen_bit(Core.lua.L);
//   api_load_libs(Core.lua.L);
  luaL_requiref(Core.lua.L, "tico", luaopen_tico, 1);

  lua_getglobal(Core.lua.L, "tico");
  lua_newtable(Core.lua.L);
  for (int i = 0; i < config->argc; i++) {
    lua_pushstring(Core.lua.L, config->argv[i]);
    lua_rawseti(Core.lua.L, -2, i + 1);
  }
  lua_setfield(Core.lua.L, -2, "args");
//   tc_luaopen_tico(Core.lua.L);

  // size_t size;
  // const char *lua_init = tic_filesystem_read_file("/home/canoi/Documentos/Projects/engine-tests/tico/src/wrap/lua/scripts/init.lua", &size);
  // int lerr = luaL_loadbuffer(Core.lua.L, lua_init, size, "init.lua");
  int lerr = luaL_loadbuffer(Core.lua.L, init_lua, sizeof(init_lua), "init.lua");
  // free((void*)lua_init);
  int err = lua_pcall(Core.lua.L, 0, 0, 0);
  if (lerr || err) {
    const char *error = lua_tostring(Core.lua.L, -1);
    ERRLOG("Failed to init tico, error: %s", error);
    return tc_false;
  }
  Core.lua.mainLoaded = tc_true;

  return tc_true;
}

int tic_lua_load() {
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
      }
      lua_pop(L, 1);
    }
  }
  return 1;
}

int tic_lua_step() {
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
    lua_settop(L, 1);
  }
  return 1;
}

void tic_lua_callback(const char *name) {
  lua_State *L = Core.lua.L;
  lua_getglobal(L, "tico");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, "_doCallback");
    if (!lua_isnil(L, -1)) {
      lua_pushstring(L, name);
      int err = lua_pcall(L, 1, 0, 0);
      if (err) {
        const char *str = lua_tostring(L, -1);
        TRACELOG("%s", str);
      }
    }
    lua_pop(L, 1);
  }
}

static int lua_get_tico_version(lua_State *L) {
  lua_pushstring(L, TICO_VERSION);
  return 1;
}

static int lua_get_tico_mode(lua_State *L) {
  lua_pushstring(L, Core.state.mode == TIC_MODE_GAME ? "game" : "editor");
  return 1;
}

static int tic_lua_load_module(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);

  char rname[512];
  strcpy(rname, name);
  tic_replace_char(rname, '.', '/');
  strcat(rname, ".lua");
  size_t size;
  unsigned char *buffer;
  if (tic_filesystem_file_exists(rname)) {
    buffer = tic_filesystem_read_file(rname, &size);
  } else {
    strcpy(rname, name);
    tic_replace_char(rname, '.', '/');
    strcat(rname, "/init.lua");
    buffer = tic_filesystem_read_file(rname, &size);
  }
  if (buffer) {
    luaL_loadbuffer(L, buffer, size, name);
    TIC_FREE(buffer);
    return 1;
  }
  strcpy(rname, name);
  tic_replace_char(rname, '.', '/');
  char error[2048];
  sprintf(error, "\n\tno file '%s/%s.lua'\n\tno file '%s/%s/init.lua'", Core.config.path, rname, Core.config.path, rname);
  lua_pushstring(L, error);
//   tc_lua_traceback(L);
  return 1;
}

int luaopen_tico(lua_State *L) {
  tic_lua_preload("tico.class", class_lua, class_lua_size);
  tic_lua_preload("tico.utils", utils_lua, utils_lua_size);
  tic_lua_preload("tico.color", color_lua, color_lua_size);
  tic_lua_preload("tico.sprite", sprite_lua, sprite_lua_size);
  tic_lua_preload("tico.vector", vector_lua, vector_lua_size);

  lua_getglobal(L, "package");
#ifdef LUAJIT
  lua_getfield(L, -1, "loaders");
#else
  lua_getfield(L, -1, "searchers");
#endif
  lua_remove(L, -2);

  lua_pushinteger(L, 5);
  lua_pushcfunction(L, tic_lua_load_module);
  lua_rawset(L, -3);
  lua_pop(L, 1);

  luaL_Reg reg[] = {
    {"getVersion", lua_get_tico_version},
    {"mode", lua_get_tico_mode},
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
    {"Shader", luaopen_shader},
    {"graphics", luaopen_graphics},
    {"filesystem", luaopen_filesystem},
    {"input", luaopen_input},
    {"audio", luaopen_audio},
    {"math", luaopen_tcmath},
    {"timer", luaopen_timer},
    {"window", luaopen_window},
    // {"ui", luaopen_ui},
    {NULL, NULL}
  };

  for (int i = 0; libs[i].name; i++) {
    libs[i].fn(L);
    lua_setfield(L, -2, libs[i].name);
  }

  return 1;
}

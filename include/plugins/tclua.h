#ifndef TICO_LUA_H
#define TICO_LUA_H

#include "../tico.h"

#define SOUND_CLASS LUA_CLASS(tc_Sound)

#define TEXTURE_CLASS LUA_CLASS(tc_Texture)
#define CANVAS_CLASS LUA_CLASS(tc_Canvas)
#define IMAGE_CLASS LUA_CLASS(tc_Image)
#define RECTANGLE_CLASS LUA_CLASS(tc_Rectf)
#define FONT_CLASS LUA_CLASS(tc_Font)
#define SHADER_CLASS LUA_CLASS(tc_Shader)

#define TILESET_CLASS LUA_CLASS(tc_Tileset)
#define TILEMAP_CLASS LUA_CLASS(tc_Tilemap)
#define CAMERA_CLASS LUA_CLASS(tc_Camera)
#define SPRITE_CLASS LUA_CLASS(tc_Sprite)

#define RESOURCE_CLASS LUA_CLASS(tc_Resource)

typedef int(*LuaFunction)(lua_State*);

typedef struct tc_Lua {
	lua_State *L;
} tc_Lua;

TIC_API int tico_plugin_lua_load(tc_Lua *lua);
TIC_API int tico_plugin_lua_update(tc_Lua *lua);
TIC_API int tico_plugin_lua_draw(tc_Lua *lua);

TIC_API lua_State* PUBLIC_API(tico_plugin_lua_get_state);
// TIC_API lua_State* PUBLIC_API(tico_plugin_lua_get_state, tc_Lua *lua);

TIC_API void PUBLIC_API(tico_plugin_lua_add_lib, const char *name, LuaFunction function);
TIC_API void INTERNAL_API(tico_plugin_lua_add_lib, tc_Lua *lua, const char *name, LuaFunction function);
TIC_API void PUBLIC_API(tico_plugin_lua_pcall, const char *name, int n);
TIC_API void INTERNAL_API(tico_plugin_lua_pcall, tc_Lua *lua, const char *name, int n);

TIC_API int luaopen_tico(lua_State *L);

#endif

#if defined(TICO_PLUGIN_IMPLEMENTATION)

int lua_isarray(lua_State *L, int index) {
  lua_len(L, index);
  int len = lua_tonumber(L, -1);
  lua_pop(L, 1);
  if (len > 0) {
    return 1;
  }
  return 0;
}

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

static tc_Color lua_optcolor(lua_State *l, int index, tc_Color opt) {
  tc_Color color = opt;
  if (lua_istable(l, index)) {
    lua_len(l, index);
    int count = tico_min(lua_tonumber(l, -1), 4);
    lua_pop(l, 1);
    for (int i = 0; i < count; i++) {
      lua_rawgeti(l, index, i+1);
      color.data[i] = lua_tonumber(l, -1);
      lua_pop(l, 1);
    }
  }

  return color;
}

#define TICO_LUA_IMPLEMENTATION
#include "lua/init.lua.h"
#include "lua/filesystem.h"
#include "lua/graphics.h"
#include "lua/input.h"
#include "lua/audio.h"
#include "lua/math.h"
#include "lua/timer.h"
#include "lua/window.h"
#include "lua/json.h"
#include "lua/engine.h"
#include "lua/imgui.h"

int tico_plugin_lua_enable(tc_Lua *lua) {
	lua->L = luaL_newstate();
	luaL_openlibs(lua->L);

	luaL_requiref(lua->L, "tico", luaopen_tico, 1);
	if (!tico_filesystem_file_exists("main.lua")) return 0;

  // lua_getglobal(lua->L, "tico");
  // lua_newtable(lua->L);
  // for (int i = 0; i < config->argc; i++) {
  //   lua_pushstring(lua->L, config->argv[i]);
  //   lua_rawseti(lua->L, -2, i + 1);
  // }

	// int lerr = luaL_loadfile(lua->L, "main.lua");
  // free((void*)lua_init);
  int lerr = luaL_loadbuffer(lua->L, init_lua, init_lua_size, "init.lua");
	// int lerr = luaL_dofile(lua->L, "init.lua");
  int err = lua_pcall(lua->L, 0, 0, 0);
  // int err = 0;
  if (lerr || err) {
    const char *error = lua_tostring(lua->L, -1);
    ERRLOG("Failed to init tico, error: %s", error);
    return tc_false;
  }

	return tc_true;
}

int tico_plugin_lua_load(tc_Lua *lua) {
	lua_State *L = lua->L;
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
}

int tico_plugin_lua_update(tc_Lua *lua) {

}

int tico_plugin_lua_draw(tc_Lua *lua) {
	lua_State *L = lua->L;
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

lua_State* PUBLIC_API(tico_plugin_lua_get_state) {
  tc_Plugin *plugin = tico_plugin_get("lua");
  if (!plugin) return NULL;
  tc_Lua *lua = plugin->data;

  return lua->L;
}

void PUBLIC_API(tico_plugin_lua_add_lib, const char *name, LuaFunction function) {
	tc_Plugin *plugin = tico_plugin_get("lua");
	if (!plugin) return;
	INTERNAL_API(tico_plugin_lua_add_lib, plugin->data, name, function);
}

void INTERNAL_API(tico_plugin_lua_add_lib, tc_Lua *lua, const char *name, LuaFunction function) {
	ASSERT(lua != NULL);
	ASSERT(name != NULL);
	ASSERT(function != NULL);
	lua_State *L = lua->L;
	lua_getglobal(L, "tico");

	LOG("Lua lib: %s added", name);
	
	function(L);
	lua_setfield(L, -2, name);
}

void PUBLIC_API(tico_plugin_lua_pcall, const char *name, int n) {
  tc_Plugin *plugin = tico_plugin_get("lua");
  if (!plugin) return;
  INTERNAL_API(tico_plugin_lua_pcall, plugin->data, name, n);
}

void INTERNAL_API(tico_plugin_lua_pcall, tc_Lua *lua, const char *name, int n) {
  if (!lua) return;
  lua_State *L = lua->L;
  if (!L) return;
  int top = lua_gettop(L);
  lua_getglobal(L, "tico");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, name);
    if (!lua_isnil(L, -1)) {
      // lua_pushnumber(L, tico_timer_get_delta());
      for (int i = n-1; i >= 0; i--) {
        lua_pushvalue(L, top-i);
      }
      int err = lua_pcall(L, n, 0, 0);
      if (err) {
//         tc_lua_traceback(L);
        const char *str = lua_tostring(L, -1);
        ERRLOG("error in tico: %s", str);
      }
    }
    lua_pop(L, 1);
    lua_settop(L, 1);
  }
}

static int lua_get_tico_version(lua_State *L) {
	lua_pushstring(L, TICO_VERSION);
	return 1;
}

int luaopen_tico(lua_State *L) {
	luaL_Reg reg[] = {
		{"getVersion", lua_get_tico_version},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);

	struct { char *name; int (*fn)(lua_State*); } libs[] = {
		{"Image", luaopen_image},
    {"Canvas", luaopen_canvas},
    {"Sound", luaopen_sound},
    {"Rectangle", luaopen_rectangle},
    {"Font", luaopen_font},
    {"Shader", luaopen_shader},
    {"Tileset", luaopen_tileset},
    {"Tilemap", luaopen_tilemap},
    {"Camera", luaopen_camera},
    {"Sprite", luaopen_sprite},
    {"json", luaopen_json},
    {"filesystem", luaopen_filesystem},
    {"graphics", luaopen_graphics},
    {"input", luaopen_input},
    {"imgui", luaopen_imgui},
    {"audio", luaopen_audio},
    {"timer", luaopen_timer},
    {"math", luaopen_tcmath},
    {"window", luaopen_window},
    {"engine", luaopen_engine},
    {NULL, NULL}
  };

  for (int i = 0; libs[i].name; i++) {
    libs[i].fn(L);
    lua_setfield(L, -2, libs[i].name);
  }

	return 1;
}

#endif
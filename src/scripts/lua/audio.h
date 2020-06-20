#ifndef TICO_LUA_AUDIO_H
#define TICO_LUA_AUDIO_H

#define SOUND_CLASS "Sound"

#include "../../tinycoffee.h"

TCDEF int luaopen_audio(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tc_lua_new_sound(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  tc_Sound *sound = lua_newuserdata(L, sizeof(tc_Sound));
  luaL_setmetatable(L, SOUND_CLASS);
  *sound = tc_load_sound(filename, 0);

  return 1;
}

static int tc_lua_play_sound(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  tc_play_sound(*sound);

  return 0;
}

static int tc_lua_stop_sound(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  tc_stop_sound(*sound);

  return 0;
}

static int tc_lua_pause_sound(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  tc_pause_sound(*sound);

  return 0;
}

static int tc_lua_sound_volume(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  float volume = luaL_optnumber(L, 2, sound->audioBuffer->volume);
  tc_sound_set_volume(*sound, volume);

  lua_pushnumber(L, volume);
  return 1;
}

static int tc_lua_sound_is_playing(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  int playing = tc_sound_is_playing(*sound);
  lua_pushboolean(L, playing);
  return 1;
}

static int tc_lua_sound_is_paused(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  int paused = tc_sound_is_paused(*sound);
  lua_pushboolean(L, paused);
  return 1;
}

static int tc_lua_destroy_sound(lua_State *L) {
  tc_Sound *sound = luaL_checkudata(L, 1, SOUND_CLASS);
  tc_unload_sound(*sound);

  return 0;
}

static int luaopen_sound(lua_State *L) {
  luaL_Reg reg[] = {
    {"play", tc_lua_play_sound},
    {"stop",  tc_lua_stop_sound},
    {"pause", tc_lua_pause_sound},
    {"volume", tc_lua_sound_volume},
    {"isPlaying", tc_lua_sound_is_playing},
    {"isPaused", tc_lua_sound_is_paused},
    {"__gc", tc_lua_destroy_sound},
    {NULL, NULL}
  };

  luaL_newmetatable(L, SOUND_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  return 1;
}

static int tc_lua_set_master_volume(lua_State *L) {
  float volume = luaL_checknumber(L, 1);
  tc_set_master_volume(volume);
  return 0;
}

TCDEF int luaopen_audio(lua_State *L) {
  luaL_Reg reg[] = {
    {"newSound", tc_lua_new_sound},
    {"setVolume", tc_lua_set_master_volume},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);
//   luaL_register(L, NULL, reg);
//   lua_pushvalue(L, -1);

  return 1;
}

#endif

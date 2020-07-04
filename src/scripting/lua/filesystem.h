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

static int tic_lua_json_new_object(lua_State *L, cJSON *json);

static int tic_lua_json_new_array(lua_State *L, cJSON *json) {
  lua_newtable(L);
  cJSON *item = NULL;
  int i = 1;
  cJSON_ArrayForEach(item, json) {
    switch(item->type) {
      case cJSON_String:
       {
        lua_pushstring(L, item->valuestring);
        lua_rawseti(L, -2, i++);
        break;
       }
      case cJSON_Number:
        {
          lua_pushnumber(L, item->valuedouble);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_False:
        {
          lua_pushboolean(L, 0);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_True:
        {
          lua_pushboolean(L, 1);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_NULL:
        {
          lua_pushnil(L);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_Array:
        {
          tic_lua_json_new_array(L, item);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_Object:
        {
          tic_lua_json_new_object(L, item);
          lua_rawseti(L, -2, i++);
          break;
        }
      default:
        printf("unknown json type\n");
    }
  }
}

int tic_lua_json_new_object(lua_State *L, cJSON *json) {
  lua_newtable(L);
  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {
    switch(item->type) {
      case cJSON_String:
        {
          lua_pushstring(L, item->valuestring);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_Number:
        {
          lua_pushnumber(L, item->valuedouble);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_NULL:
        {
          lua_pushnil(L);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_False:
        {
          lua_pushboolean(L, 0);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_True:
        {
          lua_pushboolean(L, 01);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_Array:
        {
          tic_lua_json_new_array(L, item);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_Object:
        {
          tic_lua_json_new_object(L, item);
          lua_setfield(L, -2, item->string);
          break;
        }
      default:
        printf("unknown json type\n");
    }
  }

  return 1;
}

static int tic_lua_json_decode(lua_State *L) {
  const char *jsonStr = luaL_checkstring(L, 1);
  cJSON *json = tic_json_parse(jsonStr);

  tic_lua_json_new_object(L, json);

  tic_json_delete(json);

  return 1;
}

static cJSON *tic_lua_json_new_cjson_object(lua_State *L, int index);

static cJSON *tic_lua_json_new_cjson_array(lua_State *L, int index) {
  cJSON *json = tic_json_create_array();
  lua_pushnil(L);
  while(lua_next(L, index)) {
    cJSON *item = NULL;
    switch(lua_type(L, -1)) {
      case LUA_TNUMBER:
        {
          float n = lua_tonumber(L, -1);
          item = cJSON_CreateNumber(n);
          break;
        }
      case LUA_TBOOLEAN:
        {
          int val = lua_toboolean(L, -1);
          item = cJSON_CreateBool(val);
          break;
        }
      case LUA_TSTRING:
        {
          const char *str = lua_tostring(L, -1);
          item = cJSON_CreateString(str);
          break;
        }
      case LUA_TTABLE:
        {
          if (lua_isarray(L, -1)) {
            int top = lua_gettop(L);
            item = tic_lua_json_new_cjson_array(L, top);
          } else {
            int top = lua_gettop(L);
            item = tic_lua_json_new_cjson_object(L, top);
          }
          break;
        }
        case LUA_TUSERDATA:
          {
            if (luaL_testudata(L, -1, RECTANGLE_CLASS)) {
              tc_Rectf *rect = lua_touserdata(L, -1);
              cJSON *robj = tic_json_create();
              tic_json_set_string(robj, "type", RECTANGLE_CLASS);
              tic_json_set_number(robj, "x", rect->x);
              tic_json_set_number(robj, "y", rect->y);
              tic_json_set_number(robj, "w", rect->w);
              tic_json_set_number(robj, "h", rect->h);
              cJSON_AddItemToArray(json, robj);
            }
          }
    }
    if (item) cJSON_AddItemToArray(json, item);
    lua_pop(L, 1);
  }
  // lua_pop(L, 1);

  return json;
}

cJSON *tic_lua_json_new_cjson_object(lua_State *L, int index) {
  cJSON *json = tic_json_create();
  lua_pushnil(L);
  while(lua_next(L, index)) {
    char *key = (char*)lua_tostring(L, -2);
    if (!key) {
      lua_pop(L, 1);
      continue;
    }
    switch(lua_type(L, -1)) {
      case LUA_TNUMBER:
        {
          float n = lua_tonumber(L, -1);
          tic_json_set_number(json, key, n);
          break;
        }
        case LUA_TBOOLEAN:
        {
          int val = lua_toboolean(L, -1);
          tic_json_set_boolean(json, key, val);
          break;
        }
        case LUA_TSTRING:
        {
          char *str = (char*)lua_tostring(L, -1);
          tic_json_set_string(json, key, str);
          break;
        }
        case LUA_TTABLE:
        {
          cJSON *item = NULL;
          if (strstr(key, "__")) break;
          if (lua_isarray(L, -1)) {
            int top = lua_gettop(L);
            item = tic_lua_json_new_cjson_array(L, top);
            tic_json_set_array(json, key, item);
          } else {
            int top = lua_gettop(L);
            item = tic_lua_json_new_cjson_object(L, top);
            tic_json_set_object(json, key, item);
          }
          break;
        }
        case LUA_TUSERDATA:
        {
          if (luaL_testudata(L, -1, RECTANGLE_CLASS)) {
            tc_Rectf *rect = lua_touserdata(L, -1);
            cJSON *robj = tic_json_create();
            tic_json_set_string(robj, "type", RECTANGLE_CLASS);
            tic_json_set_number(robj, "x", rect->x);
            tic_json_set_number(robj, "y", rect->y);
            tic_json_set_number(robj, "w", rect->w);
            tic_json_set_number(robj, "h", rect->h);
            tic_json_set_object(json, key, robj);
          }
        }
    }
    lua_pop(L, 1);
  }

  return json;
}

static int tic_lua_json_encode(lua_State *L) {
  cJSON *json = NULL;
  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_isarray(L, 1)) {
      json = tic_lua_json_new_cjson_array(L, 1);
    } else {
      json = tic_lua_json_new_cjson_object(L, 1);
    }

    lua_pushstring(L, tic_json_print(json));
    if (json) tic_json_delete(json);
  } else {
    lua_pushstring(L, "{}");
  }

  return 1;
}

static int tic_lua_json_load(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);

  cJSON *root = tic_json_open(filename);

  if (cJSON_IsArray(root)) tic_lua_json_new_array(L, root);
  else if (cJSON_IsObject(root)) tic_lua_json_new_object(L, root);

  return 1;
}

static int tic_lua_json_save(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  if (lua_type(L, 2) == LUA_TTABLE) {
    lua_remove(L, 1);
    tic_lua_json_encode(L);
    const char *jsonStr = lua_tostring(L, -1);
    tic_filesystem_write_file(filename, jsonStr, strlen(jsonStr), 0);
  } else {
    lua_pushstring(L, "argument #2 must be a table");
    lua_error(L);
    return 1;
  }

  return 0;
}

int luaopen_json(lua_State *L) {
  luaL_Reg reg[] = {
    {"load", tic_lua_json_load},
    {"save", tic_lua_json_save},
    {"decode", tic_lua_json_decode},
    {"encode", tic_lua_json_encode},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);

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

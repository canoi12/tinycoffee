#ifndef TICO_LUA_JSON_H
#define TICO_LUA_JSON_H

#include "../../tico.h"
#include "cjson/cJSON.h"

TIC_API int luaopen_json(lua_State *L);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

static int tico_lua_json_new_object(lua_State *L, cJSON *json);

int tico_lua_json_new_array(lua_State *L, cJSON *json) {
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
          tico_lua_json_new_array(L, item);
          lua_rawseti(L, -2, i++);
          break;
        }
      case cJSON_Object:
        {
          tico_lua_json_new_object(L, item);
          lua_rawseti(L, -2, i++);
          break;
        }
      default:
        printf("unknown json type\n");
    }
  }
}

int tico_lua_json_new_object(lua_State *L, cJSON *json) {
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
          tico_lua_json_new_array(L, item);
          lua_setfield(L, -2, item->string);
          break;
        }
      case cJSON_Object:
        {
          tico_lua_json_new_object(L, item);
          lua_setfield(L, -2, item->string);
          break;
        }
      default:
        printf("unknown json type\n");
    }
  }

  return 1;
}

static int tico_lua_json_decode(lua_State *L) {
  const char *jsonStr = luaL_checkstring(L, 1);
  cJSON *json = tico_json_parse(jsonStr);

  tico_lua_json_new_object(L, json);

  tico_json_delete(json);

  return 1;
}

static cJSON *tico_lua_json_new_cjson_object(lua_State *L, int index);

cJSON *tico_lua_json_new_cjson_array(lua_State *L, int index) {
  cJSON *json = tico_json_create_array();
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
            item = tico_lua_json_new_cjson_array(L, top);
          } else {
            int top = lua_gettop(L);
            item = tico_lua_json_new_cjson_object(L, top);
          }
          break;
        }
        case LUA_TUSERDATA:
          {
            if (luaL_testudata(L, -1, RECTANGLE_CLASS)) {
              tc_Rectf *rect = lua_touserdata(L, -1);
              cJSON *robj = tico_json_create();
              tico_json_set_string(robj, "type", RECTANGLE_CLASS);
              tico_json_set_number(robj, "x", rect->x);
              tico_json_set_number(robj, "y", rect->y);
              tico_json_set_number(robj, "w", rect->w);
              tico_json_set_number(robj, "h", rect->h);
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

cJSON *tico_lua_json_new_cjson_object(lua_State *L, int index) {
  cJSON *json = tico_json_create();
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
          tico_json_set_number(json, key, n);
          break;
        }
        case LUA_TBOOLEAN:
        {
          int val = lua_toboolean(L, -1);
          tico_json_set_boolean(json, key, val);
          break;
        }
        case LUA_TSTRING:
        {
          char *str = (char*)lua_tostring(L, -1);
          tico_json_set_string(json, key, str);
          break;
        }
        case LUA_TTABLE:
        {
          cJSON *item = NULL;
          if (strstr(key, "__")) break;
          if (lua_isarray(L, -1)) {
            int top = lua_gettop(L);
            item = tico_lua_json_new_cjson_array(L, top);
            tico_json_set_array(json, key, item);
          } else {
            int top = lua_gettop(L);
            item = tico_lua_json_new_cjson_object(L, top);
            tico_json_set_object(json, key, item);
          }
          break;
        }
        case LUA_TUSERDATA:
        {
          if (luaL_testudata(L, -1, RECTANGLE_CLASS)) {
            tc_Rectf *rect = lua_touserdata(L, -1);
            cJSON *robj = tico_json_create();
            tico_json_set_string(robj, "type", RECTANGLE_CLASS);
            tico_json_set_number(robj, "x", rect->x);
            tico_json_set_number(robj, "y", rect->y);
            tico_json_set_number(robj, "w", rect->w);
            tico_json_set_number(robj, "h", rect->h);
            tico_json_set_object(json, key, robj);
          }
        }
    }
    lua_pop(L, 1);
  }

  return json;
}

static int tico_lua_json_encode(lua_State *L) {
  cJSON *json = NULL;
  if (lua_type(L, 1) == LUA_TTABLE) {
    if (lua_isarray(L, 1)) {
      json = tico_lua_json_new_cjson_array(L, 1);
    } else {
      json = tico_lua_json_new_cjson_object(L, 1);
    }

    lua_pushstring(L, tico_json_print(json));
    if (json) tico_json_delete(json);
  } else {
    lua_pushstring(L, "{}");
  }

  return 1;
}

static int tico_lua_json_load(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);

  cJSON *root = tico_json_open(filename);

  if (cJSON_IsArray(root)) tico_lua_json_new_array(L, root);
  else if (cJSON_IsObject(root)) tico_lua_json_new_object(L, root);

  return 1;
}

static int tico_lua_json_save(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  if (lua_type(L, 2) == LUA_TTABLE) {
    lua_remove(L, 1);
    tico_lua_json_encode(L);
    const char *jsonStr = lua_tostring(L, -1);
    tico_filesystem_write_file(filename, jsonStr, strlen(jsonStr), 0);
  } else {
    lua_pushstring(L, "argument #2 must be a table");
    lua_error(L);
    return 1;
  }

  return 0;
}

int luaopen_json(lua_State *L) {
  luaL_Reg reg[] = {
    {"load", tico_lua_json_load},
    {"save", tico_lua_json_save},
    {"decode", tico_lua_json_decode},
    {"encode", tico_lua_json_encode},
    {NULL, NULL}
  };

  luaL_newlib(L, reg);

  return 1;
}

#endif
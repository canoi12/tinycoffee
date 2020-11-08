#ifndef TICO_LUA_JSON_H
#define TICO_LUA_JSON_H

#include "../../tico.h"
#include "cjson/cJSON.h"

TIC_API int luaopen_json(lua_State *L);

TIC_API int tico_lua_field_new_array(lua_State *L, tc_Field *field);
TIC_API int tico_lua_field_new_object(lua_State *L, tc_Field *field);
TIC_API tc_Field* tico_lua_new_field_array(lua_State *L, int index);
TIC_API tc_Field* tico_lua_new_field_object(lua_State *L, int index);

TIC_API int tico_lua_json_new_object(lua_State *L, cJSON *json);

#endif

#if defined(TICO_LUA_IMPLEMENTATION)

int tico_lua_vecn(lua_State *L, int n, float *data) {
  lua_newtable(L);
  for (int i = 0; i < n; i++) {
    lua_pushnumber(L, data[i]);
    lua_rawseti(L, -2, i);
  }

  return 1;
}

// int tico_lua_field_new_array(lua_State *L, tc_Field *field) {
//   lua_newtable(L);
//   tc_Field *item = NULL;
//   int i = 1;
//   tico_field_foreach(item, field) {
//     switch (item->type) {
//       case TIC_FIELD_RESOURCE:
//       case TIC_FIELD_STRING:
//       {
//         lua_pushstring(L, item->string);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_NUMBER:
//       {
//         lua_pushnumber(L, item->number);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_BOOLEAN:
//       {
//         lua_pushboolean(L, item->boolean);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_VEC2:
//       {
//         tico_lua_vecn(L, 2, item->vec2.data);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_VEC3:
//       {
//         tico_lua_vecn(L, 2, item->vec3.data);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_RECT:
//       case TIC_FIELD_VEC4:
//       {
//         tico_lua_vecn(L, 2, item->vec4.data);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_ARRAY:
//       {
//         tico_lua_field_new_array(L, item);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//       case TIC_FIELD_OBJECT:
//       {
//         tico_lua_field_new_object(L, item);
//         lua_rawseti(L, -2, i++);
//         break;
//       }
//     }
//   }
// }

// int tico_lua_field_new_object(lua_State *L, tc_Field *field) {
//   lua_newtable(L);
//   tc_Field *item = NULL;
//   tico_field_foreach(item, field) {
//     switch (item->type) {
//       case TIC_FIELD_RESOURCE:
//       case TIC_FIELD_STRING:
//       {
//         lua_pushstring(L, item->string);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_NUMBER:
//       {
//         lua_pushnumber(L, item->number);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_BOOLEAN:
//       {
//         lua_pushboolean(L, item->boolean);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_VEC2:
//       {
//         tico_lua_vecn(L, 2, item->vec2.data);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_VEC3:
//       {
//         tico_lua_vecn(L, 2, item->vec3.data);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_RECT:
//       case TIC_FIELD_VEC4:
//       {
//         tico_lua_vecn(L, 2, item->vec4.data);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_ARRAY:
//       {
//         tico_lua_field_new_array(L, item);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//       case TIC_FIELD_OBJECT:
//       {
//         tico_lua_field_new_object(L, item);
//         lua_setfield(L, -2, item->name);
//         break;
//       }
//     }
//   }
// }

// tc_Field *tico_lua_new_field_vecn(lua_State *L, int n, int index) {
//   if (n < 2 || n > 4) return NULL;

//   tc_Field *field = tico_field_create(n == 2 ? TIC_FIELD_VEC2 : n == 3 ? TIC_FIELD_VEC3 : TIC_FIELD_VEC4, "");

//   lua_pushnil(L);
//   int i = 0;
//   while (lua_next(L, index)) {
//     float num = lua_tonumber(L, -1);
//     if (n == 2) field->vec2.data[i] = num;
//     else if (n == 3) field->vec3.data[i] = num;
//     else field->vec4.data[i] = num;
//     lua_pop(L, 1);
//   }

//   return field;
// }

// tc_Field* tico_lua_new_field_array(lua_State *L, int index) {
//   // cJSON *json = tico_json_create_array(NULL);
//   float len = lua_rawlen(L, index);
//   tc_Field *field = tico_lua_new_field_vecn(L, len, index);
//   if (field) return field;
//   // tc_Field *field = 
//   field = tico_field_create(TIC_FIELD_ARRAY, "");
//   lua_pushnil(L);
//   while(lua_next(L, index)) {
//     tc_Field *item = NULL;
//     switch(lua_type(L, -1)) {
//       case LUA_TNUMBER:
//         {
//           float n = lua_tonumber(L, -1);
//           // item = cJSON_CreateNumber(n);
//           item = tico_field_create_number("", n);
//           break;
//         }
//       case LUA_TBOOLEAN:
//         {
//           int val = lua_toboolean(L, -1);
//           // item = cJSON_CreateBool(val);
//           item = tico_field_create_boolean("", val);
//           break;
//         }
//       case LUA_TSTRING:
//         {
//           const char *str = lua_tostring(L, -1);
//           // item = cJSON_CreateString(str);
//           item = tico_field_create_string("", str);
//           break;
//         }
//       case LUA_TTABLE:
//         {
//           if (lua_isarray(L, -1)) {
//             int top = lua_gettop(L);
//             item = tico_lua_new_field_array(L, top);
//           } else {
//             int top = lua_gettop(L);
//             item = tico_lua_new_field_object(L, top);
//           }
//           break;
//         }
//     }
//     if (item) tico_field_add(field, item);
//     lua_pop(L, 1);
//   }
//   // lua_pop(L, 1);

//   return json;
// }

// tc_Field* tico_lua_new_field_object(lua_State *L, int index) {
//   float len = lua_rawlen(L, index);
//   tc_Field *field = tico_lua_new_field_vecn(L, len, index);
//   if (field) return field;
//   // tc_Field *field = 
//   field = tico_field_create(TIC_FIELD_ARRAY, "");
//   lua_pushnil(L);
//   while(lua_next(L, index)) {
//     tc_Field *item = NULL;
//     switch(lua_type(L, -1)) {
//       case LUA_TNUMBER:
//         {
//           float n = lua_tonumber(L, -1);
//           // item = cJSON_CreateNumber(n);
//           item = tico_field_create_number("", n);
//           break;
//         }
//       case LUA_TBOOLEAN:
//         {
//           int val = lua_toboolean(L, -1);
//           // item = cJSON_CreateBool(val);
//           item = tico_field_create_boolean("", val);
//           break;
//         }
//       case LUA_TSTRING:
//         {
//           const char *str = lua_tostring(L, -1);
//           // item = cJSON_CreateString(str);
//           item = tico_field_create_string("", str);
//           break;
//         }
//       case LUA_TTABLE:
//         {
//           if (lua_isarray(L, -1)) {
//             int top = lua_gettop(L);
//             item = tico_lua_new_field_array(L, top);
//           } else {
//             int top = lua_gettop(L);
//             item = tico_lua_new_field_object(L, top);
//           }
//           break;
//         }
//     }
//     if (item) tico_field_add(field, item);
//     lua_pop(L, 1);
//   }
//   // lua_pop(L, 1);

//   return json;
// }

// static int tico_lua_json_new_object(lua_State *L, cJSON *json);

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
  cJSON *json = tico_json_create_array(NULL);
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
              tico_json_add_string(robj, "type", RECTANGLE_CLASS);
              tico_json_add_number(robj, "x", rect->x);
              tico_json_add_number(robj, "y", rect->y);
              tico_json_add_number(robj, "w", rect->w);
              tico_json_add_number(robj, "h", rect->h);
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
          tico_json_add_number(json, key, n);
          break;
        }
        case LUA_TBOOLEAN:
        {
          int val = lua_toboolean(L, -1);
          tico_json_add_boolean(json, key, val);
          break;
        }
        case LUA_TSTRING:
        {
          char *str = (char*)lua_tostring(L, -1);
          tico_json_add_string(json, key, str);
          break;
        }
        case LUA_TTABLE:
        {
          cJSON *item = NULL;
          if (strstr(key, "__")) break;
          if (lua_isarray(L, -1)) {
            int top = lua_gettop(L);
            item = tico_lua_json_new_cjson_array(L, top);
            tico_json_add_array(json, key, item);
          } else {
            int top = lua_gettop(L);
            item = tico_lua_json_new_cjson_object(L, top);
            tico_json_add_object(json, key, item);
          }
          break;
        }
        case LUA_TUSERDATA:
        {
          if (luaL_testudata(L, -1, RECTANGLE_CLASS)) {
            tc_Rectf *rect = lua_touserdata(L, -1);
            cJSON *robj = tico_json_create();
            tico_json_add_string(robj, "type", RECTANGLE_CLASS);
            tico_json_add_number(robj, "x", rect->x);
            tico_json_add_number(robj, "y", rect->y);
            tico_json_add_number(robj, "w", rect->w);
            tico_json_add_number(robj, "h", rect->h);
            tico_json_add_object(json, key, robj);
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
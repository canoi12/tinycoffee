#include "tico.h"


int tico_json_is_valid(const char *filename) {
  const char *content = (const char*)tico_filesystem_read_file(filename, NULL);
  cJSON *parsed = cJSON_Parse(content);
  free((char*)content);
  if (!parsed) return 0;

  return 1;
}

cJSON *tico_json_open(const char *filename) {
  unsigned char *content = tico_filesystem_read_file(filename, NULL);
  cJSON *parsed = cJSON_Parse((const char*)content);
  free(content);
  if (!parsed) {
    const char *err = cJSON_GetErrorPtr();
    if (err != NULL) {
      TRACEERR("Failed to load json '%s': error before %s", filename, err);
    }
  }
  return parsed;
}
cJSON *tico_json_parse(const char *jsonStr) {
  cJSON *parsed = cJSON_Parse(jsonStr);
  if (!parsed) {
    const char *err = cJSON_GetErrorPtr();
    if (err != NULL) {
      TRACEERR("Failed to parse json: error before %s", err);
    }
  }
  return parsed;
}

cJSON* tico_json_clone(cJSON *src) {
  ASSERT(src != NULL);
  const char *json_str = tico_json_print(src);
  if (!json_str) return NULL;

  cJSON* json_clone = tico_json_parse(json_str);
  free(json_str);

  return json_clone;
}

void tico_json_save(const char *filename, cJSON* const json) {
  char *string = cJSON_Print(json);
  size_t size = strlen(string);
  if (!string) {
    TRACEERR("Failed to save json: '%s'", filename);
    return;
  }
  tico_filesystem_write_file(filename, string, size, 0);
}

char *tico_json_print(cJSON* const json) {
  return cJSON_Print(json);
}

cJSON* tico_json_create() {
  return cJSON_CreateObject();
}

// cJSON* tico_json_create_array(const cJSON*) {
//   return cJSON_CreateArray();
// }

void tico_json_delete(cJSON* json) {
  cJSON_Delete(json);
}

int tico_json_add_item(cJSON *const json, const char *name, cJSON *const item) {
  ASSERT(json != NULL);
  // cJSON *string = cJSON_CreateString(value);
  if (tico_json_is_object(json)) {
    ASSERT(name != NULL);
    ASSERT(item != NULL);
    cJSON_AddItemToObject(json, name, item);
    return 1;
  }
  if (tico_json_is_array(json)) {
    ASSERT(item != NULL);
    cJSON_AddItemToArray(json, item);
    return 1;
  }

  return 0;
}

cJSON* tico_json_get_item(cJSON *const json, const char *name, int index) {
  ASSERT(json != NULL);

  if (tico_json_is_object(json)) {
    ASSERT(name != NULL);
    cJSON *item = cJSON_GetObjectItem(json, name);
    return item;
  }

  if (tico_json_is_array(json)) {
    ASSERT(index >= 0);
    cJSON *item = cJSON_GetArrayItem(json, index);
    return item;
  }

  return NULL;
}

tc_Circlef tico_json_to_circle(cJSON *const json) {
  ASSERT(json != NULL);
  tc_Circlef circle = {0};

  float radius = tico_json_get_opt_number(json, "radius", 0, 8);
  tc_Vec2 position = tico_json_get_vec2(json, "position", 0);

  circle.radius = radius;
  circle.x = position.x;
  circle.y = position.y;

  return circle;
}

/***********
 * String
 ***********/

cJSON *tico_json_create_string(const char *string) {
  ASSERT(string != NULL);
  return cJSON_CreateString(string);
}

int tico_json_is_string(cJSON* const json) {
  // cJSON *jsonName = cJSON_GetObjectItem(json, name);
  ASSERT(json != NULL);
  return cJSON_IsString(json);
}

void tico_json_set_string(cJSON* const json, const char* value) {
  if (!tico_json_is_string(json)) return;

  cJSON_SetValuestring(json, value);
}

const char* tico_json_to_string(cJSON* const json) {
  return tico_json_opt_string(json, NULL);
}

const char* tico_json_opt_string(cJSON* const json, const char* opt) {
  if (!tico_json_is_string(json)) return opt;
  return json->valuestring;
}

cJSON *tico_json_add_string(cJSON* const json, const char *name, const char* value) {
  cJSON *string = cJSON_CreateString(value);
  if (tico_json_add_item(json, name, string)) return string;

  return NULL;
}

const char *tico_json_get_string(cJSON* const json, const char *name, int index) {
  return tico_json_get_opt_string(json, name, index, NULL);
}

const char *tico_json_get_opt_string(cJSON* const json, const char *name, int index, const char *opt_string) {
  // if (!tico_json_is_object(json)) return opt_string;
  // cJSON *jsonName = cJSON_GetObjectItem(json, name);
  // return tico_json_opt_string(jsonName, opt_string);
  cJSON *item = tico_json_get_item(json, name, index);
  if (!item) return opt_string;
  return tico_json_opt_string(item, opt_string);
}


/**********
 * Number
 **********/

cJSON* tico_json_create_number(float number) {
  return cJSON_CreateNumber(number);
}

int tico_json_is_number(cJSON* const json) {
  // cJSON *value = cJSON_GetObjectItem(json, name);
  // if (value) return cJSON_IsNumber(value);
  // return tc_false;
  ASSERT(json != NULL);
  return cJSON_IsNumber(json);
}

void tico_json_set_number(cJSON* const json, float value) {
  if (!tico_json_is_number(json)) return;
  cJSON_SetNumberValue(json, value);
}

float tico_json_to_number(cJSON *const json) {
  return tico_json_opt_number(json, -1);
}

float tico_json_opt_number(cJSON *const json, float opt) {
  if (!tico_json_is_number(json)) return opt;

  return json->valuedouble;
}

cJSON *tico_json_add_number(cJSON *const json, const char *name, float number) {
  cJSON *value = cJSON_CreateNumber(number);
  if (tico_json_add_item(json, name, value)) return value;

  return NULL;
}

float tico_json_get_number(cJSON* const json, const char *name, int index) {
  return tico_json_get_opt_number(json, name, index, -1);
}
float tico_json_get_opt_number(cJSON* const json, const char *name, int index, float optVal) {
  cJSON *item = tico_json_get_item(json, name, index);
  if (!item) return optVal;
  return tico_json_opt_number(item, optVal);
}

/************
 * Vectors
 ************/

static cJSON *tico_json_create_vecn(float *data, int n) {
  ASSERT(data != NULL);
  ASSERT(n > 0);
  cJSON *array = tico_json_create_array(NULL);
  for (int i = 0; i < n; i++) {
    cJSON *item = tico_json_create_number(data[i]);
    cJSON_AddItemToArray(array, item);
  }

  return array;
}

static int tico_json_is_vecn(cJSON *const json, int n) {
  ASSERT(json != NULL);
  ASSERT(n > 0);
  if (!tico_json_is_array(json)) return 0;
  if (cJSON_GetArraySize(json) < n) return 0;

  return 1;
}

static void tico_json_set_vecn(cJSON *const json, float *data, int n) {
  ASSERT(json != NULL);
  if (!tico_json_is_vecn(json, n)) return;
  // ASSERT(json != NULL);
  ASSERT(data != NULL);

  for (int i = 0; i < n; i++) {
    // cJSON *item = tico_json_get_item(json, NULL, i);
    cJSON *item = cJSON_GetArrayItem(json, i);
    if (!item) return;
    // TRACELOG("%d: %d", i, data[i]);
    // item->valuedouble = data[i];
    tico_json_set_number(item, data[i]);
  }
}

static float* tico_json_opt_vecn(cJSON *const json, int n, float *opt) {

}

// TRACELOG("%d: %f %f", i, vec.data[i], item->valuedouble);

#define JSON_VECN(n) \
cJSON *tico_json_create_vec##n (tc_Vec##n vec) { \
  return tico_json_create_vecn(vec.data, n); \
} \
int tico_json_is_vec##n (cJSON *const json) { \
  return tico_json_is_vecn(json, n); \
} \
void tico_json_set_vec##n (cJSON *const json, tc_Vec##n vec) { \
  ASSERT(json != NULL); \
  if (!tico_json_is_vec##n(json)) return; \
  for (int i = 0; i < n; i++) { \
    cJSON *item = tico_json_get_item(json, NULL, i); \
    if (!item) return; \
    tico_json_set_number(item, vec.data[i]); \
  } \
} \
tc_Vec##n tico_json_to_vec##n (cJSON *const json) { \
  tc_Vec##n opt = {-1}; \
  return tico_json_opt_vec##n (json, opt); \
} \
tc_Vec##n tico_json_opt_vec##n (cJSON *const json, tc_Vec##n opt) { \
  if (!tico_json_is_vecn(json, n)) return opt; \
  tc_Vec##n out = {0}; \
  for (int i = 0; i < n; i++) { \
    out.data[i] = tico_json_get_number(json, NULL, i); \
  } \
  return out; \
} \
cJSON *tico_json_add_vec##n (cJSON *const json, const char *name, tc_Vec##n vec) { \
  cJSON *value = tico_json_get_item(json, name, 0); \
  if (value) { \
    tico_json_set_vec##n(value, vec); \
    return value; \
  } \
  value = tico_json_create_vec##n(vec); \
  if (tico_json_add_item(json, name, value)) return value; \
  tico_json_delete(value); \
  return NULL; \
} \
tc_Vec##n tico_json_get_vec##n (cJSON *const json, const char *name, int index) { \
  tc_Vec##n opt; \
  memset(&opt, -1, sizeof(tc_Vec##n)); \
  return tico_json_get_opt_vec##n (json, name, index, opt); \
} \
tc_Vec##n tico_json_get_opt_vec##n (cJSON *const json, const char *name, int index, tc_Vec##n opt) { \
  cJSON *item = tico_json_get_item(json, name, index); \
  if (!item) return opt; \
  return tico_json_opt_vec##n(item, opt); \
}

JSON_VECN(2);
JSON_VECN(3);
JSON_VECN(4);

// cJSON* tico_json_create_vec2(tc_Vec2 vec) {
//   // cJSON *array = tico_json_create_array(NULL);
//   // cJSON *x = tico_json_create_number(vec.x);
//   // cJSON *y = tico_json_create_number(vec.y);

//   // cJSON_AddItemToArray(array, x);
//   // cJSON_AddItemToArray(array, y);

//   // return array;
//   return tico_json_create_vecn(vec.data, 2);
// }

// int tico_json_is_vec2(cJSON * const json) {
//   // cJSON *array = tico_json_get_array(json, name);
//   // if (!array) return 0;
//   if (!tico_json_is_array(json)) return 0;
//   if (cJSON_GetArraySize(json) < 2) return 0;

//   return 1;
// }

// void tico_json_set_vec2(cJSON *const json, tc_Vec2 vec) {
//   // if (!tico_json_is_vec2(json)) return;
//   // cJSON *x = cJSON_GetArrayItem(json, 0);
//   // cJSON *y = cJSON_GetArrayItem(json, 1);
//   // x->valuedouble = vec.x;
//   // y->valuedouble = vec.y;
//   tico_json_set_vecn(json, vec.data, 2);
// }

// tc_Vec2 tico_json_to_vec2(cJSON* const json) {
//   return tico_json_opt_vec2(json, tico_vec2(-1, -1));
// }

// tc_Vec2 tico_json_opt_vec2(cJSON *const json, tc_Vec2 opt) {
//   if (!tico_json_is_vec2(json)) return opt;

//   cJSON *x = cJSON_GetArrayItem(json, 0);
//   cJSON *y = cJSON_GetArrayItem(json, 1);

//   tc_Vec2 vec = tico_vec2(x->valuedouble, y->valuedouble);

//   return vec;
// }

// cJSON* tico_json_add_vec2(cJSON *const json, const char *name, tc_Vec2 vec) {
//   cJSON *value = tico_json_get_item(json, name, 0);
//   if (value) {
//     tico_json_set_vec2(value, vec);
//     return value;
//   }
//   value = tico_json_create_vec2(vec);
//   if (tico_json_add_item(json, name, value)) return value;

//   tico_json_delete(value);
//   return NULL;
// }

// tc_Vec2 tico_json_get_vec2(cJSON * const json, const char *name, int index) {
//   return tico_json_get_opt_vec2(json, name, index, tico_vec2(-1, -1));
// }

// tc_Vec2 tico_json_get_opt_vec2(cJSON * const json, const char *name, int index, tc_Vec2 vec) {
//   cJSON *item = tico_json_get_item(json, name, index);
//   if (!item) return vec;

//   return tico_json_opt_vec2(item, vec);
// }

/***********
 * Boolean
 ***********/

cJSON* tico_json_create_boolean(int boolean) {
  return cJSON_CreateBool(boolean);
}

int tico_json_is_boolean(cJSON* const json) {
  // cJSON *val = cJSON_GetObjectItem(json, name);
  ASSERT(json != NULL);
  return cJSON_IsBool(json);
}

void tico_json_set_boolean(cJSON *const json, int boolean) {
  if (!tico_json_is_boolean(json)) return;

  json->valueint = boolean;
}

int tico_json_to_boolean(cJSON* const json) {
  return tico_json_opt_boolean(json, -1);
}

int tico_json_opt_boolean(cJSON *const json, int opt) {
  if (!tico_json_is_boolean(json)) return opt;

  return json->valueint;
}


cJSON *tico_json_add_boolean(cJSON *const json, const char *name, int boolean) {
  cJSON *item = tico_json_create_boolean(boolean);
  if (tico_json_add_item(json, name, item)) return item;

  tico_json_delete(item);
  return NULL;
}

int tico_json_get_boolean(cJSON* const json, const char *name, int index) {
  return tico_json_get_opt_boolean(json, name, index, -1);
}
int tico_json_get_opt_boolean(cJSON* const json, const char *name, int index, int opt_boolean) {
//   cJSON *val = cJSON_GetObjectItem(json, name);
//   if (cJSON_IsBool(val)) {
//     return val->valueint;
//   }
//   return optVal;
// }
// cJSON* tico_json_set_boolean(cJSON* const json, char *name, tc_bool value) {
//   cJSON *boolVal = cJSON_AddBoolToObject(json, name, value);
//   if (boolVal == NULL) TRACEERR("Failed to add boolean '%s' to json ['%s']", value == 0 ? "false" : "true", name);
//   return boolVal;
  // cJSON *boolean = cJSON_GetIte
  cJSON *item = tico_json_get_item(json, name, index);
  if (!item) return opt_boolean;

  return tico_json_opt_boolean(item, opt_boolean);
}

/***********
 * Array
 ***********/

cJSON* tico_json_create_array(const cJSON *json) {
  cJSON *item = cJSON_CreateArray();
  return item;
}

int tico_json_is_array(cJSON* const json) {
  // cJSON *array = cJSON_GetObjectItem(json, name);
  // if (array) return cJSON_IsArray(array);
  // return tc_false;
  ASSERT(json != NULL);
  return cJSON_IsArray(json);
}

void tico_json_set_array(cJSON* const json, const cJSON* jsonArray) {
  // cJSON *val = cJSON_GetObjectItem(json, name);
  // if (val) {
  //   // TRACELOG("testando");
  //   tico_json_delete(val);
  // }
  // if (jsonArray) {
  //   cJSON_AddItemToObject(json, name, jsonArray);
  // } else {
  //   return cJSON_AddArrayToObject(json, name);
  // }
}

const cJSON* tico_json_to_array(cJSON *const json) {
  return tico_json_opt_array(json, NULL);
}

const cJSON* tico_json_opt_array(cJSON *const json, const cJSON* opt) {
  if (!tico_json_is_array(json)) return opt;

  return json;
}

cJSON* tico_json_add_array(cJSON *const json, const char *name, const cJSON *array) {
  if (!array) {
    cJSON *item = tico_json_create_array(NULL);
    tico_json_add_item(json, name, item);
    return item;
  }

  if (!tico_json_is_array((cJSON*)array)) return NULL;
  // tico_json_add_array(json, name, array);
  tico_json_add_item(json, name, array);
  return (cJSON*)array;
}

const cJSON* tico_json_get_array(cJSON* const json, const char *name, int index) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (cJSON_IsArray(array)) {
    return array;
  }
  return NULL;
}

const cJSON* tico_json_get_opt_array(cJSON *const json, const char *name, int index, const cJSON *opt) {
  cJSON *item = tico_json_get_item(json, name, index);
  if (!item) return opt;

  return tico_json_opt_array(item, opt);
}



int tico_json_get_array_size(const cJSON *json) {
  return cJSON_GetArraySize(json);
}

/*************
 * Object
 *************/

cJSON *tico_json_create_object(const cJSON* json) {
  cJSON *item = cJSON_CreateObject();
  return item;
}

int tico_json_is_object(cJSON* const json) {
  // cJSON *obj = cJSON_GetObjectItem(json, name);
  // if (obj) return cJSON_IsObject(obj);
  // return tc_false;
  ASSERT(json != NULL);
  return cJSON_IsObject(json);
}

void tico_json_set_object(cJSON *const json, const cJSON *value) {

}

const cJSON *tico_json_to_object(cJSON *const json) {
  return tico_json_opt_object(json, NULL);
}

const cJSON *tico_json_opt_object(cJSON *const json, const cJSON *opt) {
  if (!tico_json_is_object(json)) return opt;

  return json;
}

cJSON *tico_json_add_object(cJSON *const json, const char *name, const cJSON *value) {
  if (!value) {
    cJSON *item = tico_json_create_object(NULL);
    tico_json_add_item(json, name, item);
    return item;
  }

  if (!tico_json_is_object((cJSON*)value)) return NULL;
  tico_json_add_item(json, name, value);
  return (cJSON*)value;
}

const cJSON* tico_json_get_object(cJSON* const json, const char *name, int index) {
  // cJSON *obj = cJSON_GetObjectItem(json, name);
  // if (cJSON_IsObject(obj)) return obj;
  // return NULL;
  return tico_json_get_opt_object(json, name, index, NULL);
}

const cJSON* tico_json_get_opt_object(cJSON *const json, const char *name, int index, const cJSON *opt) {
  cJSON *item = tico_json_get_item(json, name, index);
  if (!item) return opt;

  return tico_json_opt_object(item, opt);
}

// cJSON* tico_json_set_object(cJSON* const json, const char *name, cJSON* const jsonObj) {
//     // if (jsonObj) {
//     //   cJSON_AddItemToObject(json, name, jsonObj);
//     //   return jsonObj;
//     // } else {
//     //   return cJSON_AddObjectToObject(json, name);
//     // }

// }
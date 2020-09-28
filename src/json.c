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

cJSON* tico_json_create_array() {
  return cJSON_CreateArray();
}

void tico_json_delete(cJSON* json) {
  cJSON_Delete(json);
}

/***********
 * String
 ***********/

tc_bool tico_json_is_string(cJSON* const json, char *name) {
  cJSON *jsonName = cJSON_GetObjectItem(json, name);
  if (jsonName) {
    return cJSON_IsString(jsonName);
  }
  return tc_false;
}

char *tico_json_get_string(cJSON* const json, char *name) {
  return tico_json_get_opt_string(json, name, NULL);
}

char *tico_json_get_opt_string(cJSON* const json, char *name, char *optVal) {
  cJSON *jsonName = cJSON_GetObjectItem(json, name);
  if (cJSON_IsString(jsonName) && (jsonName->valuestring != NULL)) {
    return jsonName->valuestring;
  }
  return optVal;
}
cJSON *tico_json_set_string(cJSON* const json, char *name, char *value) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    cJSON_SetValuestring(val, value);
    return val;
  }
  cJSON *strVal = cJSON_AddStringToObject(json, name, value);
  if (strVal == NULL) TRACEERR("failed to add string '%s' to json ['%s']", value, name);
  return strVal;
}

/**********
 * Number
 **********/

tc_bool tico_json_is_number(cJSON* const json, char *name) {
  cJSON *value = cJSON_GetObjectItem(json, name);
  if (value) return cJSON_IsNumber(value);
  return tc_false;
}

float tico_json_get_number(cJSON* const json, char *name) {
  return tico_json_get_opt_number(json, name, -1);
}
float tico_json_get_opt_number(cJSON* const json, char *name, float optVal) {
  cJSON *value = cJSON_GetObjectItem(json, name);
  if (cJSON_IsNumber(value)) {
    return value->valuedouble;
  }
  return optVal;
}
cJSON *tico_json_set_number(cJSON* const json, char *name, float value) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    val->valuedouble = value;
    return val;
  }
  cJSON *nVal = cJSON_AddNumberToObject(json, name, value);
  if (nVal == NULL) TRACEERR("Failed to add number '%f' to json ['%s']", value, name);
  return nVal;
}

/***********
 * Boolean
 ***********/

tc_bool tico_json_is_boolean(cJSON* const json, char *name) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) return cJSON_IsBool(val);
  return tc_false;
}

int tico_json_get_boolean(cJSON* const json, char *name) {
  return tico_json_get_opt_boolean(json, name, tc_false);
}
int tico_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (cJSON_IsBool(val)) {
    return val->valueint;
  }
  return optVal;
}
cJSON* tico_json_set_boolean(cJSON* const json, char *name, tc_bool value) {
  cJSON *boolVal = cJSON_AddBoolToObject(json, name, value);
  if (boolVal == NULL) TRACEERR("Failed to add boolean '%s' to json ['%s']", value == 0 ? "false" : "true", name);
  return boolVal;
}

/***********
 * Array
 ***********/

tc_bool tico_json_is_array(cJSON* const json, char *name) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (array) return cJSON_IsArray(array);
  return tc_false;
}

cJSON* tico_json_get_array(cJSON* const json, char *name) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (cJSON_IsArray(array)) {
    return array;
  }
  return NULL;
}

cJSON* tico_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    // TRACELOG("testando");
    tico_json_delete(val);
  }
  if (jsonArray) {
    cJSON_AddItemToObject(json, name, jsonArray);
  } else {
    return cJSON_AddArrayToObject(json, name);
  }
}

int tico_json_get_array_size(const cJSON *json) {
  return cJSON_GetArraySize(json);
}

/*************
 * Object
 *************/

tc_bool tico_json_is_object(cJSON* const json, char *name) {
  cJSON *obj = cJSON_GetObjectItem(json, name);
  if (obj) return cJSON_IsObject(obj);
  return tc_false;
}

cJSON* tico_json_get_object(cJSON* const json, char *name) {
  cJSON *obj = cJSON_GetObjectItem(json, name);
  if (cJSON_IsObject(obj)) return obj;
  return NULL;
}
cJSON* tico_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj) {
    if (jsonObj) {
      cJSON_AddItemToObject(json, name, jsonObj);
      return jsonObj;
    } else {
      return cJSON_AddObjectToObject(json, name);
    }
}
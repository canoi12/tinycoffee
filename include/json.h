#ifndef TICO_JSON_H
#define TICO_JSON_H

#include "definitions.h"

#include "cjson/cJSON.h"

/*********************
 * JSON
 *********************/

TIC_API int tico_json_is_valid(const char *filename);
TIC_API cJSON* tico_json_open(const char *filename);
TIC_API cJSON* tico_json_parse(const char *jsonStr);
TIC_API void tico_json_save(const char *filename, cJSON* const json);
TIC_API char* tico_json_print(cJSON* const json);

TIC_API cJSON* tico_json_create();
TIC_API cJSON* tico_json_create_array();
TIC_API void tico_json_delete(cJSON* const json);

TIC_API tc_bool tico_json_is_string(cJSON* const json, char *name);
TIC_API char* tico_json_get_string(cJSON* const json, char *name);
TIC_API char* tico_json_get_opt_string(cJSON* const json, char *name, char *optVal);
TIC_API cJSON* tico_json_set_string(cJSON* const json, char *name, char *value);

TIC_API tc_bool tico_json_is_number(cJSON* const json, char *name);
TIC_API float tico_json_get_number(cJSON* const json, char *name);
TIC_API float tico_json_get_opt_number(cJSON* const json, char *name, float optVal);
TIC_API cJSON* tico_json_set_number(cJSON* const json, char *name, float value);

TIC_API tc_bool tico_json_is_boolean(cJSON* const json, char *name);
TIC_API int tico_json_get_boolean(cJSON* const json, char *name);
TIC_API int tico_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal);
TIC_API cJSON* tico_json_set_boolean(cJSON* const json, char *name, tc_bool value);

TIC_API tc_bool tico_json_is_array(cJSON* const json, char *name);
TIC_API cJSON* tico_json_get_array(cJSON* const json, char *name);
TIC_API cJSON* tico_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray);
TIC_API int tico_json_get_array_size(const cJSON* json);

TIC_API tc_bool tico_json_is_object(cJSON* const json, char *name);
TIC_API cJSON* tico_json_get_object(cJSON* const json, char *name);
TIC_API cJSON* tico_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj);

#endif // TICO_JSON_H
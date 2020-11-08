#ifndef TICO_JSON_H
#define TICO_JSON_H

#include "definitions.h"
// #include "tico_math.h"

#include "cjson/cJSON.h"

#define JSON_TYPE(name_t, type_t) \
TIC_API cJSON* tico_json_create_##name_t (type_t value); \
TIC_API int tico_json_is_##name_t (cJSON *const json); \
TIC_API void tico_json_set_##name_t (cJSON *const json, type_t value); \
TIC_API type_t tico_json_to_##name_t (cJSON *const json); \
TIC_API type_t tico_json_opt_##name_t (cJSON *const json, type_t opt); \
TIC_API cJSON* tico_json_add_##name_t (cJSON *const json, const char *name, type_t value); \
TIC_API type_t tico_json_get_##name_t (cJSON *const json, const char *name, int index); \
TIC_API type_t tico_json_get_opt_##name_t (cJSON *const json, const char *name, int index, type_t opt) 
// TIC_API cJSON* tico_json_get_##name_t##_item (cJSON *const json, const char *name);
// TIC_API int tico_json_get_##name_t (cJSON *const json, type_t type); \
// TIC_API int tico_json_get_opt_##name_t (cJSON *const json, type_t type); \
// TIC_API int tico_json_is_##name_t (cJSON *const json, type_t type); \
// TIC_API int tico_json_is_##name_t (cJSON *const json, type_t type); \

/*********************
 * JSON
 *********************/

TIC_API int tico_json_is_valid(const char *filename);
TIC_API cJSON* tico_json_open(const char *filename);
TIC_API cJSON* tico_json_parse(const char *jsonStr);
TIC_API cJSON* tico_json_clone(cJSON *src);
TIC_API void tico_json_save(const char *filename, cJSON* const json);
TIC_API char* tico_json_print(cJSON* const json);

TIC_API cJSON* tico_json_create();
// TIC_API cJSON* tico_json_create_array();
TIC_API void tico_json_delete(cJSON* const json);

TIC_API int tico_json_add_item(cJSON *const json, const char *name, cJSON *const item);
TIC_API cJSON *tico_json_get_item(cJSON *const json, const char *name, int index);

// TIC_API tc_bool tico_json_is_string(cJSON* const json, char *name);
// TIC_API char* tico_json_to_string(cJSON* const json);
// TIC_API char* tico_json_get_string(cJSON* const json, char *name);
// TIC_API char* tico_json_get_opt_string(cJSON* const json, char *name, char *optVal);
// TIC_API cJSON* tico_json_add_string(cJSON* const json, char *name, char *value);
// TIC_API cJSON* tico_json_set_string(cJSON* const json, char *value);
JSON_TYPE(string, const char*);
JSON_TYPE(number, float);
JSON_TYPE(boolean, int);
JSON_TYPE(vec2, tc_Vec2);
JSON_TYPE(vec3, tc_Vec3);
JSON_TYPE(vec4, tc_Vec4);
JSON_TYPE(array, const cJSON*);
JSON_TYPE(int_array, int*);
JSON_TYPE(object, const cJSON*);

TIC_API tc_Circlef tico_json_to_circle(cJSON *const json);

// TIC_API tc_bool tico_json_is_number(cJSON* const json, char *name);
// TIC_API float tico_json_to_number(cJSON* const json);
// TIC_API float tico_json_get_number(cJSON* const json, char *name);
// TIC_API float tico_json_get_opt_number(cJSON* const json, char *name, float optVal);
// TIC_API cJSON* tico_json_add_number(cJSON* const json, char *name, float value);
// TIC_API cJSON* tico_json_set_number(cJSON* const json, float value);

// TIC_API tc_bool tico_json_is_vec2(cJSON * const json);
// TIC_API tc_Vec2 tico_json_to_vec2(cJSON* const json);
// TIC_API tc_Vec2 tico_json_get_vec2(cJSON * const json, char *name);
// TIC_API tc_Vec2 tico_json_get_opt_vec2(cJSON * const json, char *name, tc_Vec2 vec);
// TIC_API cJSON* tico_json_add_vec2(cJSON * const json, char *name, tc_Vec2 vec);
// TIC_API cJSON* tico_json_set_vec2(cJSON * const json, tc_Vec2 vec);

// TIC_API tc_bool tico_json_is_vec3(cJSON * const json, char *name);
// TIC_API tc_Vec3 tico_json_to_vec3(cJSON* const json);
// TIC_API tc_Vec3 tico_json_get_vec3(cJSON * const json, char *name);
// TIC_API tc_Vec3 tico_json_get_opt_vec3(cJSON * const json, char *name, tc_Vec3 vec);
// TIC_API cJSON* tico_json_add_vec3(cJSON * const json, char *name, tc_Vec3 vec);
// TIC_API cJSON* tico_json_set_vec3(cJSON * const json, tc_Vec3 vec);

// TIC_API tc_bool tico_json_is_boolean(cJSON* const json, char *name);
// TIC_API int tico_json_to_boolean(cJSON* const json);
// TIC_API int tico_json_get_boolean(cJSON* const json, char *name);
// TIC_API int tico_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal);
// TIC_API cJSON* tico_json_add_boolean(cJSON* const json, char *name, tc_bool value);
// TIC_API cJSON* tico_json_set_boolean(cJSON* const json, tc_bool value);

// TIC_API tc_bool tico_json_is_array(cJSON* const json, char *name);
// TIC_API cJSON* tico_json_get_array(cJSON* const json, char *name);
// TIC_API cJSON* tico_json_add_array(cJSON* const json, char *name, cJSON* const jsonArray);
// TIC_API cJSON* tico_json_set_array(cJSON* const json, cJSON* const jsonArray);
TIC_API int tico_json_get_array_size(const cJSON* json);

// TIC_API tc_bool tico_json_is_object(cJSON* const json, char *name);
// TIC_API cJSON* tico_json_get_object(cJSON* const json, char *name);
// TIC_API cJSON* tico_json_add_object(cJSON* const json, char *name, cJSON* const jsonObj);
// TIC_API cJSON* tico_json_set_object(cJSON* const json, cJSON* const jsonObj);

#endif // TICO_JSON_H
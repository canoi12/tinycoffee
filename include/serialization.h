#ifndef TICO_SERIALIZATION_H
#define TICO_SERIALIZATION_H

#include "tico.h"
#define tico_field_foreach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#define FIELD_FUNCTION(func_name, ...) \
func_name (tc_Field *field, __VA_ARGS__)

#define FIELD_TYPE(name_p, type_t) \
TIC_API tc_Field* tico_field_create_##name_p(type_t value); \
TIC_API tc_Field* tico_field_##name_p##_from_json(cJSON *json); \
TIC_API type_t tico_field_get_##name_p(tc_Field *field, const char *name); \
TIC_API type_t tico_field_get_opt_##name_p(tc_Field *field, const char *name, type_t opt); \
TIC_API tc_Field* tico_field_add_##name_p(tc_Field *field, const char *name, type_t value); \
TIC_API type_t tico_field_to_##name_p(tc_Field *field); \
TIC_API type_t tico_field_to_opt_##name_p(tc_Field *field, type_t value); \
TIC_API void tico_field_set_##name_p(tc_Field *field, type_t value); \
TIC_API void tico_field_##name_p##_imgui(tc_Field *field); \
TIC_API void tico_field_draw_##name_p(tc_Field *field, tc_Vec2 pos, float scale); \
TIC_API cJSON* tico_field_##name_p##_to_json(tc_Field *field, int export_meta); \
TIC_API void tico_meta_draw_##name_p(tc_Field*, tc_Field*); \
TIC_API void tico_meta_imgui_##name_p(tc_Field*, tc_Field*);

typedef struct tc_Field tc_Field;
typedef struct tc_Meta tc_Meta;

typedef enum {
  TIC_FIELD_TYPE_NULL = 0,
  TIC_FIELD_TYPE_NUMBER,
  TIC_FIELD_TYPE_STRING,
  TIC_FIELD_TYPE_BOOLEAN,
  TIC_FIELD_TYPE_VEC2,
  TIC_FIELD_TYPE_VEC3,
  TIC_FIELD_TYPE_VEC4,
  // TIC_FIELD_TYPE_CIRCLE,
  // TIC_FIELD_TYPE_POINTS,
  // TIC_FIELD_TYPE_OPTIONS,
  // TIC_FIELD_TYPE_MULTIOPTIONS,
  // TIC_FIELD_TYPE_RESOURCE,
  TIC_FIELD_TYPE_ARRAY,
  TIC_FIELD_TYPE_OBJECT,
  TIC_FIELD_TYPE_CUSTOM,
  TIC_FIELD_TYPE_REFERENCE,
  TIC_FIELD_TYPE_MAX
} TIC_FIELD_TYPE_;

typedef tc_Field*(*loadFromJson)(cJSON*);
typedef tc_Field*(*exportToJson)(cJSON*);
typedef void(*drawField)(tc_Field*, tc_Vec2, float);
typedef void(*imguiField)(tc_Field*);

typedef struct tc_MetaFn {
  // FromJSON from_json;
  drawField draw;
  imguiField imgui;
} tc_FieldFn;

typedef struct tc_FieldType {
    loadFromJson load;
    exportToJson json;
    drawField draw;
    imguiField imgui;
} tc_FieldType;

typedef map_t(tc_FieldType) map_fieldtype_t;

// typedef map_t(tc_MetaFn) map_metafn_t;

typedef struct tc_Serialization {
  // tc_MetaFn meta_types[TIC_FIELD_TYPE_MAX];
  // tc_FieldFn field_types[TIC_FIELD_TYPE_MAX];
  map_fieldtype_t types;
} tc_Serialization;

struct tc_Field {
  TIC_FIELD_TYPE_ type;
  tc_FieldType* field_type;
  char name[128];
  // tc_Meta *meta;
  tc_Field *meta;
  int own_meta;
  
  float number;
  char string[256];
  int boolean;
  // tc_Color color;
  tc_Vec2 vec2;
  tc_Vec3 vec3;
  tc_Vec4 vec4;
  // tc_Circlef circle;
  tc_Field *ref;
  void *user_data;

  tc_Field *prev;
  tc_Field *next;
  tc_Field *parent;
  tc_Field *child;
};

typedef map_t(struct tc_Field*) map_field_t;

struct tc_Meta {
  map_field_t fields;
};

TIC_API void tico_serialization_init();
// TIC_API tc_FieldFn tico_serialization_get_field_functions(tc_Serialization *serial, int type);
// TIC_API void tico_serialization_deinit(tc_Serialization)

TIC_API tc_Meta* tico_meta_from_json(cJSON *json);
TIC_API tc_Field* tico_meta_to_field(tc_Meta *meta);
TIC_API void tico_meta_destroy(tc_Meta *meta);
TIC_API void tico_meta_field_draw(tc_Meta *meta, tc_Field *field);
TIC_API void tico_meta_field_imgui(tc_Meta *meta, tc_Field *field);

TIC_API tc_Field* tico_field_create(TIC_FIELD_TYPE_ type);
TIC_API tc_Field* tico_field_load(const char *path);
TIC_API tc_Field* tico_field_from_json(cJSON *const json, tc_Field *meta_field);
TIC_API void tico_field_delete(tc_Field *field);
TIC_API void tico_field_push(tc_Field *field, int index, tc_Field *item);
TIC_API tc_Field* tico_field_set(tc_Field *field, const char *name, tc_Field *item);
TIC_API tc_Field* tico_field_get(tc_Field *field, const char *name);
TIC_API int tico_field_length(tc_Field *field);

TIC_API void tico_field_draw_imgui(tc_Field *field);
TIC_API void tico_field_draw(tc_Field *field, tc_Vec2 pos, float scale);

TIC_API tc_Field* tico_field_vec2_array_from_json(cJSON *const json);

FIELD_TYPE(number, float);
FIELD_TYPE(string, const char*);
FIELD_TYPE(boolean, int);
FIELD_TYPE(vec2, tc_Vec2);
FIELD_TYPE(vec3, tc_Vec3);
FIELD_TYPE(vec4, tc_Vec4);
// FIELD_TYPE(circle, tc_Circlef);
// FIELD_TYPE(points, tc_Field*);
// FIELD_TYPE(options, tc_Field*);
// FIELD_TYPE(multioptions, tc_Field*);
// FIELD_TYPE(resource, void*);
FIELD_TYPE(array, tc_Field*);
FIELD_TYPE(object, tc_Field*);

#endif
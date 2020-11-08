#include "serialization.h"

#define FIELD_HEADER_NAME "__header__"
#define FIELD_TYPE_NAME "__type__"
#define FIELD_DATA_NAME "__data__"
#define FIELD_META_NAME "__meta__"

#define FIELD_META_TYPE "__type__"

#define FIELD_META_COLOR "__color__"
#define FIELD_META_FORMAT "__format__"
#define FIELD_META_V_MIN "__v_min__"
#define FIELD_META_V_MAX "__v_max__"
#define FIELD_META_SPEED "__speed__"
#define FIELD_META_RES_TYPE "__res_type__"
#define FIELD_META_LOCAL_POS "__local_pos__"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"


static tc_Serialization serial;

const struct {
  const char *name;
  int type;
} type_names[] = {
  {"null", TIC_FIELD_TYPE_NULL},
  {"number", TIC_FIELD_TYPE_NUMBER},
  {"string", TIC_FIELD_TYPE_STRING},
  {"boolean", TIC_FIELD_TYPE_BOOLEAN},
  {"vec2", TIC_FIELD_TYPE_VEC2},
  {"vec3",   TIC_FIELD_TYPE_VEC3},
  {"vec4",   TIC_FIELD_TYPE_VEC4},
  // {"circle", TIC_FIELD_TYPE_CIRCLE, tico_field_circle_from_json, tico_field_circle_imgui},
  // {"points", TIC_FIELD_TYPE_POINTS, NULL, NULL},
  // {"options", TIC_FIELD_TYPE_OPTIONS, NULL, NULL},
  // {"multioptions", TIC_FIELD_TYPE_MULTIOPTIONS, NULL, NULL},
  // {"resource", TIC_FIELD_TYPE_RESOURCE, tico_field_resource_from_json, NULL},
  {"array", TIC_FIELD_TYPE_ARRAY},
  {"object", TIC_FIELD_TYPE_OBJECT},
  {"custom", TIC_FIELD_TYPE_CUSTOM},
  {"reference", TIC_FIELD_TYPE_REFERENCE}
};

static int get_type_from_string(const char *string) {
  tc_FieldType *type = map_get(&serial.types, string);
  if (type) return TIC_FIELD_TYPE_CUSTOM;
  int i;
  for (i = 0; i < TIC_FIELD_TYPE_MAX; i++) {
    if (!strcmp(type_names[i].name, string)) return type_names[i].type;
  }

  return TIC_FIELD_TYPE_NULL;
}

static tc_Vec2 get_local_pos(tc_Field *field) {
  if (!field) return tico_vec2(0, 0);
  tc_Vec2 vec = tico_vec2(0, 0);

  tc_Field *iter = field;
  while (iter) {
    tc_Vec2 pos = tico_field_get_opt_vec2(iter, "position", tico_vec2(0, 0));
    tico_vec2_add(&vec, vec, pos);

    iter = iter->parent;
  }

  return vec;
}

int get_type_from_cjson_type(int cjson_type, cJSON *const json) {
  int type = TIC_FIELD_TYPE_NULL;
  switch (cjson_type) {
    case cJSON_Number:
      type = TIC_FIELD_TYPE_NUMBER;
      break;
    case cJSON_True:
      type = TIC_FIELD_TYPE_BOOLEAN;
      break;
    case cJSON_False:
      type = TIC_FIELD_TYPE_BOOLEAN;
      break;
    case cJSON_String:
      type = TIC_FIELD_TYPE_STRING;
      break;
    case cJSON_Object:
      type = TIC_FIELD_TYPE_OBJECT;
      break;
    case cJSON_Array:
      {
        int sz = -1;
        if (json) sz = cJSON_GetArraySize(json);
        if (sz == 2) type = TIC_FIELD_TYPE_VEC2;
        else if (sz == 3) type = TIC_FIELD_TYPE_VEC3;
        else if (sz == 4) type = TIC_FIELD_TYPE_VEC4;
        else type = TIC_FIELD_TYPE_ARRAY;
      }
      break;
    default:
      TRACELOG("Unkown JSON type: %d", cjson_type);
  }

  return type;
}

static void tico_field_get_uid(tc_Field *field, char *uid) {
  ASSERT(field != NULL);
  ASSERT(uid != NULL);
  sprintf(uid, "%s##", field->name);

  tc_Field *parent = field->parent;
  while (parent) {
    strcat(uid, "_");
    strcat(uid, parent->name);
    parent = parent->parent;
  }
}

// tc_Field *tico_field_color_from_json(cJSON *const json) {
//   if (!json) return NULL;

// }

tc_Field *tico_field_circle_from_json(cJSON *const json) {
  if (!json) return NULL;
  tc_Vec2 vec = tico_json_get_vec2(json, "position", 0);
  float radius = tico_json_get_number(json, "radius", 0);

  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_OBJECT);
  tico_field_add_vec2(field, "position", vec);
  tico_field_add_number(field, "radius", radius);

  return field;
}

void tico_field_color_imgui(tc_Field *field) {
  if (!field) return;

  char uid[256];
  tico_field_get_uid(field, uid);
  igColorEdit4(uid, field->vec4.data, 0);
}

void tico_field_draw_circle(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;

  tc_Vec2 position = get_local_pos(field);
  float radius = tico_field_get_number(field, "radius");

  // tico_vec2_print(position);

  ImDrawList *list = igGetWindowDrawList();

  // int a = ImGuiTreeNodeFlags_OpenOnDoubleClick

  tico_vec2_mul(&position, position, scale);
  tico_vec2_add(&position, position, pos);
  // tc_Vec4 vec_color = tico_field_arra
  tc_Vec4 vec_color = tico_field_get_opt_vec4(field->meta, FIELD_META_COLOR, tico_vec4(255, 255, 255, 255));
  // TRACELOG("%f", im_color.x);
  // tico_vec4_print(vec_color);
  ImVec4 im_color;
  memcpy(&im_color, &vec_color, sizeof(ImVec4));
  ImU32 color = igColorConvertFloat4ToU32(im_color);
  // TRACELOG("%d", color);

  ImDrawList_AddCircle(list, (ImVec2){position.x, position.y}, radius*scale, color, 32, 2);
}

void tico_field_circle_imgui(tc_Field *field) {
  if (!field) return;
  // if (field->type != TIC_FIELD_TYPE_CIRCLE) return;

  // tc_Circlef *circle = &field->circle;
    char uid[512];
  // tico_field_get_uid(field, uid);
  // if (igCollapsingHeaderTreeNodeFlags(uid, 0)) {
    const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");
    float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
    float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);

    float step = tico_field_get_opt_number(field->meta, FIELD_META_SPEED, 1);

    tc_Field *pos_f = tico_field_get(field, "position");
    tc_Field *rad_f = tico_field_get(field, "radius");

    tc_Field *colorf = tico_field_get(field->meta, FIELD_META_COLOR);
    // char uid[256];

    tico_field_get_uid(pos_f, uid);
    igDragFloat2(uid, pos_f->vec2.data, step, v_min, v_max, format, 0);
    tico_field_get_uid(rad_f, uid);
    igDragFloat(uid, &rad_f->number, step, v_min, v_max, "%.0f", 0);
    if (colorf) igColorEdit4("color", colorf->vec4.data, 0);
  // }
}


void tico_field_resource_from_json(cJSON *const json) {

}

void tico_field_resource_draw(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;

  tc_Resource *res = tico_plugin_resources_get_by_field(field);
  if (!res) return;

  tc_Image *image = NULL;
  const char *img_name = tico_json_get_string(res->data, "image", 0);
  tc_Resource *img_res = tico_plugin_resources_get_by_key(img_name);
  if (!img_res) return;

  image = img_res->data;

  tc_Vec2 origin = tico_field_get_opt_vec2(field->meta, "origin", tico_vec2_zero());
  tico_vec2_mul(&origin, origin, scale);
  tc_Vec2 vec = get_local_pos(field);
  tico_vec2_mul(&vec, vec, scale);

  float tw = tico_json_get_number(res->data, "width", 0);
  float th = tico_json_get_number(res->data, "height", 0);
  float w = image->width;
  float h = image->height;

  tico_vec2_add(&vec, vec, pos);
  tico_vec2_sub(&vec, vec, origin);
  ImVec2 pos0 = (ImVec2){vec.x, vec.y};
  ImVec2 pos1 = (ImVec2){pos0.x + tw*scale, pos0.y + th*scale};

  ImVec2 uv0 = (ImVec2){0, 0};
  ImVec2 uv1 = (ImVec2){tw/w, th/h};

  ImDrawList *list = igGetWindowDrawList();

  tc_Vec4 col = tico_field_get_opt_vec4(field->meta, FIELD_META_COLOR, tico_vec4(1, 1, 1, 1));
  ImVec4 im_col;
  memcpy(&im_col, &col, sizeof(ImVec4));
  int color = igColorConvertFloat4ToU32(im_col);

  if (image) ImDrawList_AddImage(list, image->texture.id, pos0, pos1, uv0, uv1, color);
}

void tico_field_resource_imgui(tc_Field *field) {
  if (!field) return;

  tico_field_string_imgui(field);
  tc_Field *origin = tico_field_get(field->meta, "origin");
  tc_Field *color = tico_field_get(field->meta, FIELD_META_COLOR);

  if (origin) tico_field_vec2_imgui(origin);
  if (color) tico_field_color_imgui(color);
}

void tico_serialization_init() {
  // ASSERT(serial != NULL);
  memset(&serial, 0, sizeof(tc_Serialization));
  map_init(&serial.types);

  tc_FieldType circle = {0};
  circle.load = tico_field_circle_from_json;
  circle.draw = tico_field_draw_circle;
  circle.imgui = tico_field_circle_imgui;

  tc_FieldType resource = {0};
  resource.load = tico_field_string_from_json;
  resource.draw = tico_field_resource_draw;
  resource.imgui = tico_field_resource_imgui;

  map_set(&serial.types, "circle", circle);
  map_set(&serial.types, "resource", resource);

  // tc_FieldType *type = map_get(&serial->types, key)
  // memset(serial->field_types, TIC_FIELD_TYPE_MAX, sizeof(tc_FieldFn));

  // serial->field_types[TIC_FIELD_TYPE_NUMBER] = (tc_FieldFn){tico_field_number_from_json, NULL, tico_field_number_imgui};
  // serial->field_types[TIC_FIELD_TYPE_STRING] = (tc_FieldFn){tico_field_string_from_json, NULL, tico_field_string_imgui};
  // serial->field_types[TIC_FIELD_TYPE_BOOLEAN] = (tc_FieldFn){tico_field_boolean_from_json, NULL, tico_field_boolean_imgui};
  // serial->field_types[TIC_FIELD_TYPE_VEC2] = (tc_FieldFn){tico_field_vec2_from_json, NULL, tico_field_vec2_imgui};
  // serial->field_types[TIC_FIELD_TYPE_VEC3] = (tc_FieldFn){tico_field_vec3_from_json, NULL, tico_field_vec3_imgui};
  // serial->field_types[TIC_FIELD_TYPE_VEC4] = (tc_FieldFn){tico_field_vec4_from_json, NULL, tico_field_vec4_imgui};
  // // serial->field_types[TIC_FIELD_TYPE_CIRCLE] = (tc_FieldFn){tico_field_circle_from_json, NULL, tico_field_circle_imgui};
  // // serial->field_types[TIC_FIELD_TYPE_RESOURCE] = (tc_FieldFn){tico_field_resource_from_json, NULL, tico_field_resource_imgui};
  // // serial->field_types[TIC_FIELD_TYPE_POINTS] = (tc_FieldFn){tico_field_points_from_json, NULL, NULL};
  // serial->field_types[TIC_FIELD_TYPE_ARRAY] = (tc_FieldFn){tico_field_array_from_json, NULL, tico_field_array_imgui};
  // serial->field_types[TIC_FIELD_TYPE_OBJECT] = (tc_FieldFn){tico_field_object_from_json, NULL, tico_field_object_imgui};
}

tc_FieldType *tico_serialization_set_type(tc_Serialization *serial, const char *typename, tc_FieldType type) {
  ASSERT(serial != NULL);
  if (!typename) return NULL;

  map_set(&serial->types, typename, type);

  tc_FieldType *ptr_type = map_get(&serial->types, typename);

  return ptr_type;
}

tc_FieldType *tico_serialization_get_type(tc_Serialization *serial, const char *typename) {
  ASSERT(serial != NULL);
  if (!typename) return NULL;
  tc_FieldType *type = map_get(&serial->types, typename);

  return type;
}

// tc_FieldFn tico_serialization_get_field_functions(tc_Serialization *serial, int type) {
//   ASSERT(serial != NULL);
//   if (type < 0 || type >= TIC_FIELD_TYPE_MAX) return (tc_FieldFn){0};

//   // TRACELOG("%d", type);

//   return serial->field_types[type];
// }

tc_Meta* tico_meta_from_json(cJSON *json) {
  // ASSERT(json != NULL);
  if (!json) return NULL;
  tc_Meta *meta = calloc(1, sizeof(*meta));
  map_init(&meta->fields);

  cJSON *item = NULL;

  cJSON_ArrayForEach(item, json) {
    tc_Field *field = tico_field_from_json(item, NULL);
    map_set(&meta->fields, json->string, field);
  }

  return meta;
}

void tico_meta_destroy(tc_Meta *meta) {
  if (!meta) return;
  map_iter_t iter = map_iter(&meta->field);
  const char *key;
  while ((key = map_next(&meta->fields, &iter))) {
    tc_Field **field = map_get(&meta->fields, key);
    if (*field) tico_field_delete(*field);
  }
  map_deinit(&meta->fields);
}

void tico_meta_field_draw(tc_Meta *meta, tc_Field *field) {
  if (!field || !meta) return;

  tc_Field **field_type = map_get(&meta->fields, field->name);
  if (!field_type) return;
}

tc_Field* tico_field_create(TIC_FIELD_TYPE_ type) {
  tc_Field *field = malloc(sizeof(*field));
  memset(field, 0, sizeof(*field));

  field->next = NULL;
  field->prev = NULL;
  // field->parent = NULL;
  field->child = NULL;
  field->meta = NULL;
  field->ref = NULL;
  field->user_data = NULL;

  // field.type = type;
  field->type = type;

  return field;
}

tc_Field* tico_field_vec2_array_from_json(cJSON *const array) {
  if (!array) return NULL;
  tc_Field *field = tico_field_create_array(NULL);


  cJSON *item = NULL;
  cJSON_ArrayForEach(item, array) {
    if (tico_json_is_vec2(item)) {
      tc_Vec2 vec = tico_json_to_vec2(item);
      tc_Field *v = tico_field_create_vec2(vec);
      tico_field_push(field, -1, v);
    }
  }

  return field;
}

tc_Field* tico_field_from_json(cJSON *json, tc_Field *meta_field) {
  if (!json) return NULL;
  tc_Field *field = NULL;
  const char *typename = NULL;
  if (meta_field) typename = tico_field_get_string(meta_field, FIELD_META_TYPE);

  int type = -1;
  if (typename) type = get_type_from_string(typename);

  if (type == -1) type = get_type_from_cjson_type(json->type, json);
  // TRACELOG("%d", type);

  // tc_FieldFn field_type = tico_serialization_get_field_functions(&serial, type);
  // if (field_type.from_json) field = field_type.from_json(json);
  // if (serial.field_types[type].from_json) field = serial.field_types[type].from_json(json);
  // TRACELOG("%d %p", type, field);
  
  // for (int i = 0; i < TIC_FIELD_TYPE_MAX; i++) {
  //   if (i == type_names[i].type && type_names[i].from_json) field = type_names[i].from_json(json);
  // }

  

  switch (type) {
    case TIC_FIELD_TYPE_NUMBER:
      field = tico_field_number_from_json(json);
      break;
    case TIC_FIELD_TYPE_STRING:
      field = tico_field_string_from_json(json);
      break;
    case TIC_FIELD_TYPE_BOOLEAN:
      field = tico_field_boolean_from_json(json);
      break;
    case TIC_FIELD_TYPE_VEC2:
      field = tico_field_vec2_from_json(json);
      break;
    case TIC_FIELD_TYPE_VEC3:
      field = tico_field_vec3_from_json(json);
      break;
    case TIC_FIELD_TYPE_VEC4:
      field = tico_field_vec4_from_json(json);
      break;
    // case TIC_FIELD_TYPE_CIRCLE:
    //   field = tico_field_circle_from_json(json);
    //   break;
    case TIC_FIELD_TYPE_ARRAY:
      field = tico_field_array_from_json(json);
      break;
    case TIC_FIELD_TYPE_OBJECT:
      field = tico_field_object_from_json(json);
      break;
    case TIC_FIELD_TYPE_CUSTOM:
      {
        tc_FieldType *field_type = tico_serialization_get_type(&serial, typename);
        if (field_type->load) field = field_type->load(json);
        // TRACELOG("%p", field);
        if (field) field->field_type = field_type;
      }
      break;
    default:
      TRACELOG("Unknown type");
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
    // case TIC_FIELD_TYPE_NUMBER:
  }

  if (field && meta_field) {
    field->meta = meta_field;
    field->type = type;
  }
  // TRACELOG("%s %p", json->string, field);

  return field;
}

void tico_field_delete(tc_Field *field) {
  if (!field) return;
  if (field->next) tico_field_delete(field->next);
  if (field->child) tico_field_delete(field->child);
  if (field->own_meta && field->meta) tico_field_delete(field->meta);
  // if (field->meta) tico_meta_destroy(field->meta);
  free(field);
}

tc_Field *tico_field_get(tc_Field *field, const char *name) {
  if (!field || !name) return NULL;

  tc_Field *item = NULL;
  tico_field_foreach(item, field) {
    if (!strcmp(item->name, name)) return item;
  }

  return NULL;
}

tc_Field *tico_field_index(tc_Field *field, int index) {
  if (!field) return NULL;
  if (index < 0) index = tico_field_length(field) + index;
  int i = 0;
  tc_Field *el = NULL;
  tico_field_foreach(el, field) {
    if (i == index) return el;
    i++;
  }

  return NULL;
}

tc_Field *tico_field_set(tc_Field *field, const char *name, tc_Field *item) {
  if (!field || !name || !item) return NULL;

  tc_Field *el = NULL;
  tico_field_foreach(el, field) {
    if (!strcmp(el->name, name)) return el;
  }

  strcpy(item->name, name);
  if (!field->child) {
    field->child = item;
    return field->child;
  }

  el = field->child;
  while (el->next) el = el->next;

  el->next = item;
  item->prev = el;
  item->parent = field;

  return item;
}

void tico_field_push(tc_Field *field, int index, tc_Field *item) {
  ASSERT(field != NULL);
  if (field->type != TIC_FIELD_TYPE_ARRAY) return;
  if (!item) return;


  int i = 0;
  if (!field->child) {
    field->child = item;
    return;
  }

  tc_Field *el = field->child;
  while (el->next) {
    el = el->next;
  }

  el->next = item;
  item->prev = el;
  item->parent = field;
}

int tico_field_length(tc_Field *field) {
  if (!field) return -1;

  int len = 0;
  tc_Field *el = NULL;
  tico_field_foreach(el, field) len++;

  return len;
}

void tico_field_draw(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;


  int type = field->type;
  // if (!strcmp(field->name, "circle")) TRACELOG("%d", type);
  // if (!global_pos && field->parent) {
  //   tc_Vec2 vec = tico_field_get_opt_vec2(field, "position", tico_vec2(0, 0));
  //   tico_vec2_add(&pos, pos, vec);
  // }
  switch (type) {
      case TIC_FIELD_TYPE_NUMBER:
        tico_field_draw_number(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_STRING:
        tico_field_draw_string(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_BOOLEAN:
        tico_field_draw_boolean(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_VEC2:
        tico_field_draw_vec2(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_VEC3:
        tico_field_draw_vec3(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_VEC4:
        tico_field_draw_vec4(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_ARRAY:
        tico_field_draw_array(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_OBJECT:
        tico_field_draw_object(field, pos, scale);
        break;
      case TIC_FIELD_TYPE_CUSTOM:
        {
          // tc_FieldType *field_type = tico_serialization_get_type(&serial, typename);
          // if (field_type->load) field = field_type->load(json);
          // if (field) field->field_type = field_type;
          if (field->field_type && field->field_type->draw)
              field->field_type->draw(field, pos, scale);
          break;
        }
      default:
        TRACELOG("Unknown type");
    }
}

void tico_field_draw_meta_imgui(tc_Field *field) {

}

void tico_field_draw_imgui(tc_Field *field) {
  if (!field) return;

  int type = field->type;
  ImVec2 size;
  char name[128];
  igGetContentRegionAvail(&size);
  // if (field->name[0] && type == TIC_FIELD_TYPE_OBJECT) strcpy(name, field->name);
  strcpy(name, field->name);
  if (!field->name[0] && type == TIC_FIELD_TYPE_OBJECT) strcpy(name, "object");

  
    // igIndent(8);
    switch (type) {
      case TIC_FIELD_TYPE_NUMBER:
        tico_field_number_imgui(field);
        break;
      case TIC_FIELD_TYPE_STRING:
        tico_field_string_imgui(field);
        break;
      case TIC_FIELD_TYPE_BOOLEAN:
        tico_field_boolean_imgui(field);
        break;
      case TIC_FIELD_TYPE_VEC2:
        tico_field_vec2_imgui(field);
        break;
      case TIC_FIELD_TYPE_VEC3:
        tico_field_vec3_imgui(field);
        break;
      case TIC_FIELD_TYPE_VEC4:
        tico_field_vec4_imgui(field);
        break;
      case TIC_FIELD_TYPE_ARRAY:
        tico_field_array_imgui(field);
        break;
      case TIC_FIELD_TYPE_OBJECT:
        tico_field_object_imgui(field);
        break;
      case TIC_FIELD_TYPE_CUSTOM:
        {
          // tc_FieldType *field_type = tico_serialization_get_type(&serial, typename);
          // if (field_type->load) field = field_type->load(json);
          // if (field) field->field_type = field_type;
          if (field->field_type && field->field_type->imgui)
              field->field_type->imgui(field);
          break;
        }
      default:
        TRACELOG("Unknown type");
    }

  // ImDrawList *list = igGetWindowDrawList();
  // ImVec2 pmin, pmax;
  // igGetItemRectMin(&pmin);
  // igGetItemRectMax(&pmax);

  // ImDrawList_AddRect(list, pmin, pmin, 0xffffffff, 0, 0, 2);
  // igEndChild();

  // for (int i = 0; i < TIC_FIELD_TYPE_MAX; i++) {
  //   if (i == type_names[i].type && type_names[i].imgui) type_names[i].imgui(field);
  // }
}

/******************
 * Number
 ******************/

tc_Field* tico_field_create_number(float number) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_NUMBER);
  if (!field) return NULL;

  tico_field_set_number(field, number);

  return field;
}

tc_Field *tico_field_number_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Number) return NULL;

  float number = tico_json_to_number(json);
  return tico_field_create_number(number);
}

void tico_field_set_number(tc_Field *field, float number) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_NUMBER) return;

  field->number = number;
}

float tico_field_to_number(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type != TIC_FIELD_TYPE_NUMBER);

  return tico_field_to_opt_number(field, -1);
}

float tico_field_to_opt_number(tc_Field *field, float opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_NUMBER) return opt;

  return field->number;
}

float tico_field_get_number(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_number(field, name, -1);
}

float tico_field_get_opt_number(tc_Field *field, const char *name, float opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_number(item, opt);
}

tc_Field *tico_field_add_number(tc_Field *field, const char *name, float number) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_number(number);
  tico_field_set(field, name, n_field);

  return n_field;
}

void tico_field_draw_number(tc_Field *field, tc_Vec2 pos, float scale) {}

void tico_field_number_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_NUMBER) return;

  float *number = &field->number;

  float speed = tico_field_get_opt_number(field->meta, FIELD_META_SPEED, 1);
  float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
  float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);
  const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");

  igDragFloat(field->name, number, speed, v_min, v_max, format, 0);
  *number = tico_clamp(*number, v_min, v_max);
}

/*****************
 * String
 *****************/

tc_Field* tico_field_create_string(const char *string) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_STRING);
  if (!field) return NULL;

  tico_field_set_string(field, string);

  return field;
}

tc_Field *tico_field_string_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_String) return NULL;

  const char *string = tico_json_to_string(json);
  return tico_field_create_string(string);
}

void tico_field_set_string(tc_Field *field, const char *string) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_STRING) return;

  // field->string = string;
  strcpy(field->string, string);
}


const char* tico_field_to_string(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type == TIC_FIELD_TYPE_STRING);

  return tico_field_to_opt_string(field, NULL);
}

const char* tico_field_to_opt_string(tc_Field *field, const char* opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_STRING) return opt;

  return field->string;
}

const char* tico_field_get_string(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_string(field, name, NULL);
}

const char* tico_field_get_opt_string(tc_Field *field, const char *name, const char* opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_string(item, opt);
}

tc_Field *tico_field_add_string(tc_Field *field, const char *name, const char *string) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_string(string);
  tico_field_set(field, name, n_field);

  return n_field;
}

#include "font_data.h"

void tico_field_draw_string(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_STRING && field->type != TIC_FIELD_TYPE_CUSTOM) return;

  tc_Field *parent = field->parent;
  // tc_Vec2 local = tico_vec2_zero();
  // int a = TIC_FIELD_TYPE_VEC2;

  // tc_Field *posf = tico_field_get(parent, "position");
  // TRACELOG("%s %d", posf->name, posf->type);
  // if (posf) local = tico_field_to_vec2(posf);
  tc_Vec2 local = get_local_pos(field);

  tico_vec2_mul(&local, local, scale);
  tico_vec2_add(&local, local, pos);

  tc_Vec4 color_v = tico_field_get_opt_vec4(field->meta, FIELD_META_COLOR, tico_vec4(255, 255, 255, 255));
  ImVec4 im_color = (ImVec4){color_v.x, color_v.y, color_v.z, color_v.w};

  int color = igColorConvertFloat4ToU32(im_color);

  // tico_vec2_print(local);
  // float w = strlen(field->string);
  int len = strlen(field->string);
  int w = tico_font_get_text_width(Core.graphics.default_font, field->string, len);
  // int h = tico_font_get_text_height(Core.graphics.default_font, field->string, len);
  ImDrawList *list = igGetWindowDrawList();
  ImVec2 pos0 = (ImVec2){local.x-4, local.y-4};
  ImVec2 pos1 = (ImVec2){pos0.x + w+8, pos0.y +  16 + 8};
  ImDrawList_AddRectFilled(list, pos0, pos1, 0xff000000, 4, ImDrawCornerFlags_All);
  ImDrawList_AddTextVec2(list, (ImVec2){local.x, local.y}, color, field->string, NULL);

}

void tico_field_string_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_STRING && field->type != TIC_FIELD_TYPE_CUSTOM) return;
  char *string = field->string;

  char key[256];
  tico_field_get_uid(field, key);

  igInputText(key, string, 256, 0, NULL, NULL);
}


/*****************
 * Boolean
 *****************/

tc_Field* tico_field_create_boolean(int boolean) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_BOOLEAN);
  if (!field) return NULL;

  tico_field_set_boolean(field, boolean);

  return field;
}

tc_Field *tico_field_boolean_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (!cJSON_IsBool(json)) return NULL;

  int boolean = tico_json_to_boolean(json);
  return tico_field_create_boolean(boolean);
}

void tico_field_set_boolean(tc_Field *field, int boolean) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_BOOLEAN) return;

  field->boolean = boolean;
}

int tico_field_to_boolean(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type != TIC_FIELD_TYPE_BOOLEAN);

  return tico_field_to_opt_boolean(field, 0);
}

int tico_field_to_opt_boolean(tc_Field *field, int opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_BOOLEAN) return opt;

  return field->boolean;
}

int tico_field_get_boolean(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_boolean(field, name, 0);
}

int tico_field_get_opt_boolean(tc_Field *field, const char *name, int opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_boolean(item, opt);
}

tc_Field *tico_field_add_boolean(tc_Field *field, const char *name, int boolean) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_boolean(boolean);
  tico_field_set(field, name, n_field);

  return n_field;
}

void tico_field_draw_boolean(tc_Field *field, tc_Vec2 pos, float scale) {}

void tico_field_boolean_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_BOOLEAN) return;

  bool *boolean = (bool*)&field->boolean;

  igCheckbox(field->name, boolean);
}

/*****************
 * Vec2
 *****************/

tc_Field* tico_field_create_vec2(tc_Vec2 vec) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_VEC2);
  if (!field) return NULL;

  tico_field_set_vec2(field, vec);

  return field;
}

tc_Field *tico_field_vec2_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Array) return NULL;

  tc_Vec2 vec = tico_json_to_vec2(json);
  return tico_field_create_vec2(vec);
}

void tico_field_set_vec2(tc_Field *field, tc_Vec2 vec) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC2) return;

  field->vec2 = vec;
}

tc_Vec2 tico_field_to_vec2(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type == TIC_FIELD_TYPE_VEC2);

  return tico_field_to_opt_vec2(field, tico_vec2(-1, -1));
}

tc_Vec2 tico_field_to_opt_vec2(tc_Field *field, tc_Vec2 opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_VEC2) return opt;

  return field->vec2;
}

tc_Vec2 tico_field_get_vec2(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_vec2(field, name, tico_vec2(-1, -1));
}

tc_Vec2 tico_field_get_opt_vec2(tc_Field *field, const char *name, tc_Vec2 opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_vec2(item, opt);
}

tc_Field *tico_field_add_vec2(tc_Field *field, const char *name, tc_Vec2 vec2) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_vec2(vec2);
  tico_field_set(field, name, n_field);

  return n_field;
}

void tico_field_draw_vec2(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;

  // if (strcmp(field->name, "position")) return;

  tc_Vec2 local = get_local_pos(field);
  tico_vec2_mul(&local, local, scale);
  // tico_vec2_print(local);

  tc_Vec2 position = tico_field_to_opt_vec2(field, tico_vec2(0, 0));
  tico_vec2_mul(&position, position, scale);

  // tc_Vec2 local = tico_field_get_opt_vec2(field, "position", tico_vec2(0, 0));

  // tico_vec2_mul(&local, local, scale);
  tico_vec2_add(&local, local, position);
  tico_vec2_add(&local, local, pos);

  tc_Vec4 color_v = tico_field_get_opt_vec4(field->meta, FIELD_META_COLOR, tico_vec4(255, 0, 255, 255));
  ImVec4 im_color = (ImVec4){color_v.x, color_v.y, color_v.z, color_v.w};

  int color = igColorConvertFloat4ToU32(im_color);

  ImDrawList *list = igGetWindowDrawList();

  ImDrawList_AddCircleFilled(list, (ImVec2){local.x, local.y}, scale/2, color, 32);

}

void tico_field_vec2_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC2) return;

  const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");
  float speed = tico_field_get_opt_number(field->meta, FIELD_META_SPEED, 1);
  float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
  float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);
  tc_Vec2 *vec = &field->vec2;

  tc_Field *colorf = tico_field_get(field->meta, FIELD_META_COLOR);

  char key[256];
  tico_field_get_uid(field, key);

  igDragFloat2(key, vec->data, speed, v_min, v_max, format, 0);
  if (colorf) tico_field_color_imgui(colorf);
}

/*****************
 * Vec3
 *****************/

tc_Field* tico_field_create_vec3(tc_Vec3 vec) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_VEC3);
  if (!field) return NULL;

  tico_field_set_vec3(field, vec);

  return field;
}

tc_Field *tico_field_vec3_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Array) return NULL;

  tc_Vec3 vec = tico_json_to_vec3(json);
  return tico_field_create_vec3(vec);
}

void tico_field_set_vec3(tc_Field *field, tc_Vec3 vec) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC3) return;

  field->vec3 = vec;
}

tc_Vec3 tico_field_to_vec3(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type != TIC_FIELD_TYPE_VEC3);

  return tico_field_to_opt_vec3(field, tico_vec3(-1, -1, -1));
}

tc_Vec3 tico_field_to_opt_vec3(tc_Field *field, tc_Vec3 opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_VEC3) return opt;

  return field->vec3;
}

tc_Vec3 tico_field_get_vec3(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_vec3(field, name, tico_vec3(-1, -1, -1));
}

tc_Vec3 tico_field_get_opt_vec3(tc_Field *field, const char *name, tc_Vec3 opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_vec3(item, opt);
}

tc_Field *tico_field_add_vec3(tc_Field *field, const char *name, tc_Vec3 vec3) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_vec3(vec3);
  tico_field_set(field, name, n_field);

  return n_field;
}

void tico_field_draw_vec3(tc_Field *field, tc_Vec2 pos, float scale) {}

void tico_field_vec3_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC3) return;

  const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");
  float speed = tico_field_get_opt_number(field->meta, FIELD_META_SPEED, 1);
  float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
  float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);
  tc_Vec3 *vec = &field->vec3;

  igDragFloat3(field->name, vec->data, speed, v_min, v_max, format, 0);
}

/*****************
 * Vec4
 *****************/

tc_Field* tico_field_create_vec4(tc_Vec4 vec) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_VEC4);
  if (!field) return NULL;

  tico_field_set_vec4(field, vec);

  return field;
}

tc_Field *tico_field_vec4_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Array) return NULL;

  tc_Vec4 vec = tico_json_to_vec4(json);
  return tico_field_create_vec4(vec);
}

void tico_field_set_vec4(tc_Field *field, tc_Vec4 vec) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC4) return;

  field->vec4 = vec;
}

tc_Vec4 tico_field_to_vec4(tc_Field *field) {
  ASSERT(field != NULL);

  return tico_field_to_opt_vec4(field, tico_vec4(-1, -1, -1, -1));
}

tc_Vec4 tico_field_to_opt_vec4(tc_Field *field, tc_Vec4 opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_VEC4) return opt;

  return field->vec4;
}

tc_Vec4 tico_field_get_vec4(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_vec4(field, name, tico_vec4(-1, -1, -1, -1));
}

tc_Vec4 tico_field_get_opt_vec4(tc_Field *field, const char *name, tc_Vec4 opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_vec4(item, opt);
}

tc_Field *tico_field_add_vec4(tc_Field *field, const char *name, tc_Vec4 vec4) {
  if (!field) return NULL;
  if (!name) return NULL;

  tc_Field *n_field = tico_field_create_vec4(vec4);
  tico_field_set(field, name, n_field);

  return n_field;
}

void tico_field_draw_vec4(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;
  ImDrawList *list = igGetWindowDrawList();

  tc_Vec2 local = get_local_pos(field);
  tico_vec2_mul(&local, local, scale);

  tc_Vec4 vec = tico_field_to_vec4(field);

  tico_vec4_mul(&vec, vec, scale);

  ImVec2 pos0 = {pos.x + vec.x + local.x, local.y + pos.y + vec.y};
  ImVec2 pos1 = {pos0.x + vec.z, pos0.y + vec.w};

  tc_Vec4 vec_color = tico_field_get_opt_vec4(field->meta, FIELD_META_COLOR, tico_vec4(255, 255, 255, 255));
  // TRACELOG("%f", im_color.x);
  // tico_vec4_print(vec_color);
  ImVec4 im_color;
  memcpy(&im_color, &vec_color, sizeof(ImVec4));
  ImU32 color = igColorConvertFloat4ToU32(im_color);

  // int color = tico_field_get_opt_boolean(field->meta, "color", 0xffffffff);
  // TRACELOG("%d", color);
  // pos0.x = pos.x + (vec-x*scale);
  // pos0.y = pos.y + (vec.y*scale);
  // float radius = scale;

  ImDrawList_AddRect(list, pos0, pos1, color, 0, 0, 2);
}

void tico_field_vec4_imgui(tc_Field *field) {
  if (!field) return;
  if (field->type != TIC_FIELD_TYPE_VEC4) return;


  char uid[256];
  tico_field_get_uid(field, uid);

  const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");
  float speed = tico_field_get_opt_number(field->meta, FIELD_META_SPEED, 1);
  float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
  float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);
  tc_Vec4 *vec = &field->vec4;

  tc_Field *colorf = tico_field_get(field->meta, FIELD_META_COLOR);


  igDragFloat4(uid, vec->data, speed, v_min, v_max, format, 0);
  
  sprintf(uid, "color##%s", uid);
  if (colorf) igColorEdit4(uid, colorf->vec4.data, 0);
}

/*****************
 * Circle
 *****************/

// tc_Field* tico_field_create_circle(tc_Circlef circle) {
//   tc_Field *field = tico_field_create(TIC_FIELD_TYPE_CIRCLE);
//   if (!field) return NULL;

//   tico_field_set_circle(field, circle);

//   return field;
// }

// tc_Field *tico_field_circle_from_json(cJSON *const json) {
//   if (!json) return NULL;
//   if (json->type != cJSON_Object) return NULL;

//   tc_Circlef circle = tico_json_to_circle(json);
//   return tico_field_create_circle(circle);
// }

// tc_Circlef tico_field_to_circle(tc_Field *field) {
//   ASSERT(field != NULL);
//   ASSERT(field->type != TIC_FIELD_TYPE_CIRCLE);

//   return tico_field_to_opt_circle(field, (tc_Circlef){0, 0, 0});
// }

// tc_Circlef tico_field_to_opt_circle(tc_Field *field, tc_Circlef opt) {
//   if (!field) return opt;
//   if (field->type != TIC_FIELD_TYPE_CIRCLE) return opt;

//   return field->circle;
// }

// void tico_field_set_circle(tc_Field *field, tc_Circlef circle) {
//   if (!field) return;
//   if (field->type != TIC_FIELD_TYPE_CIRCLE) return;

//   memcpy(&field->circle, &circle, sizeof(tc_Circlef));
// }

// tc_Circlef tico_field_get_circle(tc_Field *field, const char *name) {
//   ASSERT(field != NULL);
//   return tico_field_get_opt_circle(field, name, (tc_Circlef){0, 0, 0});
// }

// tc_Circlef tico_field_get_opt_circle(tc_Field *field, const char *name, tc_Circlef opt) {
//   if (!field) return opt;
//   tc_Field *item = tico_field_get(field, name);
//   return tico_field_to_opt_circle(item, opt);
// }

// tc_Field *tico_field_add_circle(tc_Field *field, const char *name, tc_Circlef circle) {
//   if (!field) return NULL;
//   if (!name) return NULL;

//   tc_Field *circ = tico_field_create_circle(circle);
//   tico_field_set(field, name, circ);

//   return circ;
// }

// void tico_field_circle_imgui(tc_Field *field) {
//   if (!field) return;
//   if (field->type != TIC_FIELD_TYPE_CIRCLE) return;

//   tc_Circlef *circle = &field->circle;
//   const char *format = tico_field_get_opt_string(field->meta, FIELD_META_FORMAT, "%f");
//   float v_min = tico_field_get_opt_number(field->meta, FIELD_META_V_MIN, -1000000);
//   float v_max = tico_field_get_opt_number(field->meta, FIELD_META_V_MAX,  1000000);

//   float step = tico_field_get_opt_number(field->meta, "step", 1);

//   igDragFloat2("position", circle->data, step, v_min, v_max, format, 0);
//   igDragFloat("radius", &circle->radius, step, v_min, v_max, "%.0f", 0);
// }

// void tico_field_draw_circle(tc_Field *field, tc_Field *meta_field) {

// }

/*****************
 * Points
 *****************/

// tc_Field* tico_field_create_points(tc_Field *ref) {
//   tc_Field *field = tico_field_create(TIC_FIELD_TYPE_POINTS);
//   if (!field) return NULL;

//   // tico_field_set_vec2(field, vec);

//   return field;
// }

// tc_Field *tico_field_points_from_json(cJSON *const json) {
//   if (!json) return NULL;
//   if (json->type != cJSON_Array) return NULL;

//   tc_Field *field = tico_field_create_array(NULL);


//   cJSON *item = NULL;
//   cJSON_ArrayForEach(item, json) {
//     if (tico_json_is_vec2(item)) {
//       tc_Vec2 vec = tico_json_to_vec2(item);
//       tico_field_add_vec2(field, NULL, vec);
//     }
//   }

//   return field;
// }

/*****************
 * Array
 *****************/

tc_Field* tico_field_create_array(tc_Field *ref) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_ARRAY);
  if (!field) return NULL;

  // tico_field_set_vec2(field, vec);

  return field;
}

tc_Field *tico_field_array_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Array) return NULL;

  tc_Field *field = tico_field_create_array(NULL);


  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {
    tc_Field *aux = tico_field_from_json(item, NULL);

    if (aux) tico_field_push(field, -1, aux);
  }

  return field;
}

tc_Field *tico_field_to_array(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type != TIC_FIELD_TYPE_ARRAY);

  return tico_field_to_opt_array(field, NULL);
}

void tico_field_set_array(tc_Field *field, tc_Field *array) {

}

tc_Field *tico_field_to_opt_array(tc_Field *field, tc_Field *opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_ARRAY) return opt;

  return NULL;
}

tc_Field* tico_field_get_array(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_array(field, name, NULL);
}

tc_Field* tico_field_get_opt_array(tc_Field *field, const char *name, tc_Field* opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_array(item, opt);
}

tc_Field* tico_field_add_array(tc_Field *field, const char *name, tc_Field *array) {
  if (!field) return NULL;
  tico_field_push(field, 0, array);

  return array;
}

void tico_field_draw_array(tc_Field *field, tc_Vec2 pos, float scale) {}

void tico_field_array_imgui(tc_Field *field) {
  if (!field) return;

  tc_Field *el = NULL;
  tico_field_foreach(el, field) {
    tico_field_draw_imgui(el);
  }
}

/*****************
 * Object
 *****************/

tc_Field* tico_field_create_object(tc_Field *ref) {
  tc_Field *field = tico_field_create(TIC_FIELD_TYPE_OBJECT);
  if (!field) return NULL;

  return field;
}

tc_Field *tico_field_object_from_json(cJSON *const json) {
  if (!json) return NULL;
  if (json->type != cJSON_Object) return NULL;

  tc_Field *field = tico_field_create_object(NULL);
  // tc_Meta *meta = NULL;
  tc_Field *meta = NULL;

  cJSON *json_meta = cJSON_GetObjectItem(json, FIELD_META_NAME);
  // tico_field_from_json(jso, tc_Field *meta_field)
  // TRACELOG("%p", json_meta);
  if (json_meta) {
    meta = tico_field_from_json(json_meta, NULL);
    field->meta = meta;
    field->own_meta = 1;
  }

  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {
    // if (strstr(item->string, "__")) continue;
    if (!strcmp(item->string, FIELD_META_NAME)) continue;
    if (!strcmp(item->string, FIELD_HEADER_NAME)) continue;
    tc_Field *meta_field = NULL;
    // TRACELOG("%s", item->string);
    if (meta) meta_field = tico_field_get(meta, item->string);
    tc_Field *aux = tico_field_from_json(item, meta_field);
    // TRACELOG("%s %p", item->string, aux);

    if (aux) tico_field_set(field, item->string, aux);
  }

  return field;
}

void tico_field_set_object(tc_Field *field, tc_Field *object) {}

tc_Field *tico_field_to_object(tc_Field *field) {
  ASSERT(field != NULL);
  ASSERT(field->type != TIC_FIELD_TYPE_ARRAY);

  return tico_field_to_opt_object(field, NULL);
}

tc_Field *tico_field_to_opt_object(tc_Field *field, tc_Field *opt) {
  if (!field) return opt;
  if (field->type != TIC_FIELD_TYPE_ARRAY) return opt;

  return field;
}

tc_Field* tico_field_get_object(tc_Field *field, const char *name) {
  ASSERT(field != NULL);
  return tico_field_get_opt_object(field, name, NULL);
}

tc_Field* tico_field_get_opt_object(tc_Field *field, const char *name, tc_Field* opt) {
  if (!field) return opt;
  tc_Field *item = tico_field_get(field, name);
  return tico_field_to_opt_object(item, opt);
}

tc_Field* tico_field_add_object(tc_Field *field, const char *name, tc_Field *object) {
  if (!field) return NULL;

  return tico_field_set(field, name, object);
}

void tico_field_draw_object(tc_Field *field, tc_Vec2 pos, float scale) {
  if (!field) return;
  // tc_Vec2 vec = tico_field_get_opt_vec2(field, "position", tico_vec2(0, 0));

  tc_Field *el = NULL;
  tico_field_foreach(el, field) {
    // tc_Vec2 aux = pos;
    // if (!strcmp(el->name, "position")) tico_vec2_add(&aux, aux, vec);
    tico_field_draw(el, pos, scale);
  }
}

void tico_field_object_imgui(tc_Field *field) {
  // if (igTree)
  if (!field) return;

  char key[512];
  tico_field_get_uid(field, key);

  // const char *n_key = strstr(key, "##");

  float frame_height = igGetFrameHeight();

  tc_Field *el = NULL;
  igBeginGroup();
  if (igCollapsingHeaderTreeNodeFlags(key, 0)) {

    tico_field_foreach(el, field) {
      tico_field_get_uid(el, key);
      int tree_open = igTreeNodeExStr(key, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed);
      igBeginGroup();
    // igSameLine(0, 0);
    // igButton("+", (ImVec2){ 16, 14});
      ImVec2 size;
      igGetContentRegionAvail(&size);
      igIndent(8);
      if (tree_open && el->type != TIC_FIELD_TYPE_OBJECT) {
        tico_field_draw_imgui(el);
      }
      igUnindent(8);

      igEndGroup();
      // ImDrawList *list = igGetForegroundDrawListNil();
      // ImVec2 pmin, pmax;
      // igGetItemRectMin(&pmin);
      // igGetItemRectMax(&pmax);
      // pmin.x -= frame_height * 1.2;
      // pmin.y -= frame_height * 1.2;

      // pmax.x = pmin.x + size.x + frame_height*1.3;
      // pmax.y += frame_height;
      if (tree_open) {
        // ImDrawList_AddRect(list, pmin, pmax, 0xffffffff, 4, ImDrawCornerFlags_BotRight | ImDrawCornerFlags_BotLeft, 2);
        // igPushItemWidth(pmax.x);
        // igPushStyleVarVec2(Im, const ImVec2 val)
        igTreePop();
        // igPopItemWidth();
      }

    }
      // igTreePop();
  }
  igEndGroup();
}

/*****************
 * Resource
 *****************/

// tc_Field* tico_field_create_resource(void *resource) {
//   if (!resource) return NULL;
//   tc_Field *field = tico_field_create(TIC_FIELD_TYPE_RESOURCE);
//   if (!field) return NULL;

//   tico_field_set_resource(field, resource);

//   return field;
// }

// tc_Field *tico_field_resource_from_json(cJSON *const json) {
//   if (!json) return NULL;
//   if (json->type != cJSON_String) return NULL;

//   tc_Resource *res = tico_plugin_resources_get_by_key(json->valuestring);
//   if (!res) return NULL;

//   tc_Field *field = tico_field_create_resource(res);

//   return field;
// }

// void *tico_field_to_resource(tc_Field *field) {
//   ASSERT(field != NULL);
//   ASSERT(field->type != TIC_FIELD_TYPE_RESOURCE);

//   return tico_field_to_opt_resource(field, NULL);
// }

// void *tico_field_to_opt_resource(tc_Field *field, void *opt) {
//   if (!field) return opt;
//   if (field->type != TIC_FIELD_TYPE_RESOURCE) return opt;

//   return field->ptr;
// }

// void* tico_field_get_resource(tc_Field *field, const char *name) {
//   ASSERT(field != NULL);
//   return tico_field_get_opt_resource(field, name, NULL);
// }

// void* tico_field_get_opt_resource(tc_Field *field, const char *name, void *opt) {
//   if (!field) return opt;
//   tc_Field *item = tico_field_get(field, name);
//   return tico_field_to_opt_resource(item, opt);
// }

// void tico_field_set_resource(tc_Field *field, void *resource) {
//   if (!field) return;
//   if (!resource) return;
//   tc_Resource *res = resource;

//   int sz = strlen(res->type) + strlen(res->name) + 2;
//   char key[sz];
//   sprintf(key, "%s//%s", res->type, res->name);

//   strcpy(field->string, key);
//   field->ptr = res;
// }

// tc_Field* tico_field_add_resource(tc_Field *field, const char *name, void *resource) {
//   if (!field) return NULL;
//   tc_Field *res = tico_field_get(field, name);
//   if (res) return res;

//   tc_Field *item = tico_field_create_resource(resource);
//   return tico_field_set(field, name, item);
// }

/*****************
 * String
 *****************/

/*****************
 * String
 *****************/
#ifndef TICO_ENGINE_H
#define TICO_ENGINE_H

#include "tico.h"
#include "json.h"

typedef struct tc_Tileset {
  tc_Image *image;
  tc_Vec2 tilesize;
  int tilecount;
  int columns;
  int *bitmasks;
  tc_Rectf *rect;
} tc_Tileset;

typedef struct tc_Tilemap {
  tc_Tileset *tileset;
  tc_Vec2 grid;
  int width;
  int height;
  int count;
  int *data;
} tc_Tilemap;

typedef struct tc_Camera {
  tc_Rectf area;
  tc_Vec2 size;
  tc_Vec2 offset;
  float angle;
} tc_Camera;

// typedef map_t()

typedef struct tc_Animation {
  int length;
  // int *frames;
  int from;
  int to;
} tc_Animation;

typedef map_t(tc_Animation) map_anim_t;

typedef struct tc_Sprite {
  char animation[64];
  int frame;
  float time;
  float speed;
  tc_Image *image;
  int width;
  int height;
  int count;
  map_anim_t animations;
} tc_Sprite;

typedef struct tc_Field tc_Field;
// struct list_field_t;

// typedef enum {
//   TIC_FIELD_UNKNOWN = 0,
//   TIC_FIELD_NUMBER,
//   TIC_FIELD_STRING,
//   TIC_FIELD_BOOLEAN,
//   TIC_FIELD_VEC2,
//   TIC_FIELD_VEC3,
//   TIC_FIELD_VEC4,
//   TIC_FIELD_RECT,
//   TIC_FIELD_CIRCLE,
//   TIC_FIELD_COLOR,
//   TIC_FIELD_RESOURCE,
//   TIC_FIELD_ARRAY,
//   TIC_FIELD_OBJECT
// } TIC_FIELD_TYPE_;

// struct tc_Field {
//   TIC_FIELD_TYPE_ type;
//   char name[128];

//   float number;
//   char string[256];
//   int boolean;
//   tc_Color color;
//   tc_Vec2 vec2;
//   tc_Vec3 vec3;
//   union {
//     tc_Vec4 vec4;
//     tc_Rectf rect;
//   };
//   tc_Circlef circle;

//   tc_Field *prev;
//   tc_Field *next;
//   tc_Field *parent;
//   tc_Field *child;

//   cJSON *data;
// };

// typedef list_t(tc_Field) list_field_t;

// list_t()

// struct {
//   int eita;
// } ok;


// typedef struct tc_Template {
//   cJSON *data;
//   // tc_Field field;
//   list_field_t fields;
//   char uuid[8];
//   tc_Field *root;
// } tc_Template;

/**********************
 * Serialization
 **********************/

// #define FIELD_FUNCTION(func_name, ...) \
// func_name (tc_Field *field, __VA_ARGS__)

// #define FIELD_TYPE(name_p, type_t) \
// TIC_API void tico_template_insert_##name_p(tc_Template *template, const char *name, type_t value); \
// TIC_API void tico_template_remove_##name_p(tc_Template *template, const char *name); \
// TIC_API tc_Field* tico_field_create_##name_p(const char *name, type_t value); \
// TIC_API void tico_field_push_##name_p(tc_Field *field, const char *name, type_t value); \
// TIC_API void tico_field_set_##name_p(tc_Field *field, const char* name , type_t value); \
// TIC_API type_t tico_field_get_##name_p(tc_Field *field); \
// TIC_API void tico_field_imgui_##name_p(tc_Field *field); \
// TIC_API void tico_field_draw_##name_p(tc_Field *field, tc_Vec2 pos, float scale); \
// TIC_API cJSON* tico_field_##name_p##_to_json(tc_Field *field, int data_only);
// #define FIELD_TYPE(name_p, type_p) \
// TIC_API void FIELD_FUNCTION(tico_field_add_##name_p, const char* name, type_p type); \
// TIC_API void FIELD_FUNCTION(tico_field_set_##name_p, const char* name, type_p type); \
// TIC_API type_p FIELD_FUNCTION(tico_field_get_##name_p) \


// TIC_API tc_Template tico_template_create();
// TIC_API void tico_template_init(tc_Template *temp, cJSON *json);
// TIC_API void tico_template_destroy(tc_Template *temp);
// TIC_API int tico_template_insert_json(tc_Template *temp, cJSON *item);
// TIC_API void tico_template_remove(tc_Template *temp, const char *name, int index);

// TIC_API void tico_template_imgui(tc_Template *temp);
// TIC_API void tico_template_draw(tc_Template *temp, tc_Vec2 pos, float scale);
// // TIC_API void tico_template_insert_number(tc_V)
// // TIC_API void tico_template_insert_vec2(tc_Template *temp, const char *name, tc_Vec2 vec);
// TIC_API cJSON* tico_template_to_json(tc_Template *temp, int data_only);
// TIC_API cJSON* tico_field_to_json(tc_Field *field, int data_only);

// TIC_API void tico_field_push(tc_Field *field, tc_Field *new_field);

// TIC_API tc_Field* tico_field_create(int type, const char *name);
// TIC_API tc_Field* tico_field_from_json(cJSON *data);
// TIC_API void tico_field_clear(tc_Field *field);
// TIC_API void tico_field_remove(tc_Field *field, const char *name, int index);
// TIC_API void tico_field_add_child(tc_Field *field, const char *name);
// TIC_API void tico_field_remove_child(tc_Field *field, const char *name, int index);
// TIC_API tc_Field* tico_field_get(tc_Field *field, const char *name, int index);
// // TIC_API void tico_field_
// // TIC_API void tico_field_destroy(tc_Field *field);

// TIC_API void tico_field_set_name(tc_Field *field, const char *name);
// TIC_API const char* tico_field_get_name(tc_Field *field);
// TIC_API void tico_field_get_type(tc_Field field);
// TIC_API const char* tico_field_get_type_name(tc_Field field);

// FIELD_TYPE(number, float);
// FIELD_TYPE(string, const char*);
// FIELD_TYPE(boolean, int);
// FIELD_TYPE(vec2, tc_Vec2);
// FIELD_TYPE(vec3, tc_Vec3);
// FIELD_TYPE(vec4, tc_Vec4);
// FIELD_TYPE(rect, tc_Rectf);
// FIELD_TYPE(circle, tc_Circlef);
// FIELD_TYPE(color, tc_Color);
// FIELD_TYPE(resource, tc_Resource);

// TIC_API void tico_field_add_vec2(tc_Field *field, const char *name, tc_Vec2 vec);
// TIC_API void tico_field_set_vec2(tc_Field *field, const char *name, tc_Vec2 vec);
// TIC_API tc_Vec2 tico_field_get_vec2(tc_Field field);

// TIC_API void tico_field_add_vec3(tc_Field *field, const char *name, tc_Vec3 vec);
// TIC_API void tico_field_set_vec3(tc_Field *field, const char *name, tc_Vec3 vec);
// TIC_API tc_Vec3 tico_field_get_vec3(tc_Field field);

// TIC_API void tico_field_add_vec4(tc_Field *field, const char *name, tc_Vec4 vec);
// TIC_API void tico_field_set_vec4(tc_Field *field, const char *name, tc_Vec4 vec);
// TIC_API tc_Vec4 tico_field_get_vec4(tc_Field field);

/**********************
 * Tileset
 **********************/

TIC_API tc_Tileset tico_tileset_create(tc_Image *image, int w, int h);
TIC_API tc_Tileset tico_tileset_load(const char *path, tc_Image *image);
TIC_API int tico_tileset_from_json(tc_Tileset *tileset, cJSON *json);
TIC_API void tico_tileset_destroy(tc_Tileset *tileset);

TIC_API void tico_tileset_set_bitmask(tc_Tileset *tileset, int index, int bitmask);

TIC_API void tico_tileset_calc_mask_array(tc_Tileset tileset, int bitmask, int *bitmask_array);
TIC_API int tico_tileset_calc_mask(tc_Tileset tileset, int *bitmask_array);

TIC_API void tico_tileset_draw(tc_Tileset tileset, int index, int x, int y, tc_Color color);

TIC_API tc_Rectf tico_tileset_get_rect(tc_Tileset tileset, int index);
  //TIC_API tc_Rectf tico_tileset_get_rect_from_bitmask(tc_Tileset tileset, int bitmask);
TIC_API int tico_tileset_get_from_bitmask(tc_Tileset tileset, int bitmask);

/**********************
 * Tilemap
 **********************/

TIC_API tc_Tilemap tico_tilemap_create(tc_Tileset *tileset, int w, int h);
TIC_API tc_Tilemap tico_tilemap_load(const char *path, tc_Tileset *tileset);
TIC_API int tico_tilemap_from_json(tc_Tilemap *tilemap, cJSON *json);
TIC_API void tico_tilemap_resize(tc_Tilemap *tilemap, int w, int h);
TIC_API void tico_tilemap_destroy(tc_Tilemap *map);

TIC_API int tico_tilemap_has_tile(tc_Tilemap map, int x, int y);

TIC_API void tico_tilemap_draw(tc_Tilemap map);
TIC_API void tico_tilemap_draw_part(tc_Tilemap map, tc_Rect part, int x, int y);

TIC_API void tico_tilemap_update(tc_Tilemap *map);
TIC_API int tico_tilemap_autotile(tc_Tilemap tilemap, int index);
TIC_API int tico_tilemap_autotile_pos(tc_Tilemap tilemap, int x, int y);

TIC_API void tico_tilemap_insert(tc_Tilemap *map, int index);
TIC_API void tico_tilemap_remove(tc_Tilemap *map, int index);
TIC_API void tico_tilemap_insert_pos(tc_Tilemap *map, int index);

TIC_API int tico_tilemap_get_index(tc_Tilemap map, int x, int y);
TIC_API void tico_tilemap_get_position(tc_Tilemap map, int index, tc_Vec2 *v);

/***********************
 * Camera
 ***********************/

TIC_API tc_Camera tico_camera_create(float x, float y, float w, float h);
TIC_API void tico_camera_destroy(tc_Camera *camera);

TIC_API void tico_camera_attach(tc_Camera camera);
TIC_API void tico_camera_detach();

TIC_API void tico_camera_set_pos(tc_Camera *camera, tc_Vec2 pos);
TIC_API void tico_camera_set_scale(tc_Camera *camera, tc_Vec2 scale);
TIC_API void tico_camera_set_size(tc_Camera *camera, tc_Vec2 size);

TIC_API void tico_camera_get_pos(tc_Camera camera, tc_Vec2 *pos);
TIC_API void tico_camera_get_scale(tc_Camera camera, tc_Vec2 *scale);
TIC_API void tico_camera_get_size(tc_Camera camera, tc_Vec2 *size);

/***********************
 * Sprite
 ***********************/

TIC_API tc_Sprite tico_sprite_create(tc_Image *image, int w, int h);
TIC_API int tico_sprite_from_json(tc_Sprite *sprite, cJSON *json);
TIC_API void tico_sprite_destroy(tc_Sprite *sprite);

TIC_API void tico_sprite_update(tc_Sprite *sprite);

TIC_API tc_Animation tico_animation_create(int from, int to);

TIC_API void tico_sprite_add_animation(tc_Sprite *sprite, const char *name, int from, int to);
TIC_API void tico_sprite_play(tc_Sprite *sprite, const char *name);
TIC_API void tico_sprite_stop(tc_Sprite *sprite);
TIC_API const char *tico_sprite_get_animation(tc_Sprite *sprite);

TIC_API void tico_sprite_draw_frame(tc_Sprite sprite, int frame, float x, float y, tc_Color color);
TIC_API void tico_sprite_draw_frame_scale(tc_Sprite sprite, int frame, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_sprite_draw_frame_ex(tc_Sprite sprite, int frame, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tico_sprite_draw(tc_Sprite sprite, float x, float y, tc_Color color);
TIC_API void tico_sprite_draw_scale(tc_Sprite sprite, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tico_sprite_draw_ex(tc_Sprite sprite, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

#endif
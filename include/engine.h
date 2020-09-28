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

/**********************
 * Tileset
 **********************/

TIC_API tc_Tileset tico_tileset_create(tc_Image *image, int w, int h);
TIC_API tc_Tileset tico_tileset_load(const char *path, tc_Image *image);
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
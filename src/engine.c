#include "tico.h"

tc_Tileset tico_tileset_create(tc_Image *image, int w, int h) {
  tc_Tileset tileset = {0};
  tileset.tilesize = tico_vec2(w, h);
  tileset.image = image;
  int sx = tileset.image->width / w;
  int sy = tileset.image->height / h;
  tileset.tilecount = sx * sy;

  tileset.columns = sx;
  tileset.bitmasks = malloc(sizeof(int) * tileset.tilecount);
  memset(tileset.bitmasks, -1, sizeof(int) * tileset.tilecount);

  return tileset;
}

tc_Tileset tico_tileset_load(const char *path, tc_Image *image) {
  tc_Tileset tileset = {0};

  cJSON *root = tico_json_open(path);
  // const char *filename = tico_json_get_string(root, "image");
  int tilew = tico_json_get_number(root, "tilewidth");
  int tileh = tico_json_get_number(root, "tileheight");
  // const char *image_name = tico_json_get_string(root, "image");
  tc_Resource *resource = NULL;
  // TRACELOG("%s", image_name);
  // if (image_name) resource = tico_resources_get(&Core.resources, "image", image_name);
  // TRACELOG("%p", resource);
  // if (!resource) tileset = tico_tileset_create(image, tilew, tileh);
  tileset = tico_tileset_create(image, tilew, tileh);
  cJSON *bitmask = tico_json_get_array(root, "bitmasks");
  cJSON *el;
  int i = 0;
  cJSON_ArrayForEach(el, bitmask) {
    tileset.bitmasks[i] = el->valueint;
    // TRACELOG("%d %d", i, el->valueint);
    i++;
  }

  tico_json_delete(root);

  return tileset;
}

void tico_tileset_destroy(tc_Tileset *tileset) {
  free(tileset->bitmasks);
}

void tico_tileset_draw(tc_Tileset tileset, int index, int x, int y, tc_Color color) {
  int xx = fmod(index, tileset.columns) * tileset.tilesize.x;
  int yy = floor(index / tileset.columns) * tileset.tilesize.y;

  tc_Rectf rect = tico_rectf(xx, yy, tileset.tilesize.x, tileset.tilesize.y);

  tico_image_draw_part(*tileset.image, rect, x, y, color);
}

void tico_tileset_calc_mask_array(tc_Tileset tileset, int bitmask, int *bitmask_array) {
  int off = 0;
  int val = -1;

  if (bitmask > -1) bitmask_array[4] = 1;
  else {
    bitmask_array[4] = -1;
    bitmask = 0;
  }

  for (int i = 0; i < 9; i++) {
    if (i == 4) i++;
    bitmask_array[i] = bitmask & 0x1;
    bitmask >>= 1;
  }
}

int tico_tileset_calc_mask(tc_Tileset tileset, int *bitmask_array) {
  int off = 0;
  int val = -1;
  if (bitmask_array[4] > 0) val = 0;

  for (int i = 0; i < 9; i++) {
    if (i == 4) {
      off = 1;
      continue;
    }
    if (bitmask_array[i] > 0) {
      if (val == -1) val = 0;
      val += 1 << (i-off);
    }
  }

  return val;
}

int tico_tileset_get_from_bitmask(tc_Tileset tileset, int bitmask) {
  int bm;
  for (int i = 0; i < tileset.tilecount; i++) {
    bm = tileset.bitmasks[i];
    // TRACELOG("%d %d", i, bm);
    if (bm == bitmask) {
      return i;
    }
  }
  return 0;
}

tc_Rectf tico_tileset_get_rect(tc_Tileset tileset, int index) {
  tc_Rectf rect = tico_rectf(0, 0, tileset.tilesize.x, tileset.tilesize.y);
  if (index < 0 && index >= tileset.tilecount) return rect;

  rect.x = fmod(index, tileset.columns) * tileset.tilesize.x;
  rect.y = floor(index / tileset.columns) * tileset.tilesize.y;

  return rect;
}

/***********************
 * Tilemap
 ***********************/

tc_Tilemap tico_tilemap_create(tc_Tileset *tileset, int w, int h) {
  tc_Tilemap map = {0};
  if (w <= 0) w = 1;
  if (h <= 0) h = 1;
  int data_size = sizeof(int) * w * h;
  map.data = malloc(data_size);
  memset(map.data, -1, data_size);

  map.width = w;
  map.height = h;
  map.tileset = tileset;
  map.grid.x = map.tileset->tilesize.x;
  map.grid.y = map.tileset->tilesize.y;
  map.count = w * h;

  return map;
}

tc_Tilemap tico_tilemap_load(const char *path, tc_Tileset *tileset) {
  tc_Tilemap map = {0};

  cJSON *root = tico_json_open(path);
  // const char *filename = tico_json_get_string(root, "image");
  int width = tico_json_get_number(root, "width");
  int height = tico_json_get_number(root, "height");
  map = tico_tilemap_create(tileset, width, height);

  cJSON *data = tico_json_get_array(root, "data");
  cJSON *el;
  int i = 0;
  cJSON_ArrayForEach(el, data) {
    map.data[i] = el->valueint;
    // TRACELOG("%d %d", i, el->valueint);
    i++;
  }

  tico_json_delete(root);

  return map;
}

void tico_tilemap_resize(tc_Tilemap *tilemap, int w, int h) {
  if (w <= 0 || h <= 0) return;
  int count = tilemap->count;
  int old_data[count];
  memcpy(old_data, tilemap->data, sizeof(int) * count);

  int n_count = w*h;
  int min_count = tico_min(count, n_count);
  int min_width = tico_min(w, tilemap->width);

  tilemap->data = TIC_REALLOC(tilemap->data, sizeof(int) * n_count);
  memset(tilemap->data, -1, sizeof(int) * n_count);
  for (int i = 0; i < min_count; i++) {
    int ox = fmod(i, min_width);
    int oy = floor(i / min_width) * tilemap->width;
    int ny = floor(i / min_width) * w;

    int oindex = ox + oy;
    int nindex = ox + ny;
    if (oindex >= count || nindex >= n_count) break;
    tilemap->data[nindex] = old_data[oindex];
  }

  tilemap->width = w;
  tilemap->height = h;
  tilemap->count = n_count;
}


void tico_tilemap_destroy(tc_Tilemap *tilemap) {
  free(tilemap->data);
}

void tico_tilemap_draw(tc_Tilemap tilemap) {
  for (int i = 0; i < tilemap.count; i++) {
    int x = fmod(i, tilemap.width) * tilemap.tileset->tilesize.x;
    int y = floor(i / tilemap.width) * tilemap.tileset->tilesize.y;
    tico_tileset_draw(*tilemap.tileset, tilemap.data[i], x, y, WHITE);
  }
}

int tico_tilemap_has_tile(tc_Tilemap map, int x, int y) {
  int index = tico_tilemap_get_index(map, x, y);
  if (index < 0) return 0; 
  return map.data[index] >= 0;
}

void tico_tilemap_update(tc_Tilemap *map) {
  for (int i = 0; i < map->count; i++) {
    int tile = tico_tilemap_autotile(*map, i);
    if (map->data[i] >= 0) map->data[i] = tile;
  }
}

int tico_tilemap_autotile(tc_Tilemap map, int index) {
  if (index < 0 || index > (map.width*map.height)) return -1;
  // if (map.data[index] < 0) return -1;

  tc_Vec2 pos;
  tico_tilemap_get_position(map, index, &pos);
  int sides[4] = {0, 0, 0, 0};
  int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  mask[4] = 1;

  sides[0] = tico_tilemap_has_tile(map, pos.x, pos.y - 1);
  sides[1] = tico_tilemap_has_tile(map, pos.x + 1, pos.y);
  sides[2] = tico_tilemap_has_tile(map, pos.x, pos.y + 1);
  sides[3] = tico_tilemap_has_tile(map, pos.x - 1, pos.y);

  mask[1] = sides[0];
  mask[5] = sides[1];
  mask[7] = sides[2];
  mask[3] = sides[3];

  mask[0] = tico_tilemap_has_tile(map, pos.x-1, pos.y-1) && sides[0] && sides[3];
  mask[2] = tico_tilemap_has_tile(map, pos.x+1, pos.y-1) && sides[0] && sides[1];
  mask[6] = tico_tilemap_has_tile(map, pos.x-1, pos.y+1) && sides[2] && sides[3];
  mask[8] = tico_tilemap_has_tile(map, pos.x+1, pos.y+1) && sides[2] && sides[1];

  int bm = tico_tileset_calc_mask(*map.tileset, mask);
  // TRACELOG("%d %d", index, bm);
  return tico_tileset_get_from_bitmask(*map.tileset, bm);
}

void tico_tilemap_insert(tc_Tilemap *map, int index) {
  if (index < 0 || index >= map->width*map->height) return;
  map->data[index] = 1;
  map->data[index] = tico_tilemap_autotile(*map, index);
  // TRACELOG("%d", index);
  // TRACELOG("%d", map->data[index]);

  tc_Vec2 pos;
  tico_tilemap_get_position(*map, index, &pos);
  // TRACELOG("%d %d", index, map->data[index]);

  tc_Vec2 ad[8] = {
    { 0, 1},
    { 0,-1},
    { 1, 0},
    {-1, 0},
    {-1,-1},
    { 1,-1},
    {-1, 1},
    { 1, 1}
  };

  for (int i = 0; i < 8; i++) {
    int n_index = tico_tilemap_get_index(*map, pos.x+ad[i].x, pos.y+ad[i].y);
    // TRACELOG("%d index: %d", i, n_index);
    if (n_index >= 0 && map->data[n_index] >= 0) map->data[n_index] = tico_tilemap_autotile(*map, n_index);
  }
}

void tico_tilemap_remove(tc_Tilemap *map, int index) {
  if (index < 0 || index >= map->width*map->height) return;
  map->data[index] = -1;
  // TRACELOG("%d", map->data[index]);

  tc_Vec2 pos;
  tico_tilemap_get_position(*map, index, &pos);
  // TRACELOG("%d %d", index, map->data[index]);

  tc_Vec2 ad[8] = {
    { 0, 1},
    { 0,-1},
    { 1, 0},
    {-1, 0},
    {-1,-1},
    { 1,-1},
    {-1, 1},
    { 1, 1}
  };

  for (int i = 0; i < 8; i++) {
    int n_index = tico_tilemap_get_index(*map, pos.x+ad[i].x, pos.y+ad[i].y);
    // TRACELOG("%d index: %d", i, n_index);
    if (n_index >= 0 && map->data[n_index] >= 0) map->data[n_index] = tico_tilemap_autotile(*map, n_index);
  }
}

void tico_tilemap_get_position(tc_Tilemap map, int index, tc_Vec2 *pos) {
  int x = fmod(index, map.width);
  int y = floor(index / map.width);
  pos->x = x;
  pos->y = y;
}

int tico_tilemap_get_index(tc_Tilemap map, int x, int y) {
  int index = 0;
  if (x >= 0 && x < map.width &&
      y >= 0 && y < map.height) {
    index = x + (y * map.width);
  }

  return index;
}

/**************************
 * Camera
 **************************/

tc_Camera tico_camera_create(float x, float y, float w, float h) {
  tc_Camera camera = {0};
  camera.area = tico_rectf(x, y, w, h);
  camera.size = (tc_Vec2){1, 1};
  camera.offset = (tc_Vec2){0, 0};
  camera.angle = 0;

  return camera;
}

void tico_camera_attach(tc_Camera camera) {
  tico_graphics_push();
  tico_graphics_translate(camera.offset.x, camera.offset.y);
  tico_graphics_scale(camera.size.x, camera.size.y);
  tico_graphics_rotate(camera.angle);
  tico_graphics_translate(camera.area.x, camera.area.y);
  // TRACELOG("%f %f", camera.area.x, camera.area.y);
  tico_graphics_scissor(0, 0, camera.area.w, camera.area.h);
}

void tico_camera_detach() {
  tico_graphics_pop();
  // tc_Canvas canvas = Core.render.state.currentCanvas;
  tc_Canvas canvas = tico_graphics_top_canvas();
  // if (canvas.id != 0) tico_graphics_scissor(0, 0, canvas.width, canvas.height);
  // else tico_graphics_scissor(0, 0, Core.window.width, Core.window.height);
  tico_graphics_scissor(0, 0, Core.window.width, Core.window.height);
}

void tico_camera_get_pos(tc_Camera camera, tc_Vec2 *pos) {
  ASSERT(pos != NULL);
  pos->x = camera.area.x;
  pos->y = camera.area.y;
}

void tico_camera_set_pos(tc_Camera *camera, tc_Vec2 pos) {
  ASSERT(camera != NULL);
  camera->area.x = pos.x;
  camera->area.y = pos.y;
}

// void tico_camera_set_scale(tc_Camera *camera, float x, float y) {}

// void tico_camera_set_size(tc_Camera *camera, float w, float h) {}

/******************
 * Sprite
 ******************/

tc_Animation tico_animation_create(int from, int to) {
  tc_Animation anim = {0};

  int first = from < to ? from : to;
  int last = from < to ? to : from;


  anim.length = abs(from - to);
  // anim.frames = malloc(sizeof(int) * anim.length);
  // for (int i = 0; i <= anim.length; i++) {
  //  anim.frames[i] = first + i;
  // }
  anim.from = first;
  anim.to = last;

  return anim;
}

tc_Sprite tico_sprite_create(tc_Image *image, int w, int h) {
  tc_Sprite spr = {0};
  spr.frame = 0;
  map_init(&spr.animations);
  spr.animation[0] = '\0';
  spr.width = w;
  spr.height = h;
  spr.image = image;
  spr.time = 0;
  spr.speed = 8;

  return spr;
}

int tico_sprite_from_json(tc_Sprite *sprite, cJSON *json) {
  int width = tico_json_get_number(json, "width");
  int height = tico_json_get_number(json, "height");
  map_init(&sprite->animations);
  sprite->frame = 0;
  sprite->animation[0] = '\0';
  sprite->width = width;
  sprite->height = height;
  sprite->time = 0;
  sprite->speed = 8;

  // TRACELOG("%d %d", width, height);

  cJSON *anims = tico_json_get_array(json, "animations");
  cJSON *el = NULL;
  cJSON_ArrayForEach(el, anims) {
    char *name = tico_json_get_string(el, "name");
    int from = tico_json_get_number(el, "from");
    int to = tico_json_get_number(el, "to");
    // TRACELOG("%s %d %d", name, from, to);
    tc_Animation *anim = map_get(&sprite->animations, name);
    if (!anim) {
      tc_Animation new_anim = tico_animation_create(from, to);
      map_set(&sprite->animations, name, new_anim);
    } else {
      anim->from = from;
      anim->to = to;
    }
  }

  cJSON *deps = tico_json_get_object(json, "dependencies");
  const char *key = NULL;
  tc_Resource *res = NULL;
  if (deps) key = tico_json_get_string(deps, "image");
  // if (key) res = map_get(&Core.resources.resources, key);
  if (key) {
    // char **uuid = map_get(&Core.resources.keys, key);
    // res = map_get(&Core.resources.resources, *uuid);
    res = tico_plugin_resources_get_by_key(key);
  }

  if (res) sprite->image = res->data;

  // TRACELOG("%d", sprite->image->width);
}

void tico_sprite_destroy(tc_Sprite *sprite) {
  map_deinit(&sprite->animations);
}

void tico_sprite_add_animation(tc_Sprite *sprite, const char *name, int from, int to) {
  tc_Animation anim = tico_animation_create(from, to);
  map_set(&sprite->animations, name, anim);
}

void tico_sprite_play(tc_Sprite *sprite, const char *name) {
  tc_Animation *anim = map_get(&sprite->animations, name);
  if (anim) strcpy(sprite->animation, name);
}

void tico_sprite_stop(tc_Sprite *sprite) {
  sprite->animation[0] = '\0';
}

const char *tico_sprite_get_animation(tc_Sprite *sprite) {
  return sprite->animation;
}


tc_Rectf tico_sprite_get_rect(tc_Sprite sprite, int frame) {
  // int w = sprite.width;
  // int h = sprite.height;
  // int iw = sprite.image->width;
  // int ih = sprite.image->height;
  float ww = (float)sprite.image->width / (float)sprite.width;
  if (frame < 0) frame = 0;

  tc_Rectf rect;
  rect.x = fmodf(frame, ww) * sprite.width;
  rect.y = floor(frame / ww) * sprite.height;
  rect.w = sprite.width;
  rect.h = sprite.height;

  return rect;
}

int tico_sprite_get_current_frame(tc_Sprite sprite) {
  tc_Animation *anim = map_get(&sprite.animations, sprite.animation);
  if (!anim) return -1;
  if (anim->from + sprite.frame > anim->to) return -1;

  return anim->from + sprite.frame;
}

void tico_sprite_update(tc_Sprite *sprite) {
  if (!sprite) return;
  const char *current = sprite->animation;
  float delta = tico_timer_get_delta();
  tc_Animation *anim = map_get(&sprite->animations, current);
  if (!anim) return;
  sprite->time += sprite->speed * delta;
  if (sprite->time >= 1) {
    sprite->frame++;
    sprite->time = 0;
  }

  if (anim->from + sprite->frame > anim->to) sprite->frame = 0;
}

void tico_sprite_draw_frame(tc_Sprite sprite, int frame, float x, float y, tc_Color color) {
  tico_sprite_draw_frame_scale(sprite, frame, x, y, 1, 1, color);
}
void tico_sprite_draw_frame_scale(tc_Sprite sprite, int frame, float x, float y, float sx, float sy, tc_Color color) {
  if (frame < 0) return;
  tc_Rectf rect = tico_sprite_get_rect(sprite, frame);

  // TRACELOG("%p", sprite.image);
  tico_image_draw_part_scale(*sprite.image, rect, x, y, sx, sy, color);
}

void tico_sprite_draw_frame_ex(tc_Sprite sprite, int frame, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  if (frame < 0) return;

  // TRACELOG("%f %f %f", angle, sx, sy);
  tc_Rectf rect = tico_sprite_get_rect(sprite, frame);
  tico_image_draw_part_ex(*sprite.image, rect, x, y, angle, sx, sy, cx, cy, color);
}

void tico_sprite_draw(tc_Sprite sprite, float x, float y, tc_Color color) {
  tico_sprite_draw_scale(sprite, x, y, 1, 1, color);
}

void tico_sprite_draw_scale(tc_Sprite sprite, float x, float y, float sx, float sy, tc_Color color) {
  int frame = tico_sprite_get_current_frame(sprite);
  tico_sprite_draw_frame_scale(sprite, frame, x, y, sx, sy, color);
}

void tico_sprite_draw_ex(tc_Sprite sprite, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color) {
  int frame = tico_sprite_get_current_frame(sprite);
  tico_sprite_draw_frame_ex(sprite, frame, x, y, angle, sx, sy, cx, cy, color);
}
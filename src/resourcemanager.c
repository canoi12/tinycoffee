#include "tico.h"

void tic_resource_init(tc_Resource *resource, TIC_RESOURCE_TYPE type, int poolSize) {
  size_t dataSize;
  if (type == TIC_RESOURCE_TEXTURE) dataSize = sizeof(tc_Texture*);
  else if (type == TIC_RESOURCE_SOUND) dataSize = sizeof(tc_AudioData*);
  else if (type == TIC_RESOURCE_FONT) dataSize = sizeof(tc_Sound*);
  else return;

  resource->pool = calloc(dataSize, poolSize);
  for (int i = 0; i < poolSize; i++) {
    resource->pool[i] = NULL;
  }
  resource->maxSize = poolSize;
  resource->map = hashmap_create(poolSize);
  resource->type = type;
}

void tic_resource_destroy(tc_Resource *resource) {
  void (*destroy_fn)(void *data) = NULL;
  if (resource->type == TIC_RESOURCE_TEXTURE) destroy_fn = tic_texture_destroy;
  else if (resource->type == TIC_RESOURCE_FONT) destroy_fn = tic_font_destroy;
  else if (resource->type == TIC_RESOURCE_SOUND) destroy_fn = tic_audio_data_free;
  else return;

  for (int i = 0; i < resource->maxSize; i++) {
    hashmap_item *item = resource->map.map[i];
    while (item) {
      destroy_fn(resource->pool[item->value]);
      TRACELOG("destroying %s %d %p", item->key, item->value, resource->pool[item->value]);
      hashmap_item *prev = item;
      item = item->next;
      free(prev->key);
      free(prev);
    }
  }
  free(resource->pool);
}

tc_bool tic_resource_add(tc_Resource *resource, const char *name, void *data) {
  if (tic_resource_get(resource, name)) return tc_false;
  if (!data) return tc_false;
  int i;
  tc_bool found = tc_false;
  TRACELOG("%s", name);
  for (i = 0; i < resource->maxSize; i++) {
    if (resource->pool[i] == NULL) {
      found = tc_true;
      break;
    }
  }

  if (!found) {
    TRACEERR("%s pool is full", resource->type == TIC_RESOURCE_TEXTURE ? "Texture" : resource->type == TIC_RESOURCE_FONT ? "Font" : "Sound");
    return tc_false;
  }
  resource->pool[i] = data;
  hashmap_set(&resource->map, name, i);
  return tc_true;
}

void* tic_resource_get(tc_Resource *resource, const char *name) {
  hashmap_item *item = hashmap_get(resource->map, name);
  if (item) {
    void *data = resource->pool[item->value];
    return data;
  }
  return NULL;
}
tc_bool tic_resource_is_full(tc_Resource *resource);

tc_bool tic_resources_init(tc_ResourceManager *resources) {
  tic_resource_init(&resources->textures, TIC_RESOURCE_TEXTURE, DEFAULT_MAX_TEXTURES);
  tic_resource_init(&resources->fonts, TIC_RESOURCE_FONT, DEFAULT_MAX_FONTS);
  tic_resource_init(&resources->sounds, TIC_RESOURCE_SOUND, DEFAULT_MAX_SOUNDS);
}

tc_bool tic_resources_destroy(tc_ResourceManager *resources) {
  tic_resource_destroy(&resources->textures);
  tic_resource_destroy(&resources->fonts);
  tic_resource_destroy(&resources->sounds);
}

tc_bool tic_resources_add_texture(const char *name, tc_Texture *image) {
  return tic_resource_add(&Core.resources.textures, name, image);
}

tc_Texture* tic_resources_get_texture(const char *name) {
  return tic_resource_get(&Core.resources.textures, name);
}

tc_bool tic_resources_add_sound(const char *name, tc_AudioData *sound) {
  return tic_resource_add(&Core.resources.sounds, name, sound);
}

tc_AudioData *tic_resources_get_sound(const char *name) {
  return tic_resource_get(&Core.resources.sounds, name);
}

tc_bool tic_resources_add_font(const char *name, tc_Font* sound) {
  return tic_resource_add(&Core.resources.fonts, name, sound);
}

tc_Font* tic_resources_get_font(const char *name) {
  return tic_resource_get(&Core.resources.fonts, name);
}
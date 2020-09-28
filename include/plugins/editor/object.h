#ifndef TICO_PLUGIN_EDITOR_OBJECT_H
#define TICO_PLUGIN_EDITOR_OBJECT_H

typedef struct tc_Object {
  char type[128];
  tc_Vec2 position;
  tc_Vec2 size;
  float angle;
  tc_Texture tex;
  tc_Rectf tex_uv;
  tc_Color color;
} tc_Object;

typedef map_t(tc_Object) map_object_t;

typedef struct tc_ObjectWidget {
  char *selected;
  map_object_t objects;
} tc_ObjectWidget;

TIC_API tc_Object tico_object_create(const char *type, tc_Vec2 position);
TIC_API tc_Object tico_object_from_json(const cJSON *json);
TIC_API void tico_object_update(tc_Object *object);
TIC_API void tico_object_draw(tc_Object *object);

TIC_API int tico_widget_object_init(tc_ObjectWidget *widget);
TIC_API void tico_widget_object_deinit(tc_ObjectWidget *widget);
TIC_API void tico_widget_object_draw(tc_ObjectWidget *widget);

#endif /* TICO_PLUGIN_EDITOR_OBJECT_H */

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

#endif
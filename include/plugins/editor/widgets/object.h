#ifndef TICO_PLUGIN_WIDGET_OBJECT_H
#define TICO_PLUGIN_WIDGET_OBJECT_H

// typedef struct tc_Object {
//   char uuid[8];
//   char name[128];
//   char type[128];
//   tc_Vec2 position;
//   tc_Vec2 size;
//   float angle;
//   tc_Texture tex;
//   tc_Rectf tex_uv;
//   tc_Color color;
// } tc_Object;

typedef struct tc_Object {
  // cJSON *data;
  // tc_Meta *meta;
  tc_Field *data;
} tc_Object;

typedef map_t(tc_Object) map_object_t;

typedef struct tc_ObjectWidget {
  tc_Object *selected;
  // cJSON *object_data;
  char uuid[8];
  tc_Field *object_data;
  map_object_t objects;
  tc_PanelWidget panel;

  int flags;
  int last_x;
  int last_y;
} tc_ObjectWidget;

TIC_API tc_Object tico_object_create(const char *type, tc_Vec2 position);
TIC_API tc_Object tico_object_from_json(const tc_Field *json);
TIC_API void tico_object_update(tc_Object *object);
TIC_API void tico_object_draw(tc_Object *object, tc_Vec2 origin, float scale, int selected);

TIC_API int tico_object_is_hovered(tc_Object *object, tc_Vec2 origin, float scale);

TIC_API int tico_widget_object_init(tc_ObjectWidget *widget, tc_Field *data);
TIC_API void tico_widget_object_deinit(tc_ObjectWidget *widget);
TIC_API int tico_widget_object_update(tc_ObjectWidget *widget, tc_PanelWidget *panel);
TIC_API int tico_widget_object_draw(tc_ObjectWidget *widget, tc_PanelWidget *panel, int *active);

// TIC_API int tico_widget_object_set_texture(tc_ObjectWidget *)
TIC_API int tico_widget_object_edit(tc_ObjectWidget *widget, tc_Object *object);
TIC_API int tico_widget_object_selected(tc_ObjectWidget *widget, tc_Object *object, tc_Vec2 origin, float scale);
TIC_API int tico_widget_object_resized(tc_ObjectWidget *widget, tc_Object *object);

#endif /* TICO_PLUGIN_EDITOR_OBJECT_H */

#if defined(TICO_PLUGIN_WIDGET_IMPLEMENTATION)

tc_Object tico_object_create(const char *type, tc_Vec2 position) {
  tc_Object object = {0};

  ASSERT(type != NULL);

  // memcpy(&object.position, &position, sizeof(tc_Vec2));
  // object.size = tico_vec2(32, 32);
  // object.angle = 0;
  // object.tex = (tc_Texture){0};
  // object.tex_uv = tico_rectf(0, 0, 1, 1);
  // object.color = WHITE;

  // object.data = tico_json_create_object(NULL);
  // tico_json_add_string(object.data, "type", type);
  // tico_json_add_vec2(object.data, "position", position);
  // tico_json_add_vec2(object.data, "size", tico_vec2(16, 16));

  object.data = tico_field_create_object(NULL);
  tico_field_add_string(object.data, "type", type);
  tico_field_add_vec2(object.data, "position", position);
  tico_field_add_vec2(object.data, "size", tico_vec2(16, 16));

  // object.data = tico_field_create_string("type", type);
  // tico_field_push_vec2(object.data, "position", position);
  // tico_field_push_vec2(object.data, "size", tico_vec2(16, 16));



  return object;
}

void tico_object_init(tc_Object *object, tc_Field *data) {
  ASSERT(object != NULL);
  // tc_Vec2 position = tico_json_get_opt_vec2(json, "position", 0, tico_vec2(0, 0));
  memset(object, 0, sizeof(*object));
  object->data = data;

  const char *type = tico_field_get_string(data, "type");
  if (!type) return;

  tc_Resource *res = tico_plugin_resources_get("object", type);
  if (!res) return;


  cJSON *sprite = cJSON_GetObjectItem(res->data, "sprite");
  if (!sprite) return;

  // TRACELOG("%s %s", res->type, res->name);
  // TRACELOG("%s %s",  sprite->valuestring);

  tc_Field *new_field = tico_field_from_json(sprite, NULL);
  // TRACELOG("%d %s %s", new_field->type, new_field->name, new_field->string);
  tico_field_push(object->data->child, -1, new_field);
  // tico_field_add(data, "sprite", new_field);

  // TRACELOG("%s", data->name);
  // tc_Field *field = data->child;
  // while (field) {

  //   field = field->next;
  // }
}

void tico_object_update(tc_Object *object) {

}

// static void tico_object_draw_sprite(tc_Object *object, const char *field, ImVec2 pos0, ImVec2 pos1, float scale) {
//   ASSERT(object != NULL);

//   const char *type = tico_json_get_string(object->data, "type", 0);
//   ASSERT(type != NULL);
//   // TRACELOG("%s", type);
//   tc_Resource *obj_res = tico_plugin_resources_get("object", type);
//   if (!obj_res) return;

//   cJSON *obj_data = obj_res->data;

//   const char *sprite = tico_json_get_string(obj_data, field, 0);
//   if (!sprite) return;

//   tc_Resource *res = tico_plugin_resources_get_by_key(sprite);
//   if (!res) return;

//   cJSON *sprite_data = res->data;
//   const char *image_name = tico_json_get_string(sprite_data, "image", 0);
//   int w = tico_json_get_number(sprite_data, "width", 0);
//   int h = tico_json_get_number(sprite_data, "height", 0);

//   tc_Resource *img_res = tico_plugin_resources_get_by_key(image_name);
//   if (!img_res) return;

//   tc_Image *image = img_res->data;

//   ImDrawList *list = igGetWindowDrawList();

//   tc_Rectf rect = tico_rectf(0, 0, (float)w/(float)image->width, (float)h/(float)image->height);
//   ImDrawList_AddImage(list, image->texture.id, pos0, pos1, (ImVec2){rect.x, rect.y}, (ImVec2){rect.w, rect.h}, 0xffffffff);
//   // lua_settop()
// }

void tico_object_draw(tc_Object *object, tc_Vec2 origin, float scale, int selected) {
  ASSERT(object != NULL);
  // tc_Vec2 size = tico_json_get_vec2(object->data, "size", 0);
  // tc_Vec2 position = tico_json_get_vec2(object->data, "position", 0);
  tc_Field *field = object->data;
  if (!field) return;

  ImDrawList *list = igGetWindowDrawList();

  // tc_Vec2 scale_size = tico_vec2(size.x*scale, size.y*scale);
  // tc_Vec2 spacing = tico_vec2(2, 2);
  // if (size.x < 0) spacing.x = -2;
  // if (size.y < 0) spacing.y = -2;
  unsigned int colors[2];
  colors[0] = 0x880000dd;
  colors[1] = 0xff0000ee;
  if (!selected) {
    colors[0] = 0x88aaaaaa;
    colors[1] = 0xffcccccc;
  }

  // TRACELOG("QQQQQQQQQQQQQQQQQ MERDAAAAAAAAAAAAAAAAAAAAAAAAAAAAA: %s", object->data->name);

  tc_Field *vec = tico_field_get(object->data, "position");
  tc_Vec2 t_pos = vec->vec2;
  vec = tico_field_get(object->data, "size");
  tc_Vec2 t_size = vec->vec2;

  // TRACELOG("%f %f", t_size.x, t_size.y);

  ImVec2 pos = (ImVec2){(t_pos.x*scale) + origin.x, (t_pos.y*scale) + origin.y};
  ImVec2 pos1 = (ImVec2){pos.x + (t_size.x*scale), pos.y + (t_size.y*scale)};

  tico_field_draw(field->child, (tc_Vec2){pos.x, pos.y}, scale);
  ImDrawList_AddRectFilled(list, pos, pos1, colors[0], 0, 0);
  ImDrawList_AddRect(list, pos, pos1, colors[1], 0, 0, 2);


  // tc_Vec2 origin;
  // tico_widget_panel_origin(&widget->panel, &origin);
  // ImVec2 pos = (ImVec2){(position.x*scale) + origin.x, (position.y*scale) + origin.y};
  // ImVec2 pos1 = (ImVec2){pos.x + (size.x*scale), pos.y + (size.y*scale)};
  // if (!selected && tico_object_is_hovered(object, origin, scale)) {
  //   // ImDrawList_AddRectFilled(list, pos, pos1, 0x88dddddd, 0, 0);
  //   // ImDrawList_AddRect(list, pos, pos1, 0xffeeeeee, 0, 0, 2);
  //   colors[0] = 0x88dddddd;
  //   colors[1] = 0xffeeeeee;
  // } else if (!selected) {
  //   // ImDrawList_AddRectFilled(list, pos, pos1, 0x88aaaaaa, 0, 0);
  //   // ImDrawList_AddRect(list, pos, pos1, 0xffcccccc, 0, 0, 2);
  //   colors[0] = 0x88aaaaaa;
  //   colors[1] = 0xffcccccc;
  // }

  // const char *sprite = tico_json_get_string(object->data);
  // if (sprite) {
  //   tc_Resource *res = tico_plugin_resources_get_by_key(sprite);
  //   if (res) {
  //     tc_Sprite *spr = res->data;
  //     ImDrawList_AddImage()
  //   }
  // }
  // tico_object_draw_sprite(object, "sprite", pos, pos1, scale);
  // ImDrawList_AddRectFilled(list, pos, pos1, colors[0], 0, 0);
  // ImDrawList_AddRect(list, pos, pos1, colors[1], 0, 0, 2);
  // ImVec2 resize_pos;
  // ImVec2 resize_pos1;
  // if (scale_size.x)
  // resize_pos = (ImVec2){pos.x + scale_size.x + spacing.x, pos.y + scale_size.y + spacing.y};
  // resize_pos1 = (ImVec2){resize_pos.x + 8, resize_pos.y + 8};
  // ImDrawList_AddRectFilled(list, resize_pos, resize_pos1, colors[0], 0, 0);
}

int tico_object_is_hovered(tc_Object *object, tc_Vec2 origin, float scale) {
  // tc_Vec2 origin;
  // tico_widget_panel_origin(&widget->panel, &origin);
  // float scale = widget->panel.scale;

  // tc_Vec2 position = tico_json_get_vec2(object->data, "position", 0);
  // tc_Vec2 size = tico_json_get_vec2(object->data, "size", 0);
  tc_Field *vec = tico_field_get(object->data, "position");
  tc_Vec2 position = vec->vec2;
  vec = tico_field_get(object->data, "size");
  tc_Vec2 size = vec->vec2;

  tc_Vec2 m_pos;
  tc_Vec2 o_pos;
  int mx, my;
  tico_input_get_mouse_pos(&mx, &my);
  m_pos = tico_vec2(mx, my);
  o_pos.x = position.x*scale;
  o_pos.y = position.y*scale;

  tico_vec2_add(&o_pos, o_pos, origin);
  // tico_vec2_add(&m_pos, m_pos, origin);


  tc_Rectf obj = tico_rectf(o_pos.x, o_pos.y, size.x*scale, size.y*scale);


  return (m_pos.x >= obj.x && m_pos.x <= obj.x + obj.w && m_pos.y >= obj.y && m_pos.y <= obj.y + obj.h);
}

static int tico_widget_object_insert_new(tc_ObjectWidget *widget, const char *type, tc_Vec2 pos) {
  // if (igSelectableBool("new##new_object", "", 0, (ImVec2){64, 16})) {
      char uuid[8];
      tico_utils_generate_uuid(uuid, 8);
      while(map_get(&widget->objects, uuid)) {
        tico_utils_generate_uuid(uuid, 8);
      }
      // int mx, my;
      // tico_input_get_mouse_pos(&mx, &my);
      // tc_Vec2 obj_pos;
      // obj_pos.x = (mx - origin.x) / scale;
      // obj_pos.y = (my - origin.y) / scale;
      // tc_Object obj = tico_object_create(type, pos);

      // map_set(&widget->objects, uuid, obj);
      // tico_field_push()
      // tico_json_add_object(widget->object_data, uuid, obj.data);
    // }

  return 1;
}


int tico_widget_object_init(tc_ObjectWidget *widget, tc_Field *data) {
  ASSERT(widget != NULL);
  ASSERT(data != NULL);
  memset(widget, 0, sizeof(*widget));
  map_init(&widget->objects);

  widget->object_data = data;
  // widget->

  widget->panel = tico_widget_panel_create(64);
  widget->selected = NULL;
  tico_utils_generate_uuid(widget->uuid, 8);

  // cJSON *obj = NULL;

  // cJSON_ArrayForEach(obj, data) {
  //   tc_Object object = {0};
  //   tico_object_init(&object, obj);
  //   map_set(&widget->objects, obj->string, object);
  // }

  // tc_Field *field = data->child;
  // while (field) {
  //   tc_Object object = {0};
  //   tico_object_init(&object, field);
  //   tc_Field *vec = tico_field_get(field, "size");
  //   map_set(&widget->objects, field->name, object);
  //   field = field->next;
  // }

  tc_Field *el = NULL;
  tico_field_foreach(el, data) {
    tc_Object object = {0};
    tico_object_init(&object, el);
    // tc_Field *vec = tico_field_get(field, "size");
    map_set(&widget->objects, el->name, object);
  }

  return 1;
}
void tico_widget_object_deinit(tc_ObjectWidget *widget) {
  ASSERT(widget != NULL);
  map_deinit(&widget->objects);
}

int tico_widget_object_draw(tc_ObjectWidget *widget, tc_PanelWidget *panel, int *active) {
  ASSERT(widget != NULL);
  ASSERT(active != NULL);
  // if (!active || !*active) return;
  // ImVec2 cPos;
  // igGetCursorScreenPos(&cPos);
  tc_Vec2 origin;
  tico_widget_panel_origin(panel, &origin);
  ImDrawList *list = igGetWindowDrawList();
  float scale = panel->scale;
  // int mx = 0;
  // int my = 0;


  // tico_widget_panel_begin(&widget->panel);
  if (igIsMouseClicked(1, 0)) {
    tico_input_get_mouse_pos(&widget->last_x, &widget->last_y);
  }
  if (*active && igBeginPopupContextItem("object_context_menu", ImGuiPopupFlags_MouseButtonRight)) {
    // if (igSelectableBool("new##new_object", "", 0, (ImVec2){64, 16})) {
    //   char uuid[8];
    //   tico_utils_generate_uuid(uuid, 8);
      int mx = widget->last_x;
      int my = widget->last_y;
      tc_Vec2 obj_pos;
      obj_pos.x = (mx - origin.x) / scale;
      obj_pos.y = (my - origin.y) / scale;
    //   tc_Object obj = tico_object_create("teste", obj_pos);
    //   map_set(&widget->objects, uuid, obj);
    //   tico_json_set_object(widget->object_data, uuid, obj.data);
    // }
    if (igBeginMenu("new##new_object", 1)) {
      if (igMenuItemBool("player##new_player", NULL, 0, 1)) {
        tico_widget_object_insert_new(widget, "player", obj_pos);
      }
      if (igMenuItemBool("enemy##new_lizard", NULL, 0, 1)) {
        tico_widget_object_insert_new(widget, "lizard", obj_pos);
      }
      if (igMenuItemBool("floor##new_floor", NULL, 0, 1)) {
        tico_widget_object_insert_new(widget, "floor", obj_pos);
      }
      igEndMenu();
    }
    igEndPopup();
  }
  // tc_Texture *tex = widget->texture;
  // if (tex) {
	// 	ImVec2 canvas_p1;
	// 	canvas_p1.x = origin.x + (tex->width * scale);
	// 	canvas_p1.y = origin.y + (tex->height * scale);
	// 	// TRACELOG("Teste");
	// 	ImDrawList_AddImage(list, (ImTextureID)tex->id, (ImVec2){origin.x, origin.y}, canvas_p1, (ImVec2){uv.x, uv.y}, (ImVec2){uv.w, uv.h}, 0xffffffff);
	// 	ImDrawList_AddRect(list, (ImVec2){origin.x, origin.y}, canvas_p1, 0x99ffffff, 0, 0, 2);
	// }

  map_iter_t iter = map_iter(&widget->objects);
  const char *key;
  // int selected = widget->selected ? 1 : 0;
  int selected = 0;
  while ((key = map_next(&widget->objects, &iter))) {
    tc_Object *object = map_get(&widget->objects, key);
    tc_Vec2 origin;
    tico_widget_panel_origin(panel, &origin);

    if (!selected && *active) selected = tico_widget_object_selected(widget, object, origin, scale);
    tico_object_draw(object, origin, scale, widget->selected == object);
    // TRACELOG("%s %f %f", key, object->position.x, object->position.y);
    // if (igIsItemClicked(0)) {
    //   int is_hovered = tico_object_is_hovered(object, widget);
    //   clicked = 1;
    //   if (is_hovered) {
    //     widget->selected = object;
    //     hovered = 1;
    //   }
    //   // else if ( widget->selected = NULL;
    // }
  }

  if (!selected) widget->selected = NULL;
  // if (clicked && !hovered) widget->selected = NULL;

  // if (widget->selected && igIsItemHovered(0)) {
  //   ImVec2 delta;
  //   if (igIsMouseDragging(0, 0)) {
  //     ImGuiIO *io = igGetIO();
  //     widget->selected->position.x += io->MouseDelta.x / widget->panel.scale;
  //     widget->selected->position.y += io->MouseDelta.y / widget->panel.scale;
  //   }
  // }


  // tico_widget_panel_end(&widget->panel);
}

#define setup_props() \
  ASSERT(json != NULL); \
  int sz = strlen(json->string) + 2 + strlen(uuid); \
  char key[sz]; \
  sprintf(key, "%s##%s", json->string, uuid); \

static void tico_object_string_prop(const char *uuid, cJSON *json) {
  const char *str = json->valuestring;
  igText("type: %s", str);
}

static void tico_object_vec2_prop(const char *uuid, cJSON *json) {
  // ASSERT(uuid != NULL);
  ASSERT(json != NULL);
  int sz = strlen(json->string) + 2 + strlen(uuid);
  char key[sz];
  sprintf(key, "%s##%s", json->string, uuid);

  tc_Vec2 vec = tico_json_to_vec2(json);
  igDragFloat2(json->string, vec.data, 1, -10000000, 100000000, "%.0f", 0);
  // cJSON *x = cJSON_GetArrayItem(json, 0);
  // cJSON *y = cJSON_GetArrayItem(json, 1);
  // x->valuedouble = vec.x;
  // y->valuedouble = vec.y;
  tico_json_set_vec2(json, vec);
}



void tico_widget_object_draw_props(tc_ObjectWidget *widget) {
  ASSERT(widget != NULL);

  if (widget->selected) {
    tc_Object *obj = widget->selected;
    char *uuid = obj->data->name;
    if (igButton("reload", (ImVec2){64, 16})) {
      // lua_State *L = tico_plugin_lua_get_state();
      // lua_pushstring(L, "object_edit");
      // lua_pushstring(L, uuid);
      // tico_lua_json_new_object(L, obj->data);
      // tico_plugin_lua_pcall("editor_change", 3);
    }
    // igInputText("name", obj->name, 128, 0, NULL, NULL);
    // igInputText("type", obj->type, 128, 0, NULL, NULL);
    // cJSON *prop = NULL;
    
    // cJSON_ArrayForEach(prop, obj->data) {
    //   if (tico_json_is_vec2(prop)) tico_object_vec2_prop(uuid, prop);
    //   if (cJSON_IsString(prop)) tico_object_string_prop(uuid, prop);
    // }
    tico_field_draw_imgui(obj->data);
    // igDragFloat2("size", obj->size.data, 1, -1000000, 10000000, "%.0f", 0);
    // igDragFloat("angle", &obj->angle, 1, 0, 360, "%.0f", 0);
  }
}

int tico_widget_object_selected(tc_ObjectWidget *widget, tc_Object *object, tc_Vec2 origin, float scale) {
  int res = 0;
  tc_Vec2 position = tico_json_get_vec2(object->data, "position", 0);
  if (widget->selected == object) res = 1;
  if (igIsItemClicked(0)) {
    int is_hovered = tico_object_is_hovered(object, origin, scale);
    // clicked = 1;
    if (is_hovered) {
      widget->selected = object;
      res = 1;
      // hovered = 1;
    } else if (widget->selected == object) {
      widget->selected = NULL;
      res = 0;
    }
    // if (clicked && !hovered) widget->selected = NULL;
    // else if ( widget->selected = NULL;
  }

  if (widget->selected == object && igIsItemHovered(0)) {
    ImVec2 delta;
    if (igIsMouseDragging(0, 0)) {
      ImGuiIO *io = igGetIO();
      position.x += io->MouseDelta.x / scale;
      position.y += io->MouseDelta.y / scale;
      tico_json_add_vec2(object->data, "position", position);
      // tico_json_set_vec2(object->data, position);
    }
  }
  return res;
}
void tico_widget_object_resize(tc_ObjectWidget *widget, tc_Object *object) {

}

#endif
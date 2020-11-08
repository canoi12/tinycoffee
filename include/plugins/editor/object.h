#ifndef TICO_PLUGIN_EDITOR_OBJECT_H
#define TICO_PLUGIN_EDITOR_OBJECT_H

// #include "../editor.h"

typedef struct tc_ObjectEditor {
  tc_EditorWindow *window;

  // tc_Template temp;
  tc_Field *ref;
  tc_Field *data;
  // cJSON *obj_data;

  tc_PanelWidget panel_widget;
  tc_ObjectWidget object_widget;
} tc_ObjectEditor;

TIC_API tc_EditorPlugin tico_plugin_editor_object();
TIC_API int tico_plugin_editor_object_init(tc_ObjectEditor *editor, tc_EditorWindow *win);
TIC_API int tico_plugin_editor_object_update(tc_ObjectEditor *editor);
TIC_API int tico_plugin_editor_object_draw(tc_ObjectEditor *editor);
TIC_API int tico_plugin_editor_object_draw_panel(tc_ObjectEditor *editor);
TIC_API int tico_plugin_editor_object_destroy(tc_ObjectEditor *editor);
TIC_API int tico_plugin_editor_object_save(tc_ObjectEditor *editor);

#endif

// #define TICO_PLUGIN_EDITOR_IMPLEMENTATION

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_EditorPlugin tico_plugin_editor_object() {
  tc_EditorPlugin plugin = {0};

	plugin.size = sizeof(tc_ObjectEditor);
	plugin.init = (EditorPluginInit)tico_plugin_editor_object_init;
	plugin.draw = (EditorPluginDraw)tico_plugin_editor_object_draw;
  plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_object_draw_panel;
  plugin.destroy = (EditorPluginDestroy)tico_plugin_editor_object_destroy;
  plugin.save = (EditorPluginSave)tico_plugin_editor_object_save;
	// plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_tilemap_draw_panel;

	return plugin; 
}

int tico_plugin_editor_object_init(tc_ObjectEditor *editor, tc_EditorWindow *window) {
  ASSERT(editor != NULL);
  ASSERT(window != NULL);
  editor->window = window;
  tc_Resource *res = window->res;

  // editor->obj_data = res->data;

  editor->panel_widget = tico_widget_panel_create(64);

  // tico_template_init(&editor->temp, res->data);
  editor->data = tico_field_from_json(res->data, NULL);
  // TRACELOG("%d", ((cJSON*)res->data)->type);
  // cJSON *data = tico_json_create_object(NULL);
  // tico_json_add_object(data, "testandow", res->data);
  // tico_widget_object_init(&editor->object_widget, data);

  return 1;
}

int tico_plugin_editor_object_draw(tc_ObjectEditor *editor) {
  tc_Vec2 pos;
  tico_widget_panel_origin(&editor->panel_widget, &pos);
  float scale = editor->panel_widget.scale;

  tico_widget_panel_begin(&editor->panel_widget);
  // tico_template_draw(&editor->temp, pos, scale);
  // tico_
  tico_field_draw(editor->data, pos, scale);
  tico_widget_panel_end(&editor->panel_widget);
  // list_iter_t iter = list_iter(&editor->te)
  return 1;
}

int tico_plugin_editor_object_draw_panel(tc_ObjectEditor *editor) {
  ImVec2 size;
  igGetContentRegionAvail(&size);
  size.y = 16;
  if (igButton("new field", size)) {
    // tico_template_insert_number(&editor->temp, "", 0);
    tico_field_add_number(editor->data, "novo campo", 0);
    // tico_field_push(editor->data, -1, new_field);
  }
  // tico_template_imgui(&editor->temp);
  // tico_field_imgui(editor->data);
  tico_field_draw_imgui(editor->data);
  // if (igButton("qqqqqqqqqqqqq", (ImVec2){48, 16})) {
  //   TRACELOG("qqqqq");
  // }
  return 1;
}

int tico_plugin_editor_object_destroy(tc_ObjectEditor *editor) {
  // tico_template_destroy(&editor->temp);
  tico_field_delete(editor->data);

  return 1;
}

int tico_plugin_editor_object_save(tc_ObjectEditor *editor) {
  ASSERT(editor != NULL);
  // cJSON *json = tico_field_to_json(editor->data, 1);
  return 0;


  tc_Resource *res = editor->window->res;
  cJSON *json = NULL;
  ASSERT(res != NULL);
  tico_json_delete(res->data);
  // TRACELOG("%p", json);

  res->data = json;
  tico_json_save(res->path, json);

  return 1;
}

#endif
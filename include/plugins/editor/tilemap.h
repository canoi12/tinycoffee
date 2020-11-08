#ifndef TICO_PLUGIN_EDITOR_TILEMAP_H
#define TICO_PLUGIN_EDITOR_TILEMAP_H

// #include "widget.h"

enum TILEMAP_MODE_ {
	TILEMAP_MODE_TILE = 0,
	TILEMAP_MODE_OBJECT
};

typedef struct tc_TilemapEditor {
  tc_EditorWindow *win;
  tc_Canvas canvas;
  tc_Canvas bg;
	tc_Camera camera;
  tc_Tilemap map;
  void (*draw_panel)(void*);
  char *name;
  char *filename;
  char *tileset_name;
  int cell;
  float scale;
  tc_Vec2 size;
  void *cnt;
	int layer;
	// tc_GridTool grid;
	tc_PanelWidget panel_widget;
  tc_GridWidget grid_widget;
	tc_ObjectWidget object_widget;
	int mode;
} tc_TilemapEditor;

TIC_API tc_EditorPlugin tico_plugin_editor_tilemap();
TIC_API int tico_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_EditorWindow *win);
TIC_API int tico_plugin_editor_tilemap_update(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_draw(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_destroy(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_save(tc_TilemapEditor *editor);

#endif // TICO_PLUGIN_EDITOR_TILEMAP_H

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_EditorPlugin tico_plugin_editor_tilemap() {
	tc_EditorPlugin plugin = {0};

	plugin.size = sizeof(tc_TilemapEditor);
	plugin.init = (EditorPluginInit)tico_plugin_editor_tilemap_init;
	plugin.draw = (EditorPluginDraw)tico_plugin_editor_tilemap_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_tilemap_draw_panel;
	plugin.destroy = (EditorPluginDestroy)tico_plugin_editor_tilemap_destroy;
	plugin.save = (EditorPluginSave)tico_plugin_editor_tilemap_save;

	return plugin;
}

int tico_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_EditorWindow *win) {
	ASSERT(win != NULL);
	ASSERT(win->res != NULL);
	memset(editor, 0, sizeof(*editor));

	editor->win = win;

	// TRACELOG("teste");
	// tc_Tilemap *map = win->res->data;
	cJSON *json = win->res->data;
	editor->tileset_name = NULL;
	// editor->tool = tool;
	editor->filename = NULL;
	// editor->map = map;
	tico_tilemap_from_json(&editor->map, json);
	tc_Tilemap *map = &editor->map;

	editor->cell = -1;
	editor->scale = 1;
	editor->layer = 0;

  tc_Vec2 sz;
  sz.x = map->grid.x * map->width;
  sz.y = map->grid.y * map->height;

	// editor->canvas = tico_canvas_create(16*16, 16*16);
	// editor->bg = tico_canvas_create(32, 32);
	// TRACELOG("%d %d", map->width, map->height);
	editor->canvas = tico_canvas_create(sz.x, sz.y);
	editor->bg = tico_canvas_create(2, 2);
	tico_texture_set_wrap(&editor->bg.tex, GL_REPEAT, GL_REPEAT);
	editor->camera = tico_camera_create(0, 0, editor->canvas.width, editor->canvas.height);
	editor->mode = 0;
	// tico_widget_object_init(&editor->object_widget);

	editor->size.x = map->width;
	editor->size.y = map->height;

	editor->cnt = NULL;
	editor->draw_panel = NULL;
	
	editor->grid_widget = tico_widget_grid_create(map->grid, sz, 0);
	editor->panel_widget = tico_widget_panel_create(64);

	cJSON *obj_json = tico_json_get_object(json, "objects", 0);

	tc_Field *field = tico_field_from_json(obj_json, NULL);
	// TRACELOG("%p", obj_json);
	tico_widget_object_init(&editor->object_widget, field);
	// TRACELOG("Testew");
	tico_widget_grid_set_texture(&editor->grid_widget, &editor->canvas.tex, tico_rectf(0, 1, 1, 0));
	// if (tool->res) tico_editor_tilemap_change(editor, tool->res->data);
	// tool->grid = tico_tool_grid_create()
}

int tico_plugin_editor_tilemap_destroy(tc_TilemapEditor *editor) {
	tico_canvas_destroy(&editor->canvas);
	tico_canvas_destroy(&editor->bg);
	tico_widget_object_deinit(&editor->object_widget);
	tico_widget_grid_destroy(&editor->grid_widget);
}

int tico_plugin_editor_tilemap_draw(tc_TilemapEditor *editor) {

  ASSERT(editor != NULL);
	tc_Tilemap *map = &editor->map;
	// tc_Resource *res = editor->tool->res;
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;

	// tico_editor_set_bg(editor->bg, 1, tico_color3(102, 59, 147), tico_color3(139, 114, 222));
  int ssz = 1;
  // tico_canvas_attach(editor->bg);
  // tico_graphics_clear(tico_color3(102, 59, 147));
  // tico_graphics_fill_rectangle(ssz, 0, ssz, ssz, tico_color3(139, 114, 222));
  // tico_graphics_fill_rectangle(0, ssz, ssz, ssz, tico_color3(139, 114, 222));
  // tico_canvas_detach();
	ImVec2 cPos;
	igGetCursorScreenPos(&cPos);

	// int ww, wh;
	ImVec2 size;
	// igGetWindowSize(&size);
	igGetContentRegionAvail(&size);


	// if (igBeginChildStr("testew", size, 0, 0)) {
		int gw, gh;
		gw = editor->map.grid.x;
		gh = editor->map.grid.y;


		// tico_canvas_attach(editor->canvas);
		// tico_graphics_clear(tico_rgba(0, 0, 0, 0));
		// tico_graphics_fill_rectangle(0, 0, cw, ch, tico_color3(75, 90, 90));
		// tico_canvas_draw_part_scale(editor->bg, tico_rectf(0, 0, cw, ch), 0, 0, editor->map.grid.x, editor->map.grid.y, WHITE);
		// // TRACELOG("testando %d", editor->map->width);
		// tico_camera_attach(editor->camera);
		// if (editor->map) tico_tilemap_draw(editor->map);
		// tico_tilemap_draw(editor->map);
		// tico_graphics_fill_rectangle(cx, cy, gw, gh, tico_color(0, 0, 0, 125));
		// tico_graphics_draw_rectangle(cx, cy, gw, gh, tico_color3(0, 0, 0));
		// tico_graphics_draw_rectangle(0, 0, editor->map->width * gw, editor->map->height * gh, WHITE);
		// tico_camera_detach();
		// tico_canvas_detach();

		int mdx, mdy;
		tico_input_get_mouse_delta(&mdx, &mdy);
		
		// if (tico_input_key_down(KEY_LEFT_ALT)) {
		// 	tc_Vec2 pos;
		// 	tc_Camera *camera = &editor->grid.camera;
		// 	tico_camera_get_pos(*camera, &pos);
		// 	pos.x -= mdx;
		// 	pos.y -= mdy;
		// 	tico_camera_set_pos(camera, pos);
		// }

		int cell = -1;
		int res = 0;
		
		tico_widget_panel_begin(&editor->panel_widget);
		tc_Vec2 origin;
		tico_widget_panel_origin(&editor->panel_widget, &origin);
		float scale = editor->panel_widget.scale;
		editor->grid_widget.grid = editor->map.grid;
		// ImDrawList* list = igGetWindowDrawList();
		// tc_Vec2 origin;
		// tico_widget_panel_origin(&editor->panel_widget, &origin);
		// ImVec2 pos1;
		// pos1.x = origin.x + size.x;
		// pos1.y = origin.y + size.y;
		// ImDrawList_AddImage(list, editor->canvas.tex.id, (ImVec2){origin.x, origin.y}, pos1, (ImVec2){0, 1}, (ImVec2){1, 0}, 0xffffffff);
		if (!editor->layer) tico_widget_grid_update(&editor->grid_widget, &editor->panel_widget);

		ImDrawList *list = igGetWindowDrawList();
		// ImDrawList_AddImage(list, map->tileset->image->texture.id, cPos, (ImVec2){cPos.x+128, cPos.y+128}, (ImVec2){0, 0}, (ImVec2){1, 1}, 0xffffffff);
		for (int i = 0; i < map->count; i++) {
			// TRACELOG("%d", i);
			tc_Texture *tex = &map->tileset->image->texture;
			int x = fmod(i, map->width) * (map->tileset->tilesize.x*scale);
			int y = floor(i / map->width) * (map->tileset->tilesize.y*scale);
			// tico_tileset_draw(*map->tileset, map->data[i], x, y, WHITE);
			tc_Rectf tileset_rect = tico_tileset_get_rect(*map->tileset, map->data[i]);
			// tc_Rectf rect = tico_rectf(tileset_rect.x/tex->width, tileset_rect.y/tex->height, tileset_rect.w/tex->width, tileset_rect.h/tex->height);
			tileset_rect.x = tileset_rect.x/tex->width;
			tileset_rect.y = tileset_rect.y/tex->height;
			tileset_rect.w = tileset_rect.x + (tileset_rect.w/tex->width);
			tileset_rect.h = tileset_rect.y + (tileset_rect.h/tex->height);
			int id = map->tileset->image->texture.id;
			ImVec2 pos0 = (ImVec2){x+origin.x, y+origin.y};
			ImVec2 pos1 = (ImVec2){pos0.x+(map->tileset->tilesize.x*scale), pos0.y+(map->tileset->tilesize.y*scale)};
			ImDrawList_AddImage(list, id, pos0, pos1, (ImVec2){tileset_rect.x, tileset_rect.y}, (ImVec2){tileset_rect.w, tileset_rect.h}, 0xffffffff);
		}

		tico_widget_grid_draw(&editor->grid_widget, &editor->panel_widget);
		tico_widget_object_draw(&editor->object_widget, &editor->panel_widget, &editor->layer);
		tico_widget_panel_end(&editor->panel_widget);
		if (igIsWindowFocused(0) && (editor->grid_widget.state & TICO_WIDGET_DOWN)) {
			// TRACELOG("%d", editor->grid_widget.state);
			cell = editor->grid_widget.cell;
			if (igIsMouseDown(MOUSE_LEFT)) {
				tico_tilemap_insert(&editor->map, cell);
			} else if (igIsMouseDown(MOUSE_RIGHT)) {
				tico_tilemap_remove(&editor->map, cell);
			}
		}
		// tico_widget_object_draw(&editor->object_widget, NULL, &editor->canvas.tex, tico_rectf(0, 1, 1, 0), 0);
		return res;
}

int tico_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor) {
	// igInputFloat2("grid##tilemap_editor_grid", &editor->map.grid, "%.0f", 0);
	igDragFloat2("grid##tilemap_editor_grid", &editor->map.grid, 1, 1, 1000, "%.0f", 0);
	igCheckbox("object", (_Bool*)&editor->layer);

	if (editor->object_widget.selected && igTreeNodeStr("object##prop_tree")) {
		tico_widget_object_draw_props(&editor->object_widget);
		igTreePop();
	}

	return 1;
}

int tico_plugin_editor_tilemap_save(tc_TilemapEditor *editor) {
	ASSERT(editor != NULL);
	ASSERT(editor->win != NULL);
	int i = 0;
	tc_Resource *res = editor->win->res;
	tc_Tilemap *map = &editor->map;
	cJSON *json = res->data;

	cJSON *array = tico_json_get_array(json, "data", 0);
	cJSON *tile = NULL;
	cJSON_ArrayForEach(tile, array) {
		tile->valuedouble = map->data[i];
		i++;
	}
}

#endif
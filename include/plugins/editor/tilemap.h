#ifndef TICO_PLUGIN_EDITOR_TILEMAP_H
#define TICO_PLUGIN_EDITOR_TILEMAP_H

typedef struct tc_TilemapEditor {
  tc_EditorWindow *win;
  tc_Canvas canvas;
  tc_Canvas bg;
	tc_Camera camera;
  tc_Tilemap *map;
  void (*draw_panel)(void*);
  char *name;
  char *filename;
  char *tileset_name;
  int cell;
  float scale;
  tc_Vec2 size;
  void *cnt;
	// tc_GridTool grid;
  tc_GridWidget grid;
} tc_TilemapEditor;

TIC_API tc_EditorPlugin tico_plugin_editor_tilemap();
TIC_API int tico_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_EditorWindow *win);
TIC_API int tico_plugin_editor_tilemap_draw(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor);
TIC_API int tico_plugin_editor_tilemap_destroy(tc_TilemapEditor *editor);

#endif // TICO_PLUGIN_EDITOR_TILEMAP_H

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_EditorPlugin tico_plugin_editor_tilemap() {
	tc_EditorPlugin plugin = {0};

	plugin.size = sizeof(tc_TilemapEditor);
	plugin.init = (EditorPluginInit)tico_plugin_editor_tilemap_init;
	plugin.draw = (EditorPluginDraw)tico_plugin_editor_tilemap_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_tilemap_draw_panel;
	plugin.destroy = (EditorPluginDestroy)tico_plugin_editor_tilemap_destroy;

	return plugin;
}

int tico_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_EditorWindow *win) {
	ASSERT(win != NULL);
	ASSERT(win->res != NULL);
	tc_Tilemap *map = win->res->data;
	editor->tileset_name = NULL;
	// editor->tool = tool;
	editor->filename = NULL;
	editor->map = map;
	editor->cell = -1;
	editor->scale = 1;

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

	editor->size.x = map->width;
	editor->size.y = map->height;

	editor->cnt = NULL;
	editor->draw_panel = NULL;
	
	editor->grid = tico_widget_grid_create(map->grid, sz);
	// if (tool->res) tico_editor_tilemap_change(editor, tool->res->data);
	// tool->grid = tico_tool_grid_create()
}

int tico_plugin_editor_tilemap_destroy(tc_TilemapEditor *editor) {
	tico_canvas_destroy(&editor->canvas);
	tico_canvas_destroy(&editor->bg);
	tico_widget_grid_destroy(&editor->grid);
}

int tico_plugin_editor_tilemap_draw(tc_TilemapEditor *editor) {

  ASSERT(editor != NULL);
	// tc_Resource *res = editor->tool->res;
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;

	// tico_editor_set_bg(editor->bg, 1, tico_color3(102, 59, 147), tico_color3(139, 114, 222));
  int ssz = 1;
  tico_canvas_attach(editor->bg);
  tico_graphics_clear(tico_color3(102, 59, 147));
  tico_graphics_fill_rectangle(ssz, 0, ssz, ssz, tico_color3(139, 114, 222));
  tico_graphics_fill_rectangle(0, ssz, ssz, ssz, tico_color3(139, 114, 222));
  tico_canvas_detach();

	// int ww, wh;
	ImVec2 size;
	// igGetWindowSize(&size);
	igGetContentRegionAvail(&size);


	// if (igBeginChildStr("testew", size, 0, 0)) {
		int gw, gh;
		gw = editor->map->grid.x;
		gh = editor->map->grid.y;


		tico_canvas_attach(editor->canvas);
		tico_graphics_clear(BLACK);
		// tico_graphics_fill_rectangle(0, 0, cw, ch, tico_color3(75, 90, 90));
		tico_canvas_draw_part_scale(editor->bg, tico_rectf(0, 0, cw, ch), 0, 0, 16, 16, WHITE);
		// // TRACELOG("testando %d", editor->map->width);
		// tico_camera_attach(editor->camera);
		if (editor->map) tico_tilemap_draw(*editor->map);
		// tico_graphics_fill_rectangle(cx, cy, gw, gh, tico_color(0, 0, 0, 125));
		// tico_graphics_draw_rectangle(cx, cy, gw, gh, tico_color3(0, 0, 0));
		// tico_graphics_draw_rectangle(0, 0, editor->map->width * gw, editor->map->height * gh, WHITE);
		// tico_camera_detach();
		tico_canvas_detach();

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
		int res = tico_widget_grid_draw(&editor->grid, &cell, &editor->canvas.tex, (tc_Rectf){0, 1, 1, 0}, WHITE, 0);
		if (igIsWindowFocused(0) && res) {
			if (igIsMouseDown(MOUSE_LEFT)) {
				tico_tilemap_insert(editor->map, cell);
			} else if (igIsMouseDown(MOUSE_RIGHT)) {
				tico_tilemap_remove(editor->map, cell);
			}
		}
}

int tico_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor) {
}

#endif
#ifndef TICO_PLUGIN_EDITOR_H
#define TICO_PLUGIN_EDITOR_H

#include "../tico.h"

typedef int(*EditorPluginInit)(void*, tc_Tool*);
typedef int(*EditorPluginUpdate)(void*);
#define EditorPluginCreate EditorPluginUpdate
#define EditorPluginDraw EditorPluginUpdate
#define EditorPluginDrawPanel EditorPluginUpdate

struct tc_EditorPlugin {
  void *editor;
  int size;
  EditorPluginCreate create;
  EditorPluginInit init;
  EditorPluginUpdate update;
  EditorPluginDraw draw;
  EditorPluginDrawPanel draw_panel;
  // void (*update)(void*);
  // void (*draw)(void*);
  // void (*draw_panel)(void*);
};

TIC_API tc_EditorPlugin tic_plugin_create(EditorPluginInit init, EditorPluginUpdate update, EditorPluginDraw draw, EditorPluginDrawPanel draw_panel);

/****************
 * Game
 ****************/

typedef struct tc_GameEditor {
  tc_Tool *tool;
  tc_Canvas *canvas;
  int paused;
} tc_GameEditor;

TIC_API tc_EditorPlugin tic_plugin_editor_game();
TIC_API int tic_plugin_editor_game_init(tc_GameEditor *editor, tc_Tool *tool);
TIC_API int tic_plugin_editor_game_draw(tc_GameEditor *editor);
TIC_API int tic_plugin_editor_game_draw_panel(tc_GameEditor *editor);

/****************
 * Tileset
 ****************/

typedef struct tc_TilesetEditor {
  tc_Tool *tool;
  tc_Canvas canvas;
  tc_Tileset *tileset;
  void (*draw_panel)(void*);
  char *name;
  char *filename;
  char *image_name;
  int cell;
  float scale;
  int mask[9];
  void *cnt;
} tc_TilesetEditor;

TIC_API tc_EditorPlugin tic_plugin_editor_tileset();
TIC_API int tic_plugin_editor_tileset_init(tc_TilesetEditor *editor, tc_Tool *tool);
TIC_API int tic_plugin_editor_tileset_draw(tc_TilesetEditor *editor);
TIC_API int tic_plugin_editor_tileset_draw_panel(tc_TilesetEditor *editor);

/****************
 * Tilemap
 ****************/

typedef struct tc_TilemapEditor {
  tc_Tool *tool;
  tc_Canvas canvas;
  tc_Canvas bg;
  tc_Tilemap *map;
  void (*draw_panel)(void*);
  char *name;
  char *filename;
  char *tileset_name;
  int cell;
  float scale;
  tc_Vec2 size;
  void *cnt;
} tc_TilemapEditor;

TIC_API tc_EditorPlugin tic_plugin_editor_tilemap();
TIC_API int tic_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_Tool *tool);
TIC_API int tic_plugin_editor_tilemap_draw(tc_TilemapEditor *editor);
TIC_API int tic_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor);

/******************
 * Sprite
 ******************/

typedef struct tc_SpriteEditor {
	tc_Tool *tool;
} tc_SpriteEditor;

TIC_API tc_EditorPlugin tic_plugin_editor_sprite();
TIC_API int tic_plugin_editor_sprite_init(tc_SpriteEditor *editor, tc_Tool *tool);
TIC_API int tic_plugin_editor_sprite_draw(tc_SpriteEditor *editor);
TIC_API int tic_plugin_editor_sprite_draw_panel(tc_SpriteEditor *editor);

#endif

// #define TICO_PLUGIN_IMPLEMENTATION

#if defined(TICO_PLUGIN_IMPLEMENTATION)

tc_EditorPlugin tic_plugin_create(EditorPluginInit init, EditorPluginUpdate update, EditorPluginDraw draw, EditorPluginDrawPanel draw_panel) {
	tc_EditorPlugin plugin = {0};
	plugin.size = 0;
	// plugin.data = NULL;
	plugin.create = NULL;
	plugin.init = init;
	plugin.update = update;
	plugin.draw = draw;
	plugin.draw_panel = draw_panel;
	return plugin;
}


/****************
 * Game
 ****************/

tc_EditorPlugin tic_plugin_editor_game() {
	// tc_EditorPlugin plugin = tic_plugin_create(tic_plugin_editor_game_init);
	tc_EditorPlugin plugin = {0};
	plugin.init = (EditorPluginInit)tic_plugin_editor_game_init;
	plugin.draw = (EditorPluginDraw)tic_plugin_editor_game_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tic_plugin_editor_game_draw_panel;

	plugin.size = sizeof(tc_GameEditor);

	return plugin;
}

int tic_plugin_editor_game_init(tc_GameEditor *editor, tc_Tool *tool) {
	editor->tool = tool;
	editor->canvas = NULL;
	editor->paused = 0;
}

int tic_plugin_editor_game_draw(tc_GameEditor *editor) {
	mu_Container *cnt = (mu_Container*)editor->tool->cnt;
	if (editor->canvas) {
		// tic_ui_layout_row(1, ((int[]){-1}), -1);
		tc_Canvas *c = editor->canvas;
		float cw = c->width;
		float ch = c->height;
		// float cw = editor->tilemap_editor.canvas.width;
		// float ch = editor->tilemap_editor.canvas.height;
		float ww = cnt->body.w;
		float hh = cnt->body.h;
		float ratio = tic_min((ww-10)/cw, (hh-12)/ch);

		int i = cw*ratio;
		tic_ui_layout_row(1, &i, ch*ratio);

		// TRACELOG("%f", ratio);

		tic_ui_image(c->texture, mu_vec2(0, 0), tic_rect(0, (int)ch, (int)cw, (int)-ch), WHITE);
	}
}

int tic_plugin_editor_game_draw_panel(tc_GameEditor *editor) {}


/****************
 * Tileset
 ****************/

tc_EditorPlugin tic_plugin_editor_tileset() {
	tc_EditorPlugin plugin = {0};
	plugin.size = sizeof(tc_TilesetEditor);
	plugin.init = (EditorPluginInit)tic_plugin_editor_tileset_init;
	plugin.draw = (EditorPluginDraw)tic_plugin_editor_tileset_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tic_plugin_editor_tileset_draw_panel;

	return plugin;
}

int tic_plugin_editor_tileset_init(tc_TilesetEditor *editor, tc_Tool *tool) {
	editor->cell = -1;
	memset(editor->mask, 0, sizeof(int) * 9);
	editor->tool = tool;
	editor->tileset = NULL;
	editor->image_name = NULL;
	editor->scale = 3;
	editor->cnt = NULL;

	editor->draw_panel = NULL;
	if (tool->res) tic_editor_tileset_change(editor, tool->res->data);
	return 1;
}

int tic_plugin_editor_tileset_draw(tc_TilesetEditor *editor) {
	int res = 0;
	tic_canvas_attach(editor->canvas);
	tic_graphics_clear(tic_color(0, 0, 0, 0));
	tic_image_draw(*editor->tileset->image, 0, 0, WHITE);
	float hh = editor->tileset->image->height / editor->tileset->tilesize.y;
	float mw = editor->tileset->tilesize.x/3;
	float mh = editor->tileset->tilesize.y/3;

	int columns = editor->tileset->columns;
	for (int i = 0; i < editor->tileset->tilecount; i++) {
		float xx = fmod(i, columns) * editor->tileset->tilesize.x;
		float yy = floor(i / columns) * editor->tileset->tilesize.y;
		int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		tic_tileset_calc_mask_array(*editor->tileset, editor->tileset->bitmasks[i], mask);
		for (int j = 0; j < 9; j++) {
			float mx = xx + (fmod(j, 3) * mw);
			float my = yy + (floor(j / 3) * mh);

			if (mask[j] > 0) tic_graphics_fill_rectangle(mx, my, mw, mh, tic_color(128, 12, 83, 175));
		}
	}
	tic_canvas_detach();

	mu_Container *cnt = editor->tool->cnt;

	if (Core.ui.ctx->hover_root == cnt) {
		if (tic_input_key_down(KEY_LEFT_CONTROL)) {
			float y;
			tic_input_get_mouse_scroll_delta(NULL, &y);
			if (y) editor->scale += y/10.f;
			editor->scale = tic_clamp(editor->scale, 1, 10);
		}
	}

	tic_ui_layout_row(1, ((int[]){-1}), -1);

	tic_ui_begin_column();
	mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, -1);
	tic_ui_begin_panel("tileset_panel");

	tc_Tileset *tileset = editor->tileset;

	mu_layout_row(Core.ui.ctx, 1, (int[]){tileset->image->width*editor->scale}, tileset->image->height*editor->scale);
	int tw = tileset->image->width / tileset->tilesize.x;
	int th = tileset->image->height / tileset->tilesize.y;
	if (tic_ui_image_grid_ex("tileset_grid", editor->canvas.texture, tic_rect(0, editor->canvas.height, editor->canvas.width, -editor->canvas.height), mu_vec2(tw, th), &editor->cell, 0)) {

		tic_tileset_calc_mask_array(*editor->tileset, tileset->bitmasks[editor->cell], editor->mask);
		tic_ui_open_popup("tile_bitmask");
	}


	tc_Tool *t = editor->tool;
	if (t && t->res) t->res->changed = tc_false;
	if (tic_ui_begin_popup("tile_bitmask")) {
		int r = 96;
		tic_ui_layout_row(1, &r, 96);
    tc_Rectf rect = tic_tileset_get_rect(*tileset, editor->cell);
    if (tic_ui_image_grid_ex("bitmask_grid", tileset->image->texture, tic_rect(rect.x, rect.y, rect.w, rect.h), mu_vec2(3, 3), editor->mask, MU_OPT_INPUTDOWN | MU_OPT_GRIDMULTI)) {
			tileset->bitmasks[editor->cell] = tic_tileset_calc_mask(*tileset, editor->mask);
			if (t && t->res) t->res->changed = tc_true;
		}
		mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, 0);
		if (tic_ui_button("clear")) {
			memset(editor->mask, 0, sizeof(int) * 9);
			tileset->bitmasks[editor->cell] = -1;
			if (t && t->res) t->res->changed = tc_true;
		}
		res = 1;
		tic_ui_end_popup();
	} else {
		editor->cell = -1;
	}
	tic_ui_end_panel();
	tic_ui_end_column();

  return res;
}

int tic_plugin_editor_tileset_draw_panel(tc_TilesetEditor *editor) {
	tic_ui_layout_row(1, ((int[]){-1}), 0);

	if (tic_ui_button("save##tileset")) {
		tic_editor_tileset_save(editor);
	}
	tic_ui_number("scale", &editor->scale, 1);
	editor->scale = tic_clamp(editor->scale, 1, 10);
	return 1;
}


/****************
 * Tilemap
 ****************/

tc_EditorPlugin tic_plugin_editor_tilemap() {
	tc_EditorPlugin plugin = {0};

	plugin.size = sizeof(tc_TilemapEditor);
	plugin.init = (void(*)(void*, tc_Tool*))tic_plugin_editor_tilemap_init;
	plugin.draw = (void(*)(void*))tic_plugin_editor_tilemap_draw;
	plugin.draw_panel = (void(*)(void*))tic_plugin_editor_tilemap_draw_panel;

	return plugin;
}

int tic_plugin_editor_tilemap_init(tc_TilemapEditor *editor, tc_Tool *tool) {
	editor->tileset_name = NULL;
	editor->tool = tool;
	editor->filename = NULL;
	editor->map = NULL;
	editor->cell = -1;
	editor->scale = 1;
	editor->canvas = tic_canvas_create(16*16, 16*16);
	editor->bg = tic_canvas_create(32, 32);
	tic_texture_set_wrap(&editor->bg.texture, GL_REPEAT, GL_REPEAT);
	editor->cnt = NULL;
	editor->draw_panel = NULL;
	if (tool->res) tic_editor_tilemap_change(editor, tool->res->data);
}

int tic_plugin_editor_tilemap_draw(tc_TilemapEditor *editor) {
	int width,height;
	tic_window_get_size(&width, &height);
	char *title = "tilemap";
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;

	tc_Resource *tres = editor->tool->res;
	list_iter_t iter = tres ? list_iter(&tres->deps) : (list_iter_t){0};
	tc_ResourceDep *dep = NULL;
	while((dep = list_next(&iter))) {
		if (dep->res->changed) tic_tilemap_update(editor->map);
	}

	editor_set_bg(editor->bg, 16, tic_color3(102, 59, 147), tic_color3(139, 114, 222));

	tic_canvas_attach(editor->canvas);
	tic_graphics_clear(tic_color(0, 0, 0, 0));
	tic_canvas_draw_part(editor->bg, tic_rectf(0, 0, cw, ch), 0, 0, WHITE);
	if (editor->map) tic_tilemap_draw(*editor->map);
	tic_canvas_detach();

	mu_Container *cnt = editor->tool->cnt;

	if (Core.ui.ctx->hover_root == cnt) {
		if (tic_input_key_down(KEY_LEFT_CONTROL)) {
			float y;
			tic_input_get_mouse_scroll_delta(NULL, &y);
			if (y) editor->scale += y/10.f;
			editor->scale = tic_clamp(editor->scale, 1, 10);
		}
	}

	int tesw = editor->map ? editor->map->width * editor->map->grid.x : 16 * 16;
	int tesh = editor->map ? editor->map->height * editor->map->grid.y : 16 * 16;

	tic_ui_layout_row(1, ((int[]){tesw*editor->scale}), tesh*editor->scale);
	int tw = cw / 16;
	int th = ch / 16;
	int res = tic_ui_image_grid_ex("tilemap_grid", editor->canvas.texture, mu_rect(0, ch, cw, -ch), mu_vec2(tw, th), &editor->cell, MU_OPT_INPUTDOWN);
	if (res & MU_RES_CHANGE) {
		if (res & MU_RES_MOUSELEFT) tic_tilemap_insert(editor->map, editor->cell);
		else if (res & MU_RES_MOUSERIGHT) tic_tilemap_remove(editor->map, editor->cell);
	}
	else editor->cell = -1;
}

int tic_plugin_editor_tilemap_draw_panel(tc_TilemapEditor *editor) {
	int w = -1;
	tic_ui_layout_row(1, &w, 0);
	if (tic_ui_button("save##tilemap")) {
		// tic_editor_tilemap_save(editor);
		tc_Resource *res = editor->tool->res;
		if (editor->tool) {
			
			if (res && strcmp(res->path, editor->tool->filename)) {
				strcpy(res->path, editor->tool->filename);
			}
			tic_resources_tilemap_store(&Core.resources, res);
		}
	}
	tic_ui_number("map width", &editor->size.x, 1);
	tic_ui_number("map height", &editor->size.y, 1);
	if (tic_ui_button("set")) {
		if (editor->size.x != editor->map->width ||
			  editor->size.y != editor->map->height) {
			// tic_tilemap_resize(editor->map, editor->size.x, editor->size.y);
			tic_editor_tilemap_resize(editor, editor->size.x, editor->size.y);
		}
	}
}

#endif
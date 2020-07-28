#include "tico.h"

static void editor_set_bg(tc_Canvas canvas, int size, tc_Color col1, tc_Color col2) {
	tic_canvas_attach(canvas);
	tic_graphics_fill_rectangle(0, 0, size, size, col1);
	tic_graphics_fill_rectangle(size, 0, size, size, col2);
	tic_graphics_fill_rectangle(0, size, size, size, col2);
	tic_graphics_fill_rectangle(size, size, size, size, col1);
	tic_canvas_detach();
}

void tic_editor_load(tc_Editor *editor) {
	// tic_editor_tilemap_init(&editor->tilemap_editor, tc_Tilemap *tilemap)
	tic_editor_tileset_init(&editor->tileset_editor);
	tic_editor_tilemap_init(&editor->tilemap_editor);
	editor->bg = tic_canvas_create(32, 32);
	tic_texture_set_wrap(&editor->bg.texture, GL_REPEAT, GL_REPEAT);

	editor->game_canvas = tic_canvas_create(Core.window.width, Core.window.height);
	editor->sidebar_width = 96;
}

void tic_editor_destroy(tc_Editor *editor) {
	tic_editor_tileset_destroy(&editor->tileset_editor);
	tic_editor_tilemap_destroy(&editor->tilemap_editor);
}

void tic_editor_set_tileset(tc_Editor *editor, const char *name, tc_Tileset *tileset) {
	if (name) {
		tc_Resource *res = tic_resources_get(&Core.resources, "tileset", name);
		// TRACELOG("%s %p", name, res);
		if (!res) {
			ERRLOG("Tileset %s not loaded", name);
			return;
		}
		editor->tileset_editor.name = res->name;
		editor->tileset_editor.filename = res->path;
		tic_editor_tileset_change(&editor->tileset_editor, res->tileset);
	} else {
		editor->tileset_editor.filename = NULL;
		tic_editor_tileset_change(&editor->tileset_editor, tileset);
	}
}

void tic_editor_set_tilemap(tc_Editor *editor, const char *name, tc_Tilemap *tilemap) {
	if (name) {
		tc_Resource *res = tic_resources_get(&Core.resources, "tilemap", name);

		if (!res) {
			ERRLOG("Tilemap %s not loaded", name);
			return ;
		}

		editor->tilemap_editor.name = res->name;
		editor->tilemap_editor.filename = res->path;
		tic_editor_tilemap_change(&editor->tilemap_editor, res->tilemap);
		return;
	}
	editor->tilemap_editor.filename = NULL;
	editor->tilemap_editor.name = NULL;
	tic_editor_tilemap_change(&editor->tilemap_editor, tilemap);
}

void tic_editor_new_tileset(tc_Editor *editor, int w, int h) {}
void tic_editor_new_tilemap(tc_Editor *editor, int w, int h) {}

void tic_editor_update(tc_Editor *editor) {}

void tic_editor_draw(tc_Editor *editor) {
	int w, h;

	editor_set_bg(editor->bg, 16, tic_color3(255, 218, 112), tic_color3(244, 176, 60));

	tic_window_get_size(&w, &h);
	tic_canvas_draw_part(editor->bg, tic_rectf(0, 0, w, h), 0, 0, WHITE);

	// editor_set_bg(editor->tilemap_editor.bg, 16, tic_color3(102, 59, 147), tic_color3(139, 114, 222));
	// tic_canvas_attach(editor->tilemap_editor.canvas);
	// tic_canvas_draw_part(editor->tilemap_editor.bg, tic_rectf(0, 0, editor->tilemap_editor.canvas.width, editor->tilemap_editor.canvas.height), 0, 0, WHITE);
	// tic_tilemap_draw(*editor->tilemap_editor.map);
	// tic_canvas_detach();
	// tic_canvas_draw(editor->tilemap_editor.canvas, -16, 0, WHITE);
	if (editor->tilemap_editor.map) tic_editor_tilemap_draw(&editor->tilemap_editor);
	if (editor->tileset_editor.tileset) tic_editor_tileset_draw(&editor->tileset_editor);

	if (mu_begin_window_ex(Core.ui.ctx, "editor_sidebar", mu_rect(0, 0, editor->sidebar_width, Core.window.height), MU_OPT_NOTITLE | MU_OPT_HRESIZE)) {
		mu_Container *cnt = tic_ui_get_current_container();
		// cnt->rect = tic_rect(0, 0, Core.editor.sidebar_width, Core.window.height);
		// cnt->body = cnt->rect;
		cnt->rect.h = Core.window.height;
		cnt->body.h = Core.window.height;
		Core.editor.sidebar_width = cnt->rect.w;
		int change = 0;
		int w = -1;
		tic_ui_layout_row(1, &w, -48);
		tic_ui_begin_panel("editor panel");
		{
			if (editor->tilemap_editor.draw_panel && tic_ui_header("tilemap editor")) {
				editor->tilemap_editor.draw_panel(&editor->tilemap_editor);
				// tic_ui_end_treenode();
			}
			if (editor->tileset_editor.draw_panel && tic_ui_header("tileset editor")) {
				editor->tileset_editor.draw_panel(&editor->tileset_editor);
				// tic_ui_end_treenode();
			}
			tic_ui_end_panel();
		}
		// tic_ui_layout_row(1, &w, -64);
		tic_ui_layout_row(1, &w, -1);
		if (mu_button_ex(Core.ui.ctx, NULL, 1, MU_OPT_ALIGNCENTER)) {
			tic_ui_open_popup("window_list");
		}

		if (tic_ui_begin_popup("window_list")) {
			if (editor->tilemap_editor.cnt) tic_ui_checkbox("tilemap", &((mu_Container*)editor->tilemap_editor.cnt)->open);
			if (editor->tileset_editor.cnt) tic_ui_checkbox("tileset", &((mu_Container*)editor->tileset_editor.cnt)->open);
			if (editor->game_cnt) tic_ui_checkbox("game", &((mu_Container*)editor->game_cnt)->open);
			tic_ui_end_popup();
		}
		tic_ui_end_window();
	}
}

int tic_editor_begin_draw(tc_Editor *editor) {
	int res = tic_ui_begin_window("game", tic_rect(0, 0, 240, 190));
	if (res) {
		tic_canvas_attach(editor->game_canvas);
		if (!editor->game_cnt) editor->game_cnt = tic_ui_get_current_container();
	}
	// int res = 1;
	return res;
}

void tic_editor_end_draw(tc_Editor *editor) {
	tic_canvas_detach();
	mu_Container *cnt = tic_ui_get_current_container();
	float cw = editor->game_canvas.width;
	float ch = editor->game_canvas.height;
	// float cw = editor->tilemap_editor.canvas.width;
	// float ch = editor->tilemap_editor.canvas.height;
	float ww = cnt->body.w;
	float hh = cnt->body.h;
	float ratio = tic_min((ww-10)/cw, (hh-12)/ch);

	int i = cw*ratio;
	tic_ui_layout_row(1, &i, ch*ratio);

	// TRACELOG("%f", ratio);

	tic_ui_image(editor->game_canvas.texture, mu_vec2(0, 0), tic_rect(0, ch, cw, -ch), WHITE);
	// tic_ui_image(editor->tilemap_editor.canvas.texture, mu_vec2(0, 0), tic_rect(0, ch, cw, -ch), WHITE);

	tic_ui_end_window();

	// tic_canvas_draw(editor->game_canvas, 0, 0, WHITE);
}

/*********************
 * Tileset
 *********************/

static void editor_tileset_draw_panel(tc_TilesetEditor *editor) {
	tic_ui_layout_row(1, ((int[]){-1}), 0);
	if (editor->name) tic_ui_textbox(editor->name, strlen(editor->name));
	if (editor->filename) tic_ui_textbox(editor->filename, strlen(editor->filename));
	tic_ui_number("scale", &editor->scale, 1);
	editor->scale = tic_clamp(editor->scale, 1, 10);
	if (tic_ui_button("save")) {
		tic_editor_tileset_save(editor);
	}
}

void tic_editor_tileset_init(tc_TilesetEditor *editor) {
	editor->cell = -1;
	memset(editor->mask, 0, sizeof(int) * 9);
	editor->tileset = NULL;
	editor->image_name = NULL;
	editor->filename = NULL;
	editor->scale = 3;
	editor->cnt = NULL;
	editor->draw_panel = &editor_tileset_draw_panel;
}
void tic_editor_tileset_destroy(tc_TilesetEditor *editor) {
	tic_canvas_destroy(&editor->canvas);
}

void tic_editor_tileset_change(tc_TilesetEditor *editor, tc_Tileset *tileset) {
	tic_canvas_destroy(&editor->canvas);
	editor->tileset = tileset;
	memset(editor->mask, 0, sizeof(int) * 9);
	editor->canvas = tic_canvas_create(editor->tileset->image.width, editor->tileset->image.height);
}

static void tic_editor_tileset_draw_canvas(tc_TilesetEditor editor) {
	// tic_ui_layout_row(1, ((int[]){-1}), 0);
	tic_canvas_attach(editor.canvas);
	tic_image_draw(editor.tileset->image, 0, 0, WHITE);
	tic_canvas_detach();
}

int tic_editor_tileset_draw(tc_TilesetEditor *editor) {
	int res = 0;
	tic_canvas_attach(editor->canvas);
	// tic_texture_draw(tileset->tex, tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), WHITE);
	tic_image_draw(editor->tileset->image, 0, 0, WHITE);
	// float ww = editor->tileset->image.width / editor->tileset->tilesize.x;
	float hh = editor->tileset->image.height / editor->tileset->tilesize.y;
	float mw = editor->tileset->tilesize.x/3;
	float mh = editor->tileset->tilesize.y/3;

	int columns = editor->tileset->columns;
	for (int i = 0; i < editor->tileset->tilecount; i++) {
		float xx = fmod(i, columns) * editor->tileset->tilesize.x;
		float yy = floor(i / columns) * editor->tileset->tilesize.y;
		// int val = calc_mask(tileset->bitmask[i]);
		// TRACELOG("%d %d %d", i, tileset->bitmask[i][4], val);
		// if (val > -1) tic_graphics_fill_rectangle(xx, yy, tileset->size.x, tileset->size.y, tic_color(0, 0, 0, 125));
		int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		tic_tileset_calc_mask_array(*editor->tileset, editor->tileset->bitmasks[i], mask);
		for (int j = 0; j < 9; j++) {
			float mx = xx + (fmod(j, 3) * mw);
			float my = yy + (floor(j / 3) * mh);

			if (mask[j] > 0) tic_graphics_fill_rectangle(mx, my, mw, mh, tic_color(128, 12, 83, 175));
		}
	}
	tic_canvas_detach();


	if (tic_ui_begin_window("tileset", mu_rect(128, 64, 240, 260))) {
			if (!editor->cnt) editor->cnt = tic_ui_get_current_container();
			tic_ui_layout_row(1, ((int[]){-1}), -1);
  		// mu_layout_row(Core.ui.ctx, 2, (int[]){64, -1}, -1);

  		// tic_ui_begin_column();
  		// tic_ui_number("scale", &editor->scale, 1);
  		// editor->scale = tic_clamp(editor->scale, 1, 10);
  		// if (tic_ui_button("save")) {
  		// 	tic_editor_tileset_save(editor);
  		// }
  		// tic_ui_end_column();

  		tic_ui_begin_column();
  		mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, -1);
  		tic_ui_begin_panel("tileset_panel");

  		tc_Tileset *tileset = editor->tileset;
	  		
	  		mu_layout_row(Core.ui.ctx, 1, (int[]){tileset->image.width*editor->scale}, tileset->image.height*editor->scale);
	  		int tw = tileset->image.width / tileset->tilesize.x;
	  		int th = tileset->image.height / tileset->tilesize.y;
	  		if (tic_ui_image_grid_ex("tileset_grid", editor->canvas.texture, tic_rect(0, editor->canvas.height, editor->canvas.width, -editor->canvas.height), mu_vec2(tw, th), &editor->cell, 0)) {
	  			// memset(editor->mask, 0, sizeof(int) * 9);
	  			tic_tileset_calc_mask_array(*editor->tileset, tileset->bitmasks[editor->cell], editor->mask);
	  			tic_ui_open_popup("tile_bitmask");
	  		}


	  		if (tic_ui_begin_popup("tile_bitmask")) {
	  			int r = 96;
	  			tic_ui_layout_row(1, &r, 96);
	  			// TRACELOG("%d %d", tileset->cell, tileset->bitmask[tileset->cell][1]);
	  			// int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0};
	  			if (tic_ui_grid_ex("bitmask_grid", mu_vec2(3, 3), editor->mask, MU_OPT_INPUTDOWN | MU_OPT_GRIDMULTI)) {
	  				tileset->bitmasks[editor->cell] = tic_tileset_calc_mask(*tileset, editor->mask);
	  			}
	  			mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, 0);
	  			if (tic_ui_button("clear")) {
	  				memset(editor->mask, 0, sizeof(int) * 9);
	  				tileset->bitmasks[editor->cell] = -1;
	  			}
	  			res = 1;
	  			tic_ui_end_popup();
	  		} else {
	  			editor->cell = -1;
	  		}
	  	tic_ui_end_panel();
  		tic_ui_end_column();


  		tic_ui_end_window();
  	}
  return res;
}

void tic_editor_tileset_save(tc_TilesetEditor *editor) {
	if (editor->name) {
		// TRACELOG("%s", editor->name);
		tic_resources_tileset_store(&Core.resources, editor->name);
		return;
	}
	cJSON *root = NULL;
	if (editor->filename) root = tic_json_open(editor->filename);
	else root = tic_json_create();

	tc_Tileset *tileset = editor->tileset;
	tic_json_set_number(root, "tilewidth", tileset->tilesize.x);
	tic_json_set_number(root, "tileheight", tileset->tilesize.y);
	tic_json_set_number(root, "imagewidth", tileset->image.width);
	tic_json_set_number(root, "imageheight", tileset->image.height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "columns", tileset->columns);
	tic_json_set_number(root, "tilecount", tileset->tilecount);

	// cJSON *bitmask = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	cJSON *bitmask = tic_json_get_array(root, "bitmasks");
	if (bitmask) {
		for (int i = 0; i < tileset->tilecount; i++) {
			cJSON_DeleteItemFromArray(bitmask, 0);
		}
	} else {
		bitmask = tic_json_create_array();
		tic_json_set_array(root, "bitmasks", bitmask);
	}

	for (int i = 0; i < tileset->tilecount; i++) {
		cJSON *n = cJSON_CreateNumber(tileset->bitmasks[i]);
		cJSON_InsertItemInArray(bitmask, i, n);
	}

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);

	if (editor->filename) tic_json_save(editor->filename, root);
	else tic_json_save("tileset.json", root);
	tic_json_delete(root);
}

/**********************
 * Tilemap
 **********************/

void tic_editor_tilemap_resize(tc_TilemapEditor *editor, int w, int h) {
	if (w <= 0 || h <= 0) return;
	tic_tilemap_resize(editor->map, w, h);
	tic_canvas_destroy(&editor->canvas);
	// TRACELOG("%d %d", w, h);
	editor->canvas = tic_canvas_create(w*editor->map->grid.x, h*editor->map->grid.y);
	editor->size.x = editor->map->width;
	editor->size.y = editor->map->height;
}

static void editor_tilemap_draw_panel(tc_TilemapEditor *editor) {
	int w = -1;
	tic_ui_layout_row(1, &w, 0);
	tic_ui_number("map width", &editor->size.x, 1);
	tic_ui_number("map height", &editor->size.y, 1);
	if (tic_ui_button("set")) {
		if (editor->size.x != editor->map->width ||
			  editor->size.y != editor->map->height) {
			// tic_tilemap_resize(editor->map, editor->size.x, editor->size.y);
			tic_editor_tilemap_resize(editor, editor->size.x, editor->size.y);
		}
	}
	if (tic_ui_button("save")) {
		// tic_editor_tilemap_save(editor);
		tic_resources_tilemap_store(&Core.resources, editor->name);
	}
}

void tic_editor_tilemap_init(tc_TilemapEditor *editor) {
	editor->tileset_name = NULL;
	editor->filename = NULL;
	editor->map = NULL;
	editor->cell = -1;
	editor->scale = 1;
	editor->canvas = tic_canvas_create(16*16, 16*16);
	editor->bg = tic_canvas_create(32, 32);
	tic_texture_set_wrap(&editor->bg.texture, GL_REPEAT, GL_REPEAT);
	editor->cnt = NULL;
	editor->draw_panel = &editor_tilemap_draw_panel;
}
void tic_editor_tilemap_destroy(tc_TilemapEditor *editor) {
	tic_canvas_destroy(&editor->canvas);
}

void tic_editor_tilemap_load(const char *path) {
	
}

void tic_editor_tilemap_change(tc_TilemapEditor *editor, tc_Tilemap *tilemap) {
	editor->map = tilemap;
	tic_canvas_destroy(&editor->canvas);
	editor->canvas = tic_canvas_create(tilemap->width*tilemap->grid.x, tilemap->height*tilemap->grid.y);
	editor->size.x = tilemap->width;
	editor->size.y = tilemap->height;
}

int tic_editor_tilemap_draw(tc_TilemapEditor *editor) {
	int width,height;
	tic_window_get_size(&width, &height);
	char *title = "tilemap";
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;

	// tic_canvas_attach(editor->bg);
	// tic_graphics_fill_rectangle(0, 0, 16, 16, tic_color3(102, 59, 147));
	// tic_graphics_fill_rectangle(16, 0, 16, 16, tic_color3(139, 114, 222));
	// tic_graphics_fill_rectangle(0, 16, 16, 16, tic_color3(139, 114, 222));
	// tic_graphics_fill_rectangle(16, 16, 16, 16, tic_color3(102, 59, 147));
	// tic_canvas_detach();
	editor_set_bg(editor->bg, 16, tic_color3(102, 59, 147), tic_color3(139, 114, 222));

	tic_canvas_attach(editor->canvas);
	tic_graphics_clear(tic_color(0, 0, 0, 0));
	tic_canvas_draw_part(editor->bg, tic_rectf(0, 0, cw, ch), 0, 0, WHITE);
	tic_tilemap_draw(*editor->map);
	// tic_graphics_fill_rectangle(0, 0, 32, 32, WHITE);
	tic_canvas_detach();

	// tic_canvas_draw(editor->canvas, -32, 32, WHITE);

	if (mu_begin_window_ex(Core.ui.ctx, title, mu_rect(0, 0, width, height), (MU_OPT_NOFRONT | MU_OPT_NOFRAME | MU_OPT_NORESIZE))) {
		mu_Container *cnt = tic_ui_get_current_container();
		if (!editor->cnt) editor->cnt = cnt;
		// mu_Id id = tic_ui_get_id(title, strlen(title));
		if (Core.ui.ctx->hover_root == cnt) {
			if (tic_input_key_down(KEY_LEFT_CONTROL)) {
				float y;
				tic_input_get_mouse_scroll_delta(NULL, &y);
				if (y) editor->scale += y/10.f;
				editor->scale = tic_clamp(editor->scale, 1, 10);
			}
		}
		
		// TRACELOG("%d %d", cw, ch);

		// tic_canvas_draw(editor->canvas, -190, 0, WHITE);

		// TRACELOG("%d %d", cw, ch);
		int tesw = editor->map->width * editor->map->grid.x;
		int tesh = editor->map->height * editor->map->grid.y;

		tic_ui_layout_row(1, ((int[]){tesw*editor->scale}), tesh*editor->scale);
		// mu_Container *container = tic_ui_get_current_container();
		cnt->rect = tic_rect(Core.editor.sidebar_width, 0, Core.window.width-Core.editor.sidebar_width, Core.window.height);
		// cnt->rect = tic_rect(Core.editor.sidebar_width, 0, cw-Core.sidebar_width., ch);
		// cnt->body = tic_rect(Core.editor.sidebar_width, 0, Core.window.width-Core.editor.sidebar_width, Core.window.height);
		// cnt->rect = cnt->body;
		// cnt->rect.x = Core.editor.sidebar_width;
		// cw = cnt->rect.w;
		// ch = cnt->rect.h;
		int tw = cw / 16;
		int th = ch / 16;
		// tic_ui_image(editor->canvas.texture, mu_vec2(0, 0), mu_rect(0, ch, cw, -ch), WHITE);
		int res = tic_ui_image_grid_ex("tilemap_grid", editor->canvas.texture, mu_rect(0, ch, cw, -ch), mu_vec2(tw, th), &editor->cell, MU_OPT_INPUTDOWN);
		if (res & MU_RES_CHANGE) {
			if (res & MU_RES_MOUSELEFT) tic_tilemap_insert(editor->map, editor->cell);
			else if (res & MU_RES_MOUSERIGHT) tic_tilemap_remove(editor->map, editor->cell);
		}
		else editor->cell = -1;

		tic_ui_end_window();
	}
}

void tic_editor_tilemap_save(tc_TilemapEditor *editor) {
	cJSON *root = tic_json_create();
	tc_Tilemap *tilemap = editor->map;
	tic_json_set_number(root, "width", tilemap->width);
	tic_json_set_number(root, "height", tilemap->height);
	tic_json_set_number(root, "tilewidth", tilemap->grid.x);
	tic_json_set_number(root, "tileheight", tilemap->grid.y);
	if (editor->tileset_name) tic_json_set_string(root, "tileset", editor->tileset_name);
	tic_json_set_number(root, "count", tilemap->count);
	// tic_json_set_number(root, "tilecount", tileset->tilecount);

	cJSON *data = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	tic_json_set_array(root, "data", data);

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);
	for (int i = 0; i < tilemap->count; i++) {
		cJSON *n = cJSON_CreateNumber(tilemap->data[i]);
		cJSON_InsertItemInArray(data, i, n);
	}

	if (editor->filename) tic_json_save(editor->filename, root);
	else tic_json_save("tilemap.json", root);

	tic_json_delete(root);
}
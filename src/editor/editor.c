#include "../tico.h"

void tic_editor_load(tc_Editor *editor) {
	// tic_editor_tilemap_init(&editor->tilemap_editor, tc_Tilemap *tilemap)
}

void tic_editor_destroy(tc_Editor *editor) {
	tic_editor_tileset_destroy(&editor->tileset_editor);
	tic_editor_tilemap_destroy(&editor->tilemap_editor);
}

void tic_editor_set_tileset(tc_Editor *editor, tc_Tileset *tileset) {
	tic_editor_tileset_change(&editor->tileset_editor, tileset);
}

void tic_editor_set_tilemap(tc_Editor *editor, tc_Tilemap *tilemap) {
	tic_editor_tilemap_change(&editor->tilemap_editor, tilemap);
}

void tic_editor_new_tileset(tc_Editor *editor, int w, int h) {}
void tic_editor_new_tilemap(tc_Editor *editor, int w, int h) {}

void tic_editor_update(tc_Editor *editor) {}

void tic_editor_draw(tc_Editor *editor) {
	tic_editor_tilemap_draw(&editor->tilemap_editor);
	tic_editor_tileset_draw(&editor->tileset_editor);
}

/*********************
 * Tileset
 *********************/

TIC_API void tic_editor_tileset_init(tc_TilesetEditor *editor) {
	editor->cell = -1;
	memset(editor->mask, 0, sizeof(int) * 9);
	editor->tileset = NULL;
}
TIC_API void tic_editor_tileset_destroy(tc_TilesetEditor *editor) {
}

TIC_API void tic_editor_tileset_change(tc_TilesetEditor *editor, tc_Tileset *tileset) {
	editor->tileset = tileset;
	memset(editor->mask, 0, sizeof(int) * 9);
}

TIC_API int tic_editor_tileset_draw(tc_TilesetEditor *editor) {
	int res = 0;
	tic_canvas_attach(editor->canvas);
	// tic_texture_draw(tileset->tex, tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), WHITE);
	tic_image_draw(editor->tileset->image, 0, 0, WHITE);
	// float ww = editor->tileset->image.width / editor->tileset->tilesize.x;
	float hh = editor->tileset->image.height / editor->tileset->tilesize.y;
	float mw = editor->tileset->tilesize.x/3;
	float mh = editor->tileset->tilesize.y/3;

	int tilecount = editor->tileset->tilecount;
	for (int i = 0; i < tilecount; i++) {
		float xx = fmod(i, tilecount) * editor->tileset->tilesize.x;
		float yy = floor(i / tilecount) * editor->tileset->tilesize.y;
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


	if (tic_ui_begin_window("tileset", mu_rect(64, 64, 240, 260))) {
  		mu_layout_row(Core.ui.ctx, 2, (int[]){64, -1}, -1);

  		tic_ui_begin_column();
  		tic_ui_number("scale", &editor->scale, 1);
  		editor->scale = tic_clamp(editor->scale, 1, 10);
  		if (tic_ui_button("save")) {
  			// tic_tileset_save(*editor->tileset);
  		}
  		tic_ui_end_column();

  		tic_ui_begin_column();
  		mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, -1);
  		tic_ui_begin_panel("tileset_panel");

  		tc_Tileset *tileset = editor->tileset;
	  		
	  		mu_layout_row(Core.ui.ctx, 1, (int[]){tileset->image.width*editor->scale}, tileset->image.height*editor->scale);
	  		int tw = tileset->image.width / tileset->tilesize.x;
	  		int th = tileset->image.height / tileset->tilesize.y;
	  		if (tic_ui_image_grid_ex("tileset_grid", editor->canvas.texture, tic_rect(0, editor->canvas.height, editor->canvas.width, -editor->canvas.height), mu_vec2(tw, th), &editor->cell, 0)) {
	  			memset(editor->mask, 0, sizeof(int) * 9);
	  			tic_ui_open_popup("tile_bitmask");
	  		}


	  		if (tic_ui_begin_popup("tile_bitmask")) {
	  			int r = 96;
	  			tic_ui_layout_row(1, &r, 96);
	  			// TRACELOG("%d %d", tileset->cell, tileset->bitmask[tileset->cell][1]);
	  			// int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0};
	  			if (tic_ui_grid_ex("bitmask_grid", mu_vec2(3, 3), editor->mask, MU_OPT_GRIDMULTI)) {
	  				tileset->bitmasks[editor->cell] = tic_tileset_calc_mask(*tileset, editor->mask);
	  			}
	  			mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, 0);
	  			if (tic_ui_button("clear")) {
	  				memset(editor->mask, 0, sizeof(int) * 9);
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

/**********************
 * Tilemap
 **********************/

TIC_API void tic_editor_tilemap_init(tc_TilemapEditor *editor) {}
TIC_API void tic_editor_tilemap_destroy(tc_TilemapEditor *editor) {}

TIC_API void tic_editor_tilemap_change(tc_TilemapEditor *editor, tc_Tilemap *tilemap) {}

TIC_API int tic_editor_tilemap_draw(tc_TilemapEditor *editor) {}
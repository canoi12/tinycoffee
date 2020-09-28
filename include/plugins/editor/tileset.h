#ifndef TICO_PLUGIN_EDITOR_TILESET_H
#define TICO_PLUGIN_EDITOR_TILESET_H

// #include "definitions.h"
// #include "../editor.h"

typedef struct tc_TilesetEditor {
  tc_EditorWindow *win;
  tc_Canvas canvas;
  tc_Tileset *tileset;
	int scale;
  void (*draw_panel)(void*);
  char *name;
  char *filename;
  char *image_name;
  int cell;
  int mask[9];
  void *cnt;

	// tc_GridTool grid;
	tc_GridWidget grid;
} tc_TilesetEditor;

TIC_API tc_EditorPlugin tico_plugin_editor_tileset();
TIC_API int tico_plugin_editor_tileset_init(tc_TilesetEditor *editor, tc_EditorWindow *win);
TIC_API int tico_plugin_editor_tileset_draw(tc_TilesetEditor *editor);
TIC_API int tico_plugin_editor_tileset_draw_panel(tc_TilesetEditor *editor);
TIC_API int tico_plugin_editor_tileset_destroy(tc_TilesetEditor *editor);

#endif

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_EditorPlugin tico_plugin_editor_tileset() {
	tc_EditorPlugin plugin = {0};
	plugin.size = sizeof(tc_TilesetEditor);
	plugin.init = (EditorPluginInit)tico_plugin_editor_tileset_init;
	plugin.draw = (EditorPluginDraw)tico_plugin_editor_tileset_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_tileset_draw_panel;
	plugin.destroy = (EditorPluginDestroy)tico_plugin_editor_tileset_destroy;

	return plugin;
}

int tico_plugin_editor_tileset_init(tc_TilesetEditor *editor, tc_EditorWindow *win) {
	ASSERT(win != NULL);
	ASSERT(win->res != NULL);
	editor->cell = -1;
	memset(editor->mask, 0, sizeof(int) * 9);
	// editor->tool = tool;
	editor->tileset = NULL;
	editor->image_name = NULL;
	editor->scale = 3;
	editor->cnt = NULL;

	tc_Tileset *tileset = win->res->data;
	editor->tileset = tileset;
	editor->canvas = tico_canvas_create(tileset->image->width, tileset->image->height);
	// editor->grid = tico_tool_grid_create(tileset->tilesize.x, tileset->tilesize.y, tileset->image->width, tileset->image->height);
	editor->grid = tico_widget_grid_create(tileset->tilesize, (tc_Vec2){tileset->image->width, tileset->image->height});

	// if (tool->res) tico_editor_tileset_change(editor, tool->res->data);
	// editor->grid = tico_tool_grid_create()
	return 1;
}

int tico_plugin_editor_tileset_draw(tc_TilesetEditor *editor) {
	tc_Tileset *tileset = editor->tileset;
	// tc_Tool *t = editor->tool;
	tico_canvas_attach(editor->canvas);
	// tico_graphics_fill_rectangle(0, 0, tileset->image->width, tileset->image->height, BLACK);
	tico_graphics_clear(tico_color(0, 0, 0, 255));
	tico_image_draw(*editor->tileset->image, 0, 0, WHITE);
	float hh = editor->tileset->image->height / editor->tileset->tilesize.y;
	float mw = editor->tileset->tilesize.x/3;
	float mh = editor->tileset->tilesize.y/3;

	int columns = editor->tileset->columns;
	for (int i = 0; i < editor->tileset->tilecount; i++) {
		float xx = fmod(i, columns) * editor->tileset->tilesize.x;
		float yy = floor(i / columns) * editor->tileset->tilesize.y;
		int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		tico_tileset_calc_mask_array(*editor->tileset, editor->tileset->bitmasks[i], mask);
		for (int j = 0; j < 9; j++) {
			float mx = xx + (fmod(j, 3) * mw);
			float my = yy + (floor(j / 3) * mh);

			if (mask[j] > 0) tico_graphics_fill_rectangle(mx, my, mw, mh, tico_color(128, 12, 83, 175));
		}
	}
	tico_canvas_detach();

	igSliderInt("scale", &editor->grid.scale, 1, 10, "%d", 0);
	// tico_canvas_attach(editor->canvas);
	// tico_graphics_clear(BLACK);
	// // tico_tileset_draw(*tileset);
	// tico_image_draw(*tileset->image, 0, 0, WHITE);
	// tc_Resource *res = editor->tool->res;

	// tico_canvas_detach();
	ImVec2 size;
	igGetContentRegionAvail(&size);
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;
	int cell = -1;
	// if (igBeginChildStr("testew", size, 1, 0)) {
		// igImage(editor->canvas.texture.id, (ImVec2){cw*editor->scale, ch*editor->scale}, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
		// int res = tico_tool_grid_draw(&editor->grid, &cell, &editor->canvas, NULL);
		int res = tico_widget_grid_draw(&editor->grid, &cell, &editor->canvas.tex, (tc_Rectf){0, 1, 1, 0}, WHITE, 0);
		if (res && tico_input_mouse_pressed(MOUSE_LEFT)) {
			// TRACELOG("%d", cell);
			editor->cell = cell;
			tico_tileset_calc_mask_array(*tileset, tileset->bitmasks[editor->cell], editor->mask);
			igOpenPopup("context", 0);
		}

		if (igBeginPopup("context", ImGuiWindowFlags_NoMove)) {
			// if (editor->cell >= 0) igText("testaaaaaando, hehehehe, %d", tileset->bitmasks[editor->cell]);
			tc_Rectf rect = tico_tileset_get_rect(*tileset, editor->cell);
			tc_Rectf norm;
			ImVec2 p0;
			ImVec2 p1;
			ImVec2 mask_size;
			mask_size.x = (rect.w / tileset->image->width) / 3;
			mask_size.y = (rect.h / tileset->image->height) / 3;
			p0.x = rect.x / tileset->image->width;
			p0.y = 1 - (rect.y / tileset->image->height);
			p1.x = p0.x + (rect.w / tileset->image->width);
			p1.y = p0.y - (rect.h / tileset->image->height);
			// TRACELOG("%f %f %f %f", rect.x, rect.y, rect.w, rect.h);
			// TRACELOG("%f %f %f %f", p0.x, 1-p0.y, p1.x, 1-p1.y);
			for (int i = 0; i < 9; i++) {
				ImVec2 pp0;
				ImVec2 pp1;
				pp0.x = p0.x + (i%3) * mask_size.x;
				pp0.y = p0.y - floor(i / 3) * mask_size.y;
				if (i % 3 != 0 && i != 0) {
					igSameLine(0, 4);
					// pp0.x += (i%3) * mask_size.x;
				}
				else {
					igSpacing();
					// pp0
				}

				pp1.x = pp0.x + mask_size.x;
				pp1.y = pp0.y - mask_size.y;
				char id[8];
				sprintf(id, "##mask_%d", i);
				// igSelectableBool(id, editor->mask[i] > 0, ImGuiSelectableFlags_DontClosePopups, (ImVec2){32, 32});
				
				igImageButton(editor->canvas.tex.id, (ImVec2){32, 32}, pp0, pp1, 2, (ImVec4){1, 1, 1, 1}, (ImVec4){1, 1, 1, 1});
				int is_hovered = igIsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				if (is_hovered) {
					if (igIsMouseDragging(ImGuiMouseButton_Left, 0)) editor->mask[i] = 1;
					else if (igIsMouseDragging(ImGuiMouseButton_Right, 0)) editor->mask[i] = 0;
					tileset->bitmasks[editor->cell] = tico_tileset_calc_mask(*tileset, editor->mask);
				}
			}
			igEndPopup();
		}
}

int tico_plugin_editor_tileset_draw_panel(tc_TilesetEditor *editor) {
	// tico_ui_layout_row(1, ((int[]){-1}), 0);

	// if (tico_ui_button("save##tileset")) {
	// 	tico_editor_tileset_save(editor);
	// }
	// tico_ui_number("scale", &editor->scale, 1);
	// editor->scale = tico_clamp(editor->scale, 1, 10);
	// return 1;
}

int tico_plugin_editor_tileset_destroy(tc_TilesetEditor *editor) {
	tico_canvas_destroy(&editor->canvas);
	tico_tool_grid_destroy(&editor->grid);
}

#endif
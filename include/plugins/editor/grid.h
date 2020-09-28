#ifndef TICO_PLUGIN_EDITOR_GRID_H
#define TICO_PLUGIN_EDITOR_GRID_H

typedef struct tc_GridWidget {
  tc_Camera camera;
	// tc_Canvas canvas;
	tc_Canvas bg;
	tc_Vec2 grid;
	tc_Vec2 size;
	tc_Vec2 scrolling;
	int scale;
} tc_GridWidget;

TIC_API tc_GridWidget tico_widget_grid_create(tc_Vec2 tile, tc_Vec2 size);
TIC_API void tico_widget_grid_destroy(tc_GridWidget *widget);
TIC_API int tico_widget_grid_draw(tc_GridWidget *widget, int *cell, tc_Texture *tex, tc_Rectf uv, tc_Color color, int flags);

#endif /* TICO_PLUGIN_EDITOR_GRID_H */

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_GridWidget tico_widget_grid_create(tc_Vec2 tile, tc_Vec2 size) {
  tc_GridWidget widget = {0};
  tile.x = tico_max(1, tile.x);
  tile.y = tico_max(1, tile.y);
  size.x = tico_max(2, size.x);
  size.y = tico_max(2, size.y);

  widget.scale = 1;
  widget.scrolling = tico_vec2(0, 0);

  widget.grid = tile;
  widget.size = size;
  widget.bg = tico_canvas_create(2, 2);
  tico_texture_set_wrap(&widget.bg.tex, GL_REPEAT, GL_REPEAT);
  widget.camera = tico_camera_create(0, 0, size.x, size.y);

  return widget;
}

void tico_widget_grid_destroy(tc_GridWidget *widget) {
  tico_canvas_destroy(&widget->bg);
}

int tico_widget_grid_draw(tc_GridWidget *widget, int *cell, tc_Texture *tex, tc_Rectf uv, tc_Color color, int flags) {
  int res = 0;
	int gw, gh;
	gw = widget->grid.x * widget->scale;
	gh = widget->grid.y * widget->scale;
	ImVec2 cPos;
	igGetCursorScreenPos(&cPos);
	ImVec2 origin;
	origin.x = cPos.x + widget->scrolling.x;
	origin.y = cPos.y + widget->scrolling.y;

	int mx, my;
	tico_input_get_mouse_pos(&mx, &my);
	int cx, cy;
	cx = (mx - cPos.x) - widget->camera.area.x;
	cy = (my - cPos.y) - widget->camera.area.y;
	cx = floor((float)cx / (float)gw);
	cy = floor((float)cy / (float)gh);

	// int cw = widget->canvas.width;
	// int ch = widget->canvas.height;


	int agw, agh;
	agw = gw / widget->scale;
	agh = gh / widget->scale;
	int acx = cx * agw;
	int acy = cy * agh;

	ImVec2 size;
	igGetContentRegionAvail(&size);

  size.x = tico_max(1, size.x);
  size.y = tico_max(1, size.y);
  // TRACELOG("%f, %f", size.x, size.y);

	// return 0;


	igInvisibleButton("testandow_canvas", size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	int is_hovered = igIsItemHovered(0);
	int is_active = igIsItemActive();

	

	ImDrawList *list = igGetWindowDrawList();
	// ImDrawList_AddImage(list, widget->)
	ImVec2 pos_1;
	pos_1.x = cPos.x + size.x;
	pos_1.y = cPos.y + size.y;
	ImDrawList_AddRectFilled(list, cPos, pos_1, 0xff333333, 0, 0);
	ImDrawList_PushClipRect(list, cPos, (ImVec2){cPos.x+size.x, cPos.y+size.y}, 1);
	

	const float GRID_STEP = 64;
	for (float x = fmodf(widget->scrolling.x, GRID_STEP); x < size.x; x += GRID_STEP)
			ImDrawList_AddLine(list, (ImVec2){cPos.x + x, cPos.y}, (ImVec2){cPos.x + x, pos_1.y}, 0x33dddddd, 0);
	for (float y = fmodf(widget->scrolling.y, GRID_STEP); y < size.y; y += GRID_STEP)
			ImDrawList_AddLine(list, (ImVec2){cPos.x, cPos.y + y}, (ImVec2){pos_1.x, cPos.y + y}, 0x33dddddd, 0);
	if (tex) {
		ImVec2 canvas_p1;
		canvas_p1.x = origin.x + (tex->width * widget->scale);
		canvas_p1.y = origin.y + (tex->height * widget->scale);
		ImDrawList_AddImage(list, (ImTextureID)tex->id, origin, canvas_p1, (ImVec2){uv.x, uv.y}, (ImVec2){uv.w, uv.h}, 0xffffffff);
		ImDrawList_AddRect(list, origin, canvas_p1, 0x99ffffff, 0, 0, 2);
	}
	ImGuiIO *io = igGetIO();
	int imx = io->MousePos.x;
	int imy = io->MousePos.y;
	ImVec2 mouse_pos;
	ImVec2 grid_pos;

	// io->MouseWheel

	grid_pos.x = floor((imx - origin.x) / gw);
	grid_pos.y = floor((imy - origin.y) / gh);
	
	mouse_pos.x = grid_pos.x * gw + origin.x;
	mouse_pos.y = grid_pos.y * gh + origin.y;
	if (is_hovered) {
		ImDrawList_AddRectFilled(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0x66ffffff, 0, 0);
		// ImDrawList_AddRect(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0xff000000, 0, 0, 2);
    ImDrawList_AddRect(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0xffffffff, 4, 0xf, 2);
		if (tico_input_key_down(KEY_LEFT_ALT)) {
			tc_Vec2 delta_pos = tico_vec2(0, 0);
			tico_input_get_mouse_delta(&delta_pos.x, &delta_pos.y);

			widget->scrolling.x += io->MouseDelta.x;
			widget->scrolling.y += io->MouseDelta.y;
			// // tico_input_get_mouse_delta(&delta_pos.x, &delta_pos.y);
			// tico_
		} else if (tico_input_key_down(KEY_LEFT_CONTROL)) {
			widget->scale += io->MouseWheel;
			widget->scale = tico_clamp(widget->scale, 1, 10);
			// TRACELOG("%f", io->MouseWheel);
		}
	}

	if (is_active) {
		res = 1;
		// int index = cx + ((widget->size.x / agw) * cy);
		int index = grid_pos.x + ((widget->size.x / agw) * grid_pos.y);
		if (cell) *cell = index;
	}
	ImDrawList_PopClipRect(list);

	return res;
}

#endif
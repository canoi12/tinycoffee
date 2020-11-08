#ifndef TICO_PLUGIN_WIDGET_PANEL_H
#define TICO_PLUGIN_WIDGET_PANEL_H

typedef struct tc_PanelWidget
{
  int grid_size;
  tc_Vec2 origin;
	tc_Vec2 cursor_pos;
	tc_Vec2 size;
	tc_Canvas bg_canvas;
	tc_Color bg_colors[2];
  float scale;
  unsigned int bg_color;
	unsigned int line_color;
	unsigned int cross_color;
} tc_PanelWidget;

TIC_API tc_PanelWidget tico_widget_panel_create(int grid_size);
TIC_API void tico_widget_panel_destroy(tc_PanelWidget *widget);

TIC_API void tico_widget_panel_origin(tc_PanelWidget *widget, tc_Vec2 *out);

TIC_API int tico_widget_panel_begin(tc_PanelWidget *widget);
TIC_API void tico_widget_panel_end(tc_PanelWidget *widget);

#endif // TICO_PLUGIN_WIDGET_PANEL_H

#if defined(TICO_PLUGIN_WIDGET_IMPLEMENTATION)

tc_PanelWidget tico_widget_panel_create(int grid_size) {
  tc_PanelWidget widget = {0};
  widget.grid_size = tico_max(1, grid_size);
  // widget.bg_color = tico_color3(125, 125, 125);
  // widget.line_color = tico_color3(25, 25, 25);
	widget.bg_color = 0xff333333;
	widget.line_color = 0x33dddddd;
	widget.cross_color = 0xff0000ff;
	widget.origin = tico_vec2(0, 0);
	widget.scale = 1.f;
	widget.cursor_pos = tico_vec2(0, 0);

	widget.bg_canvas = tico_canvas_create(2, 2);
	tico_texture_set_wrap(&widget.bg_canvas.tex, GL_REPEAT, GL_REPEAT);
	widget.bg_colors[0] = tico_rgb(139, 114, 222);
	widget.bg_colors[1] = tico_rgb(102, 59, 147);

  return widget;
}

void tico_widget_panel_origin(tc_PanelWidget *widget, tc_Vec2 *out) {
	ASSERT(out != NULL);

	out->x = widget->cursor_pos.x + widget->origin.x;
	out->y = widget->cursor_pos.y + widget->origin.y;
}

int tico_widget_panel_begin(tc_PanelWidget *widget) {
  int res = 0;
	int gw, gh;
	// gw = widget->grid.x * widget->scale;
	// gh = widget->grid.y * widget->scale;
	ImVec2 cPos;
	igGetCursorScreenPos(&cPos);
	widget->cursor_pos = tico_vec2(cPos.x, cPos.y);
	ImVec2 origin;
	origin.x = cPos.x + widget->origin.x;
	origin.y = cPos.y + widget->origin.y;

	// int mx, my;
	// tico_input_get_mouse_pos(&mx, &my);
	// int cx, cy;
	// cx = (mx - cPos.x) - widget->camera.area.x;
	// cy = (my - cPos.y) - widget->camera.area.y;
	// cx = floor((float)cx / (float)gw);
	// cy = floor((float)cy / (float)gh);

	// int cw = widget->canvas.width;
	// int ch = widget->canvas.height;


	// int agw, agh;
	// agw = gw / widget->scale;
	// agh = gh / widget->scale;
	// int acx = cx * agw;
	// int acy = cy * agh;

	int ssz = 1;
  tico_canvas_attach(widget->bg_canvas);
  tico_graphics_clear(widget->bg_colors[0]);
  tico_graphics_fill_rectangle(ssz, 0, ssz, ssz, widget->bg_colors[1]);
  tico_graphics_fill_rectangle(0, ssz, ssz, ssz, widget->bg_colors[1]);
  tico_canvas_detach();
  // tico_graphics_clear(tico_rgba(0, 0, 0, 255));

	ImVec2 size;
	igGetContentRegionAvail(&size);
	widget->size.x = size.x;
	widget->size.y = size.y;

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
	ImDrawList_AddRectFilled(list, cPos, pos_1, widget->bg_color, 0, 0);
	ImDrawList_PushClipRect(list, cPos, pos_1, 1);
	

	const float GRID_STEP = widget->grid_size;

	ImVec2 tex_size = (ImVec2){size.x/(GRID_STEP*2), size.y/(GRID_STEP*2)};
	float xx = fmodf(origin.x, GRID_STEP);
	float yy = fmodf(origin.y, GRID_STEP);
	
	// ImVec2 bg_pos0 = (ImVec2){cPos.x, cPos.y};
	// ImVec2 bg_pos1 = (ImVec2){bg_pos0.x+size.x+(2*GRID_STEP), bg_pos0.y+size.y+(2*GRID_STEP)};
	ImDrawList_AddImage(list, widget->bg_canvas.tex.id, cPos, pos_1, (ImVec2){0, 0}, tex_size, 0xffffffff);
	// for (float x = fmodf(widget->origin.x, GRID_STEP); x < size.x; x += GRID_STEP)
	// 		ImDrawList_AddLine(list, (ImVec2){cPos.x + x, cPos.y}, (ImVec2){cPos.x + x, pos_1.y}, widget->line_color, 0);
	// for (float y = fmodf(widget->origin.y, GRID_STEP); y < size.y; y += GRID_STEP)
	// 		ImDrawList_AddLine(list, (ImVec2){cPos.x, cPos.y + y}, (ImVec2){pos_1.x, cPos.y + y}, widget->line_color, 0);



	int line_size = 32 * widget->scale;
	ImDrawList_AddLine(list, (ImVec2){origin.x-line_size, origin.y}, (ImVec2){origin.x+line_size, origin.y}, widget->cross_color, 0);
	ImDrawList_AddLine(list, (ImVec2){origin.x, origin.y-line_size}, (ImVec2){origin.x, origin.y+line_size}, widget->cross_color, 0);
	// if (tex) {
	// 	ImVec2 canvas_p1;
	// 	canvas_p1.x = origin.x + (tex->width * widget->scale);
	// 	canvas_p1.y = origin.y + (tex->height * widget->scale);
	// 	ImDrawList_AddImage(list, (ImTextureID)tex->id, origin, canvas_p1, (ImVec2){uv.x, uv.y}, (ImVec2){uv.w, uv.h}, 0xffffffff);
	// 	ImDrawList_AddRect(list, origin, canvas_p1, 0x99ffffff, 0, 0, 2);
	// }
	ImGuiIO *io = igGetIO();
	int imx = io->MousePos.x;
	int imy = io->MousePos.y;
	ImVec2 mouse_pos;
	// ImVec2 grid_pos;

	// // io->MouseWheel

	// grid_pos.x = floor((imx - origin.x) / gw);
	// grid_pos.y = floor((imy - origin.y) / gh);
	
	// mouse_pos.x = grid_pos.x * gw + origin.x;
	// mouse_pos.y = grid_pos.y * gh + origin.y;
	if (is_hovered) {
		// ImDrawList_AddRectFilled(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0x66ffffff, 0, 0);
		// ImDrawList_AddRect(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0xff000000, 0, 0, 2);
    // ImDrawList_AddRect(list, mouse_pos, (ImVec2){mouse_pos.x+gw, mouse_pos.y+gh}, 0xffffffff, 4, 0xf, 2);
		if (tico_input_key_down(KEY_LEFT_ALT)) {
			tc_Vec2 delta_pos = tico_vec2(0, 0);
			tico_input_get_mouse_delta(&delta_pos.x, &delta_pos.y);

			widget->origin.x += io->MouseDelta.x;
			widget->origin.y += io->MouseDelta.y;
			// // tico_input_get_mouse_delta(&delta_pos.x, &delta_pos.y);
			// tico_
		} else if (tico_input_key_down(KEY_LEFT_CONTROL)) {
			widget->scale += io->MouseWheel;
			widget->scale = tico_clamp(widget->scale, 1, 20);
			// TRACELOG("%f", io->MouseWheel);
		}
	}

	if (is_active) {
		res = 1;
		// int index = cx + ((widget->size.x / agw) * cy);
		// int index = grid_pos.x + ((widget->size.x / agw) * grid_pos.y);
		// if (cell) *cell = index;
	}
	

	return res;
}

void tico_widget_panel_end(tc_PanelWidget *widget) {
  ImDrawList *list = igGetWindowDrawList();
  ImDrawList_PopClipRect(list);
}

#endif
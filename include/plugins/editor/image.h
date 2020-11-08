#ifndef TICO_PLUGIN_EDITOR_IMAGE_H
#define TICO_PLUGIN_EDITOR_IMAGE_H

// #include "../editor.h"

typedef struct tc_ImageEditor {
	tc_EditorWindow *win;
	tc_Canvas bg;
	tc_Image *image;
	int scale;
} tc_ImageEditor;

TIC_API tc_EditorPlugin tico_plugin_editor_image();
TIC_API int tico_plugin_editor_image_init(tc_ImageEditor *editor, tc_EditorWindow *win);
TIC_API int tico_plugin_editor_image_draw(tc_ImageEditor *editor);
TIC_API int tico_plugin_editor_image_draw_panel(tc_ImageEditor *editor);
TIC_API int tico_plugin_editor_image_destroy(tc_ImageEditor *editor);

#endif

#if defined(TICO_PLUGIN_EDITOR_IMPLEMENTATION)

tc_EditorPlugin tico_plugin_editor_image() {
	tc_EditorPlugin plugin = {0};
	plugin.size = sizeof(tc_ImageEditor);
	plugin.init = (EditorPluginInit)tico_plugin_editor_image_init;
	plugin.draw = (EditorPluginDraw)tico_plugin_editor_image_draw;
	plugin.draw_panel = (EditorPluginDrawPanel)tico_plugin_editor_image_draw_panel;
	plugin.destroy = (EditorPluginDestroy)tico_plugin_editor_image_destroy;

	return plugin;
}

int tico_plugin_editor_image_init(tc_ImageEditor *editor, tc_EditorWindow *win) {
	// editor->tool = tool;
	editor->image = win->res->data;
	editor->scale = 1;
	win->flags = ImGuiWindowFlags_HorizontalScrollbar;
}

int tico_plugin_editor_image_draw(tc_ImageEditor *editor) {
	int ssz = 1;

	int w = editor->image->width * editor->scale;
	int h = editor->image->height * editor->scale;
	// tc_Resource *res = editor->tool->res;
	// ASSERT(res != NULL);
	char scale_id[16];
	sprintf(scale_id, "scale##%s", "123321");

	igSliderInt(scale_id, &editor->scale, 1, 10, "%d", 0);
	ImVec2 size;
	igGetContentRegionAvail(&size);
	if (igBeginChildStr("123321", (ImVec2){size.x, size.y}, 1, ImGuiWindowFlags_HorizontalScrollbar)) {
		igImage(editor->image->texture.id, (ImVec2){w, h}, (ImVec2){0, 0}, (ImVec2){1, 1}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
	}
	igEndChild();
}

int tico_plugin_editor_image_draw_panel(tc_ImageEditor *editor) {}

int tico_plugin_editor_image_destroy(tc_ImageEditor *editor) {

}

#endif
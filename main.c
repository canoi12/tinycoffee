#include "src/tico.h"

int main(int argc, char ** argv) {
	tc_Config config = tic_config_init(NULL, 640, 380, argc, argv);
	tic_init(&config);

	tic_main_loop();

	// tc_Image image = tic_image_load("tileset.png");
	// tc_Tileset tileset = tic_tileset_load("tileset.json", image);
	// // tc_Tileset tileset = tic_tileset_create(image, 16, 16);

	// tc_Tilemap map = tic_tilemap_create(&tileset, 64, 32);

	// tc_Editor editor;
	// tic_editor_load(&editor);
	// tic_editor_set_tileset(&editor, &tileset);
	// tic_editor_set_tilemap(&editor, &map);
	// int cell;

	// while(!tic_window_should_close()) {
	// 	tic_update();

	// 	tic_begin_draw();
	// 	tic_graphics_clear(BLACK);

	// 	// tic_image_draw(editor.tileset->image, 0, 0, WHITE);
	// 	tic_editor_draw(&editor);
	// 	// if (mu_begin_window_ex(Core.ui.ctx, "window", mu_rect(0, 0, 64, 128), MU_OPT_NOFRONT | MU_OPT_NOTITLE | MU_OPT_NORESIZE)) {
	// 	// 	tic_ui_layout_row(1, ((int[]){Core.window.width}), Core.window.height);
	// 	// 	mu_Container *container = tic_ui_get_current_container();
	// 	// 	container->rect = tic_rect(0, 0, Core.window.width, Core.window.height);
	// 	// 	int tw = Core.window.width / 16;
	// 	// 	int th = Core.window.height / 16;
	// 	// 	// tic_ui_image(editor.bg.texture, mu_vec2(0, 0), mu_rect(0, Core.window.height, Core.window.width, -Core.window.height), WHITE);
	// 	// 	tic_ui_image_grid_ex("tilemap_grid", editor.bg.texture, mu_rect(0, 0, Core.window.width, Core.window.height), mu_vec2(tw, th), &cell, MU_OPT_INPUTDOWN);
	// 	// 	mu_end_window(Core.ui.ctx);
	// 	// }

	// 	tic_end_draw();
	// }

	tic_terminate();

	return 0;
}
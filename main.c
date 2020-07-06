#include "src/tico.h"

void drawRect(float x, float y, int w, int h) {
	tic_graphics_draw_line(x, y, x+w, y, WHITE);
	tic_graphics_draw_line(x+w, y, x+w, y+h, WHITE);
	tic_graphics_draw_line(x+w, y+h, x, y+h, WHITE);
	tic_graphics_draw_line(x, y+h, x, y, WHITE);
}

int main(int argc, char ** argv) {
  tc_Config config = tic_config_init(NULL, 640, 380, argc, argv);
  tic_init(&config);

  int width, height;
  glfwGetFramebufferSize(Core.window.handle, &width, &height);
  TRACELOG("%d %d %d %d", width, height, Core.window.width, Core.window.height);

  float x = 0;
  float y = 32;

  // tc_Canvas canvas = tic_canvas_create(320, 190);
  // tc_Image image = tic_image_load("assets/images/goblin_run_anim_strip_6.png");

  tic_main_loop();
  // while(!tic_window_should_close()) {
  // 	tic_update();

  // 	tic_begin_draw();
  // 	tic_clear(tic_color3(75, 90, 90));

  // 	float dt = tic_timer_get_delta();

  // 	if (tic_input_is_joy_down(0, JOY_BUTTON_DPAD_LEFT)) x -= 180 * dt;
  // 	else if (tic_input_is_joy_down(0, JOY_BUTTON_DPAD_RIGHT)) x += 180 * dt;

  // 	tic_canvas_attach(canvas);
  // 	tic_clear(BLACK);

  // 	tic_graphics_push();
  // 	tic_graphics_translate(-x, -y);

  // 	tic_image_draw_part(image, tic_rectf(0, 0, 16, 16), x, y, WHITE);

  // 	// tic_graphics_draw_rectangle(x, y, 32, 32, WHITE);
  // 	tic_graphics_draw_rectangle(160, 90, 32, 32, WHITE);
  // 	tic_graphics_draw_rectangle(320, 160, 32, 32, WHITE);
  // 	// drawRect(x, y, 32, 32);
  // 	// drawRect(160, 90, 32, 32);
  // 	// drawRect(320, 160, 32, 32);

  // 	tic_graphics_pop();

  // 	tic_canvas_detach();

  // 	tic_canvas_draw_auto(canvas);

  // 	tic_end_draw();
  // }

  tic_terminate();

  return 0;
}

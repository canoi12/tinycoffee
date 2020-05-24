#include "src/tinycoffee.h"

// #include "src/external/zip/src/miniz.h"

int main(int argc, char ** argv) {
  tc_config config;
  tc_config_init(&config, "NULL", 640, 380);
  tc_init(&config);

  tc_texture knight = tc_load_texture("assets/knight.png");

  tc_canvas canvas = tc_create_canvas(320, 190);

  // printf("%s\n", (char*)buf);

  vec2 pos = vec2_new(64, 64);
  vec2 dt = vec2_new(0, 0);
  int flip = 1;

  tc_rectangle rect = {0, 0, 32, 32};

  while (!tc_should_close()) {
    tc_poll_events();
    tc_scripting_wren_update();

    float delta = tc_get_delta();
    dt = vec2_new(0, 0);

    if (tc_is_key_down(KEY_LEFT)) dt.x = -100 * delta;
    if (tc_is_key_down(KEY_RIGHT)) dt.x = 100 * delta;

    if (dt.x < 0) flip = -1;
    else if (dt.x > 0) flip = 1;

    vec2_add(&pos, pos, dt);

    tc_clear(BG);
    tc_begin_draw();
// //     tc_scripting_wren_draw();
    tc_set_canvas(canvas);
    tc_clear(BLACK);
    tc_draw_triangle(64, 64, 96, 0, 128, 64, RED);
    tc_fill_triangle(64, 64, 96, 0, 128, 64, WHITE);

    tc_draw_texture_part_ex(knight, rect, pos.x, pos.y, 0, flip, 1, 16, 16, WHITE);
    tc_unset_canvas();

    tc_draw_canvas_scale(canvas, 0, 0, 2, 2, WHITE);

    tc_end_draw();
  }

  return 0;
}

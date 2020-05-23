#include "src/tinycoffee.h"

// #include "src/external/zip/src/miniz.h"

int main(int argc, char ** argv) {
  tc_init("tiny coffee", 640, 380);

  // char *buffer = tc_fs_read_file_from_zip("./game.tic", "main.wren", NULL);

  // tc_texture knight = tc_load_texture("assets/knight.png");

  // printf("%s\n", (char*)buf);

  float x = 0;
  float y = 0;

  while (!tc_should_close()) {
    tc_poll_events();
    tc_scripting_wren_update();
    tc_clear(BLACK);
    tc_begin_draw();
    tc_scripting_wren_draw();
    tc_end_draw();
  }

  return 0;
}

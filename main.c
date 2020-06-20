#include "src/tinycoffee.h"

int main(int argc, char ** argv) {

  tc_Config config = tc_init_config(NULL, 640, 380, argc, argv);
  tc_init(&config);

  tc_main_loop();

  tc_terminate();
  return 0;
}

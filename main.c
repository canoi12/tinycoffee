#include "src/tico.h"

int main(int argc, char ** argv) {

  tc_Config config = tic_config_init(NULL, 640, 380, argc, argv);
  tic_init(&config);

  tic_main_loop();

  tic_terminate();

  return 0;
}

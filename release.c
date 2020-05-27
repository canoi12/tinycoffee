#include "src/tinycoffee.h"

int main(int argc, char ** argv) {

  tc_config config;
  tc_config_init(&config, "tico", 640, 380);
  tc_init(&config);

  tc_main_loop();

  tc_terminate();

  return 0;
}

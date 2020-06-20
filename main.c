#include "src/tinycoffee.h"

int main(int argc, char ** argv) {
  tc_config config;
  tc_config_init(&config, NULL, 640, 380);
  tc_init(&config);

  tc_texture tex = tc_load_texture("assets/knight.png");
  tc_canvas canvas = tc_create_canvas(320, 190);
  tc_uint8* file = tc_read_file("project.json", NULL);
  cJSON *json = cJSON_Parse(file);
  const cJSON *name = NULL;

  name = cJSON_GetObjectItem(json, "name");
  if (cJSON_IsString(name) && name->valuestring != NULL) TRACELOG("project name: %s", name->valuestring);

  tc_open_sprite(tex);
  float x = 0;
  int y = 16;

  while (!tc_should_close()) {
    tc_poll_events();
    tc_scripting_wren_update();

    tc_begin_draw();
    tc_clear(BLACK);
    tc_scripting_wren_draw();
    tc_editor_draw();
    tc_draw_circle(16, 16, 8, WHITE);
    tc_end_draw();
  }

  tc_ui_savestate();

  return 0;
}

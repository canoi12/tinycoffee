#include "module.h"

static const char* tcWrenTicoModule =
"class WindowFlags {\n"
" static NoResizable {1}\n"
"}\n"
"class Config {\n"
// " construct new() {}\n"
" foreign static title=(value)\n"
" foreign static width=(value)\n"
" foreign static height=(value)\n"
"}\n";

static tc_config *wconf;

// static void wren_config_allocate(WrenVM *vm) {
//   tc_config *config = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*config));
//   config->width = 0;
//   config->height = 0;
//   config->windowFlags = 0;
// }

// static void wren_config_finalize(void *data) {
//   free(data);
// }

static void wren_config_init(tc_config *config) {
  // wconf = malloc(sizeof(*wconf));
  // memcpy(wconf, config, sizeof(tc_config));
  wconf = config;
}

static void wren_config_title(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  const char *title = wrenGetSlotString(vm, 1);
  strcpy(wconf->title, title);
}

static void wren_config_width(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  int width = wrenGetSlotDouble(vm, 1);
  wconf->width = width;
}

static void wren_config_height(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  int height = wrenGetSlotDouble(vm, 1);
  wconf->height = height;
}

static tc_wren_lib wrenConfigLib[] = {
  {"s_title=(_)", wren_config_title},
  {"s_width=(_)", wren_config_width},
  {"s_height=(_)", wren_config_height}
};
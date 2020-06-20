#include "module.h"

static const char *tcWrenTimerModule =
"class Timer {\n"
" foreign static getFps()\n"
" foreign static getDelta()\n"
" foreign static getTime()\n"
"}\n";

/****************
 * Timer module
 ****************/

static void wren_get_delta(WrenVM *vm) {
  float delta = tc_get_delta();

  wrenSetSlotDouble(vm, 0, delta);
}

static void wren_get_fps(WrenVM *vm) {
  int fps = tc_get_fps();
  wrenSetSlotDouble(vm, 0, fps);
}

static void wren_get_time(WrenVM *vm) {
  float time = tc_get_time();
  wrenSetSlotDouble(vm, 0, time);
}

static tc_WrenLib wrenTimerLib[] = {
  {"s_getFps()", wren_get_fps},
  {"s_getDelta()", wren_get_delta},
  {"s_getTime()", wren_get_time}
};

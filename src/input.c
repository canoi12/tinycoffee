#include "tico.h"

tc_bool tic_input_init(tc_Input *input, TIC_INPUT_FLAGS flags) {
  memset(input->keyState.down, 0, sizeof(tc_bool) * KEY_LAST);
  memset(input->keyState.pressed, 0, sizeof(tc_bool) * KEY_LAST);

  memset(input->mouseState.down, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  memset(input->mouseState.pressed, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  input->keyState.keyNames = hashmap_create(48);
  input->mouseState.buttonNames = hashmap_create(3);

  struct {
    char *name;
    int code;
  } keyNames[] = {
    {"space",         32},
    {"apostrophe",    39},  /* ' */
    {"comma",         44}, /* , */
    {"minus",         45},  /* - */
    {"period",        46},  /* . */
    {"slash",         47},  /* / */
    {"0",             48},
    {"1",             49},
    {"2",             50},
    {"3",             51},
    {"4",             52},
    {"5",             53},
    {"6",             54},
    {"7",             55},
    {"8",             56},
    {"9",             57},
    {"semicolon",     59},  /* ; */
    {"equal",         61},  /* = */
    {"a",             65},
    {"b",             66},
    {"c",             67},
    {"d",             68},
    {"e",             69},
    {"f",             70},
    {"g",             71},
    {"h",             72},
    {"i",             73},
    {"j",             74},
    {"k",             75},
    {"l",             76},
    {"m",             77},
    {"n",             78},
    {"o",             79},
    {"p",             80},
    {"q",             81},
    {"r",             82},
    {"s",             83},
    {"t",             84},
    {"u",             85},
    {"v",             86},
    {"w",             87},
    {"x",             88},
    {"y",             89},
    {"z",             90},
    {"left_bracket",  91},  /* [ */
    {"backslash",     92},  /* \ */
    {"right_bracket", 93},  /* ] */
    {"grave_accent",  96},  /* ` */
    {"world_1",       161}, /* non-us #1 */
    {"world_2",       162}, /* non-us #2 */
    {"escape",        256},
    {"enter",         257},
    {"tab",           258},
    {"backspace",     259},
    {"insert",        260},
    {"delete",        261},
    {"right",         262},
    {"left",          263},
    {"down",          264},
    {"up",            265},
    {"page_up",       266},
    {"page_down",     267},
    {"home",          268},
    {"end",           269},
    {"caps_lock",     280},
    {"scroll_lock",   281},
    {"num_lock",      282},
    {"print_screen",  283},
    {"pause",         284},
    {"f1",            290},
    {"f2",            291},
    {"f3",            292},
    {"f4",            293},
    {"f5",            294},
    {"f6",            295},
    {"f7",            296},
    {"f8",            297},
    {"f9",            298},
    {"f10",           299},
    {"f11",           300},
    {"f12",           301},
    {"f13",           302},
    {"f14",           303},
    {"f15",           304},
    {"f16",           305},
    {"f17",           306},
    {"f18",           307},
    {"f19",           308},
    {"f20",           309},
    {"f21",           310},
    {"f22",           311},
    {"f23",           312},
    {"f24",           313},
    {"f25",           314},
    {"kp_0",          320},
    {"kp_1",          321},
    {"kp_2",          322},
    {"kp_3",          323},
    {"kp_4",          324},
    {"kp_5",          325},
    {"kp_6",          326},
    {"kp_7",          327},
    {"kp_8",          328},
    {"kp_9",          329},
    {"kp_decimal",    330},
    {"kp_divide",     331},
    {"kp_multiply",   332},
    {"kp_subtract",   333},
    {"kp_add",        334},
    {"kp_enter",      335},
    {"kp_equal",      336},
    {"left_shift",    340},
    {"left_control",  341},
    {"left_alt",      342},
    {"left_super",    343},
    {"right_shift",   344},
    {"right_control", 345},
    {"right_alt",     346},
    {"right_super",   347},
    {"menu",          348},
    {NULL, 0}
  };

  struct {
    char *name;
    int code;
  } mouseButtonNames[] = {
    {"mouse1", MOUSE_LEFT},
    {"mouse2", MOUSE_RIGHT},
    {"mouse3", MOUSE_MIDDLE},
    {NULL, 0}
  };

  for (int i = 0; keyNames[i].name != NULL; i++) {
    hashmap_set(&input->keyState.keyNames, keyNames[i].name, keyNames[i].code);
  }

  for (int i = 0; mouseButtonNames[i].name != NULL; i++) {
    hashmap_set(&input->mouseState.buttonNames, mouseButtonNames[i].name, mouseButtonNames[i].code);
  }
  TRACELOG("Input initiated");
}

void tic_input_destroy(tc_Input *input) {
	hashmap_clear(&input->keyState.keyNames);
	hashmap_clear(&input->mouseState.buttonNames);
	TRACELOG("Input clear");
}

void tic_input_update(tc_Input *input) {
  memcpy(input->keyState.pressed, input->keyState.down, sizeof(tc_bool) * KEY_LAST);
  memcpy(input->mouseState.pressed, input->mouseState.down, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
}

int tic_input_get_keycode(const char *name) {
  hashmap_item *item = hashmap_get(Core.input.keyState.keyNames, name);
  int code = -1;
  if (item) code = item->value;

  return code;
}

/* key functions */

tc_bool tic_input_is_key_down(TIC_KEY key) {
  key = tic_clamp(key, 0, KEY_LAST);
  return Core.input.keyState.down[key];
}

tc_bool tic_input_is_key_pressed(TIC_KEY key) {
  key = tic_clamp(key, 0, KEY_LAST);
  return !Core.input.keyState.pressed[key] && Core.input.keyState.down[key];
}

tc_bool tic_input_is_key_up(TIC_KEY key) {
  key = tic_clamp(key, 0, KEY_LAST);
  return !Core.input.keyState.down[key];
}

tc_bool tic_input_is_key_released(TIC_KEY key) {
  key = tic_clamp(key, 0, KEY_LAST);
  return Core.input.keyState.pressed[key] && !Core.input.keyState.down[key];
}

/* Mouse functions */

void tic_input_get_mouse(int *x, int *y) {
  if (x) *x = Core.input.mouseState.x;
  if (y) *y = Core.input.mouseState.y;
}

void tic_input_fix_mouse() {
  Core.input.mouseState.fixX = Core.input.mouseState.x;
  Core.input.mouseState.fixY = Core.input.mouseState.y;
  Core.input.mouseState.fixed = tc_true;
}

void tic_input_release_mouse() {
  Core.input.mouseState.fixed = tc_false;
}

void tic_input_get_mouse_delta(int *x, int *y) {
  if (x) *x = Core.input.mouseState.fixX - Core.input.mouseState.x;
  if (y) *y = Core.input.mouseState.fixY - Core.input.mouseState.y;
}

void tic_input_get_mouse_scroll(float *x, float *y) {
  if (x) *x = Core.input.mouseState.scrollX;
  if (y) *y = Core.input.mouseState.scrollY;
}

tc_bool tic_input_is_mouse_down(TIC_MOUSEBUTTON button) {
  button = tic_clamp(button, 0, MOUSE_BUTTON_LAST);
  return Core.input.mouseState.down[button];
}

tc_bool tic_input_is_mouse_pressed(TIC_MOUSEBUTTON button) {
  button = tic_clamp(button, 0, MOUSE_BUTTON_LAST);
  return !Core.input.mouseState.pressed[button] && Core.input.mouseState.down[button];
}

tc_bool tic_input_is_mouse_up(TIC_MOUSEBUTTON button) {
  button = tic_clamp(button, 0, MOUSE_BUTTON_LAST);
  return !tic_input_is_mouse_down(button);
}

tc_bool tic_input_is_mouse_released(TIC_MOUSEBUTTON button) {
  button = tic_clamp(button, 0, MOUSE_BUTTON_LAST);
  return Core.input.mouseState.pressed[button] && !Core.input.mouseState.down[button];
}


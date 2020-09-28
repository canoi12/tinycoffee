#include "tico.h"

tc_bool tico_input_init(tc_Input *input, TIC_INPUT_FLAGS_ flags) {
	input->mouseState.scroll.x = 0;
  input->mouseState.scroll.y = 0;
  input->mouseState.scroll_delta.x = 0;
  input->mouseState.scroll_delta.y = 0;

  memset(input->keyState.down, 0, sizeof(tc_bool) * KEY_LAST);
  memset(input->keyState.pressed, 0, sizeof(tc_bool) * KEY_LAST);

  memset(input->mouseState.down, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  memset(input->mouseState.pressed, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  map_init(&input->names.keyNames);
  map_init(&input->names.mouseButtonNames);
  map_init(&input->names.joyButtonNames);
  map_init(&input->names.joyAxisNames);


  for (int i = 0; i < TIC_JOY_COUNT; i++) {
    if (!glfwJoystickIsGamepad(i)) {
      input->joystickState[i].active = tc_false;
      continue;
    }

    input->joystickState[i].active = tc_true;
  }

  struct {
    char *name;
    int code;
  } joyButtonNames[] = {
    {"jp_a", JOY_BUTTON_A},
    {"jp_b", JOY_BUTTON_B},
    {"jp_x", JOY_BUTTON_X},
    {"jp_y", JOY_BUTTON_Y},
    {"jp_lb", JOY_BUTTON_LEFT_BUMPER},
    {"jp_rb", JOY_BUTTON_RIGHT_BUMPER},
    {"jp_lt", JOY_BUTTON_LEFT_THUMB},
    {"jp_rt", JOY_BUTTON_RIGHT_THUMB},
    {"jp_start", JOY_BUTTON_START},
    {"jp_back", JOY_BUTTON_BACK},
    {"jp_guide", JOY_BUTTON_GUIDE},
    {"jp_dup", JOY_BUTTON_DPAD_UP},
    {"jp_ddown", JOY_BUTTON_DPAD_DOWN},
    {"jp_dleft", JOY_BUTTON_DPAD_LEFT},
    {"jp_dright", JOY_BUTTON_DPAD_RIGHT},
    {"a", JOY_BUTTON_A},
    {"cross", JOY_BUTTON_A},
    {"b", JOY_BUTTON_B},
    {"circle", JOY_BUTTON_B},
    {"x", JOY_BUTTON_X},
    {"square", JOY_BUTTON_X},
    {"y", JOY_BUTTON_Y},
    {"triangle", JOY_BUTTON_Y},
    {"lb", JOY_BUTTON_LEFT_BUMPER},
    {"rb", JOY_BUTTON_RIGHT_BUMPER},
    {"lt", JOY_BUTTON_LEFT_THUMB},
    {"rt", JOY_BUTTON_RIGHT_THUMB},
    {"start", JOY_BUTTON_START},
    {"back", JOY_BUTTON_BACK},
    {"guide", JOY_BUTTON_GUIDE},
    {"dup", JOY_BUTTON_DPAD_UP},
    {"ddown", JOY_BUTTON_DPAD_DOWN},
    {"dleft", JOY_BUTTON_DPAD_LEFT},
    {"dright", JOY_BUTTON_DPAD_RIGHT},
    {NULL, 0}
  };

  struct
  {
    char *name;
    int count; 
  } joyAxisNames[] = {
    {"jp_leftx", JOY_AXIS_LEFT_X},
    {"jp_lefty", JOY_AXIS_LEFT_Y},
    {"jp_rightx", JOY_AXIS_RIGHT_X},
    {"jp_righty", JOY_AXIS_RIGHT_X},
    {"jp_lefttrigger", JOY_AXIS_LEFT_TRIGGER},
    {"jp_righttrigger", JOY_AXIS_RIGHT_TRIGGER},
    {"leftx", JOY_AXIS_LEFT_X},
    {"lefty", JOY_AXIS_LEFT_Y},
    {"rightx", JOY_AXIS_RIGHT_X},
    {"righty", JOY_AXIS_RIGHT_Y},
    {"lefttrigger", JOY_AXIS_LEFT_TRIGGER},
    {"righttrigger", JOY_AXIS_RIGHT_TRIGGER},
    {NULL, 0}
  };

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
    {"shift",         340},
    {"ctrl",          341},
    {"alt",           342},
    {"super",         343},
    {"rshift",        344},
    {"rctrl",         345},
    {"ralt",          346},
    {"lsuper",        347},
    {"menu",          348},
    {NULL,              0}
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
    map_set(&input->names.keyNames, keyNames[i].name, keyNames[i].code);
  }

  for (int i = 0; mouseButtonNames[i].name != NULL; i++) {
    map_set(&input->names.mouseButtonNames, mouseButtonNames[i].name, mouseButtonNames[i].code);
  }

  for (int i = 0; joyButtonNames[i].name != NULL; i++) {
    map_set(&input->names.joyButtonNames, joyButtonNames[i].name, joyButtonNames[i].code);
  }
  
  LOG("Input Module initiated");
  return tc_true;
}

void tico_input_terminate(tc_Input *input) {
	map_deinit_(&input->names.keyNames.base);
  map_deinit_(&input->names.mouseButtonNames.base);
  map_deinit_(&input->names.joyButtonNames.base);
	LOG("Input Module terminated");
}

void INTERNAL_API(tico_input_update_mouse_scroll, tc_Input *input, float x, float y) {
  input->mouseState.scroll.x = x * -10;
  input->mouseState.scroll.y = y * -10;
  input->mouseState.scroll_delta.x += x * -10;
  input->mouseState.scroll_delta.y += y * -10;
}
void INTERNAL_API(tico_input_update_key, tc_Input *input, TIC_KEY_ key, int action) {
  input->keyState.down[key] = tico_clamp(action, 0, 1);
}
void INTERNAL_API(tico_input_update_mouse_pos, tc_Input *input, int x, int y) {
  input->mouseState.x = x;
  input->mouseState.y = y;
}
void INTERNAL_API(tico_input_update_joy_button, tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button, int action) {

}
void INTERNAL_API(tico_input_update_mouse_button, tc_Input *input, TIC_MOUSEBUTTON_ button, int action) {
  button = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  input->mouseState.down[button] = action;
}


int tico_input_update(tc_Input *input) {
  memcpy(input->keyState.pressed, input->keyState.down, sizeof(tc_bool) * KEY_LAST);
  memcpy(input->mouseState.pressed, input->mouseState.down, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  GLFWgamepadstate state;
  for (int i = 0; i < TIC_JOY_COUNT; i++) {
    if (!glfwJoystickIsGamepad(i)) continue;

    if (glfwGetGamepadState(i, &state)) {
      memcpy(input->joystickState[i].pressed, input->joystickState[i].down, sizeof(tc_bool) * JOY_BUTTON_COUNT);
      for (int btn = 0; btn < JOY_BUTTON_COUNT; btn++) input->joystickState[i].down[btn] = state.buttons[btn];
      memcpy(input->joystickState[i].axis, state.axes, sizeof(tc_bool) * JOY_AXIS_COUNT);
    }
  }
  input->mouseState.scroll_delta = tico_vec2(0, 0);
  input->mouseState.pos_delta.x = input->mouseState.x;
  input->mouseState.pos_delta.y = input->mouseState.y;

  return 1;
}

int tico_input_get_key_code_internal(tc_Input *input, const char *name) {
  // hashmap_item *item = hashmap_get(Core.input.names.keyNames, name);
  int *val = map_get(&input->names.keyNames, name);
  int code = -1;
  if (val) code = *val;

  return code;
}

int tico_input_get_mouse_code_internal(tc_Input *input, const char *name) {
  // hashmap_item *item = hashmap_get(Core.input.names.mouseButtonNames, name);
  int *val = map_get(&input->names.mouseButtonNames, name);
  int code = -1;
  if (val) code = *val;

  return code;
}

int tico_input_get_joy_btncode_internal(tc_Input *input, const char *name) {
  int *val = map_get(&input->names.joyButtonNames, name);
  int code = -1;
  if (val) code = *val;

  return code;
}

int tico_input_get_joy_axiscode_internal(tc_Input *input, const char *name) {
  // hashmap_item *item = hashmap_get(Core.input.names.joyAxisNames, name);
  int *val = map_get(&input->names.joyAxisNames, name);
  int code = -1;
  if (val) code = *val;

  return code;
}


/*****************
 * Key functions
 *****************/

tc_bool tico_input_key_down_internal(tc_Input *input, TIC_KEY_ key) {
  if (key < 0) return tc_false;
  key = tico_clamp(key, 0, KEY_LAST);
  return input->keyState.down[key];
}

tc_bool tico_input_key_pressed_internal(tc_Input *input, TIC_KEY_ key) {
  key = tico_clamp(key, 0, KEY_LAST);
  return !input->keyState.pressed[key] && input->keyState.down[key];
}

tc_bool tico_input_key_up_internal(tc_Input *input, TIC_KEY_ key) {
  key = tico_clamp(key, 0, KEY_LAST);
  return !input->keyState.down[key];
}

tc_bool tico_input_key_released_internal(tc_Input *input, TIC_KEY_ key) {
  key = tico_clamp(key, 0, KEY_LAST);
  return input->keyState.pressed[key] && !input->keyState.down[key];
}

/******************** 
 * Mouse functions 
 ********************/

void tico_input_get_mouse_pos_internal(tc_Input *input, int *x, int *y) {
  if (x) *x = input->mouseState.x;
  if (y) *y = input->mouseState.y;
}

void tico_input_fix_mouse_internal(tc_Input *input) {
  input->mouseState.fixX = input->mouseState.x;
  input->mouseState.fixY = input->mouseState.y;
  input->mouseState.fixed = tc_true;
}

void tico_input_release_mouse_internal(tc_Input *input) {
  input->mouseState.fixed = tc_false;
}

void tico_input_get_mouse_delta_internal(tc_Input *input, int *x, int *y) {
  // if (x) *x = Core.input.mouseState.fixX - Core.input.mouseState.x;
  // if (y) *y = Core.input.mouseState.fixY - Core.input.mouseState.y;
  if (x) *x = input->mouseState.pos_delta.x - input->mouseState.x;
  if (y) *y = input->mouseState.pos_delta.y - input->mouseState.y;

}

void tico_input_get_mouse_scroll_internal(tc_Input *input, float *x, float *y) {
  if (x) *x = input->mouseState.scroll.x;
  if (y) *y = input->mouseState.scroll.y;
}

void tico_input_get_mouse_scroll_delta_internal(tc_Input *input, float *x, float *y) {
  if (x) *x = input->mouseState.scroll_delta.x;
  if (y) *y = input->mouseState.scroll_delta.y;
}

tc_bool tico_input_mouse_down_internal(tc_Input *input, TIC_MOUSEBUTTON_ button) {
  button = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  return input->mouseState.down[button];
}

tc_bool tico_input_mouse_pressed_internal(tc_Input *input, TIC_MOUSEBUTTON_ button) {
  button = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  return !input->mouseState.pressed[button] && input->mouseState.down[button];
}

tc_bool tico_input_mouse_up_internal(tc_Input *input, TIC_MOUSEBUTTON_ button) {
  button = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  return !tico_input_mouse_down_internal(input, button);
}

tc_bool tico_input_mouse_released_internal(tc_Input *input, TIC_MOUSEBUTTON_ button) {
  button = tico_clamp(button, 0, MOUSE_BUTTON_LAST);
  return input->mouseState.pressed[button] && !input->mouseState.down[button];
}

/***********************
 * Joystick functions
 ***********************/

tc_bool tico_input_is_joy_active_internal(tc_Input *input, TIC_JOYSTICKS_ jid) {
  return input->joystickState[jid].active;
}

tc_bool tico_input_joy_down_internal(tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  if (jid > TIC_JOY_COUNT) return tc_false;

  return tico_input_is_joy_active_internal(input, jid) && input->joystickState[jid].down[button];
}

tc_bool tico_input_joy_up_internal(tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  if (jid > TIC_JOY_COUNT) return tc_false;

  return tico_input_is_joy_active_internal(input, jid) && !input->joystickState[jid].down[button];
}

tc_bool tico_input_joy_pressed_internal(tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  if (jid > TIC_JOY_COUNT) return tc_false;

  tc_bool pressed = !input->joystickState[jid].pressed[button] && input->joystickState[jid].down[button];
  return tico_input_is_joy_active_internal(input, jid) && pressed;
}

tc_bool tico_input_joy_released_internal(tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button) {
  if (jid > TIC_JOY_COUNT) return tc_false;

  tc_bool released = input->joystickState[jid].pressed[button] && !input->joystickState[jid].down[button];
  return tico_input_is_joy_active_internal(input, jid) && released;
}
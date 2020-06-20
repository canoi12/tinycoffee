#ifndef TICO_INPUT_H
#define TICO_INPUT_H

#include "tinycoffee.h"

typedef enum {
  KEY_UNKNOWN =       -1,
  KEY_SPACE =         32,
  KEY_APOSTROPHE =    39,  /* ' */
  KEY_COMMA =         44, /* , */
  KEY_MINUS =         45,  /* - */
  KEY_PERIOD =        46,  /* . */
  KEY_SLASH =         47,  /* / */
  KEY_0 =             48,
  KEY_1 =             49,
  KEY_2 =             50,
  KEY_3 =             51,
  KEY_4 =             52,
  KEY_5 =             53,
  KEY_6 =             54,
  KEY_7 =             55,
  KEY_8 =             56,
  KEY_9 =             57,
  KEY_SEMICOLON =     59,  /* ; */
  KEY_EQUAL =         61,  /* = */
  KEY_A =             65,
  KEY_B =             66,
  KEY_C =             67,
  KEY_D =             68,
  KEY_E =             69,
  KEY_F =             70,
  KEY_G =             71,
  KEY_H =             72,
  KEY_I =             73,
  KEY_J =             74,
  KEY_K =             75,
  KEY_L =             76,
  KEY_M =             77,
  KEY_N =             78,
  KEY_O =             79,
  KEY_P =             80,
  KEY_Q =             81,
  KEY_R =             82,
  KEY_S =             83,
  KEY_T =             84,
  KEY_U =             85,
  KEY_V =             86,
  KEY_W =             87,
  KEY_X =             88,
  KEY_Y =             89,
  KEY_Z =             90,
  KEY_LEFT_BRACKET =  91,  /* [ */
  KEY_BACKSLASH =     92,  /* \ */
  KEY_RIGHT_BRACKET = 93,  /* ] */
  KEY_GRAVE_ACCENT =  96,  /* ` */
  KEY_WORLD_1 =       161, /* non-US #1 */
  KEY_WORLD_2 =       162, /* non-US #2 */

/* Function keys */
  KEY_ESCAPE =        256,
  KEY_ENTER =         257,
  KEY_TAB =           258,
  KEY_BACKSPACE =     259,
  KEY_INSERT =        260,
  KEY_DELETE =        261,
  KEY_RIGHT =         262,
  KEY_LEFT =          263,
  KEY_DOWN =          264,
  KEY_UP =            265,
  KEY_PAGE_UP =       266,
  KEY_PAGE_DOWN =     267,
  KEY_HOME =          268,
  KEY_END =           269,
  KEY_CAPS_LOCK =     280,
  KEY_SCROLL_LOCK =   281,
  KEY_NUM_LOCK =      282,
  KEY_PRINT_SCREEN =  283,
  KEY_PAUSE =         284,
  KEY_F1 =            290,
  KEY_F2 =            291,
  KEY_F3 =            292,
  KEY_F4 =            293,
  KEY_F5 =            294,
  KEY_F6 =            295,
  KEY_F7 =            296,
  KEY_F8 =            297,
  KEY_F9 =            298,
  KEY_F10 =           299,
  KEY_F11 =           300,
  KEY_F12 =           301,
  KEY_F13 =           302,
  KEY_F14 =           303,
  KEY_F15 =           304,
  KEY_F16 =           305,
  KEY_F17 =           306,
  KEY_F18 =           307,
  KEY_F19 =           308,
  KEY_F20 =           309,
  KEY_F21 =           310,
  KEY_F22 =           311,
  KEY_F23 =           312,
  KEY_F24 =           313,
  KEY_F25 =           314,
  KEY_KP_0 =          320,
  KEY_KP_1 =          321,
  KEY_KP_2 =          322,
  KEY_KP_3 =          323,
  KEY_KP_4 =          324,
  KEY_KP_5 =          325,
  KEY_KP_6 =          326,
  KEY_KP_7 =          327,
  KEY_KP_8 =          328,
  KEY_KP_9 =          329,
  KEY_KP_DECIMAL =    330,
  KEY_KP_DIVIDE =     331,
  KEY_KP_MULTIPLY =   332,
  KEY_KP_SUBTRACT =   333,
  KEY_KP_ADD =        334,
  KEY_KP_ENTER =      335,
  KEY_KP_EQUAL =      336,
  KEY_LEFT_SHIFT =    340,
  KEY_LEFT_CONTROL =  341,
  KEY_LEFT_ALT =      342,
  KEY_LEFT_SUPER =    343,
  KEY_RIGHT_SHIFT =   344,
  KEY_RIGHT_CONTROL = 345,
  KEY_RIGHT_ALT =     346,
  KEY_RIGHT_SUPER =   347,
  KEY_MENU =          348,

  KEY_LAST = KEY_MENU
} TC_KEY;

typedef enum {
  MOUSE_LEFT =   0,
  MOUSE_RIGHT =  1,
  MOUSE_MIDDLE = 2,
  MOUSE_BUTTON_LAST =   3
} TC_MOUSEBUTTON;

typedef enum {
  JOY_DPAD_LEFT = 0
} TC_JOYSTICK;

typedef enum {
  TC_INPUT_INIT_ALL = 0,
  TC_INIT_KEYBOARD = 1,
  TC_INIT_MOUSE = 1 << 2,
  TC_INIT_JOYSTICK = 1 << 3
} TC_INPUT_FLAGS;

typedef struct {
  struct {
    int x;
    int y;
    int fixX;
    int fixY;
    float scrollX;
    float scrollY;
    tc_bool down[MOUSE_BUTTON_LAST];
    tc_bool pressed[MOUSE_BUTTON_LAST];
    tc_bool fixed;
    tc_bool active;
  } mouseState;
  struct {
    tc_bool down[KEY_LAST];
    tc_bool pressed[KEY_LAST];
    tc_bool active;
    hashmap keyNames;
  } keyState;
  struct {
    tc_bool down[4];
    tc_bool pressed[4];
    float axis[2];
    tc_bool active;
  } joystickState[4];
} tc_Input;

TCDEF tc_bool tc_init_input(tc_Input *input, TC_INPUT_FLAGS flags);
TCDEF void tc_destroy_input(tc_Input *input);

TCDEF void tc_update_input(tc_Input *input);
TCDEF int tc_get_keycode(const char *name);

TCDEF tc_bool tc_is_key_down(TC_KEY key);
TCDEF tc_bool tc_is_key_pressed(TC_KEY key);
TCDEF tc_bool tc_is_key_up(TC_KEY key);
TCDEF tc_bool tc_is_key_released(TC_KEY key);

TCDEF tc_bool tc_is_mouse_down(TC_MOUSEBUTTON button);
TCDEF tc_bool tc_is_mouse_pressed(TC_MOUSEBUTTON button);
TCDEF tc_bool tc_is_mouse_up(TC_MOUSEBUTTON button);
TCDEF tc_bool tc_is_mouse_released(TC_MOUSEBUTTON button);
TCDEF void tc_get_mouse(int *x, int *y);
TCDEF void tc_fix_mouse();
TCDEF void tc_release_mouse();
TCDEF void tc_get_mouse_delta(int *x, int *y);


#endif /* TICO_INPUT_H */

#if defined(TICO_INPUT_IMPLEMENTATION)

TCDEF tc_bool tc_init_input(tc_Input *input, TC_INPUT_FLAGS flags) {
  memset(input->keyState.down, 0, sizeof(tc_bool) * KEY_LAST);
  memset(input->keyState.pressed, 0, sizeof(tc_bool) * KEY_LAST);

  memset(input->mouseState.down, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  memset(input->mouseState.pressed, 0, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
  input->keyState.keyNames = hashmap_create(48);

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

  for (int i = 0; keyNames[i].name != NULL; i++) {
    hashmap_set(&input->keyState.keyNames, keyNames[i].name, keyNames[i].code);
  }

}

TCDEF void tc_update_input(tc_Input *input) {
  memcpy(input->keyState.pressed, input->keyState.down, sizeof(tc_bool) * KEY_LAST);
  memcpy(input->mouseState.pressed, input->mouseState.down, sizeof(tc_bool) * MOUSE_BUTTON_LAST);
}

TCDEF int tc_get_keycode(const char *name) {
  hashmap_item *item = hashmap_get(Core.input.keyState.keyNames, name);
  int code = -1;
  if (item) code = item->value;

  return code;
}

/* key functions */

TCDEF tc_bool tc_is_key_down(TC_KEY key) {
  return Core.input.keyState.down[key];
}

TCDEF tc_bool tc_is_key_pressed(TC_KEY key) {
  return !Core.input.keyState.pressed[key] && Core.input.keyState.down[key];
}

TCDEF tc_bool tc_is_key_up(TC_KEY key) {
  return !Core.input.keyState.down[key];
}

TCDEF tc_bool tc_is_key_released(TC_KEY key) {
  return Core.input.keyState.pressed[key] && !Core.input.keyState.down[key];
}

/* Mouse functions */

TCDEF void tc_get_mouse(int *x, int *y) {
  if (x) *x = Core.input.mouseState.x;
  if (y) *y = Core.input.mouseState.y;
}

TCDEF void tc_fix_mouse() {
  Core.input.mouseState.fixX = Core.input.mouseState.x;
  Core.input.mouseState.fixY = Core.input.mouseState.y;
  Core.input.mouseState.fixed = tc_true;
}

TCDEF void tc_release_mouse() {
  Core.input.mouseState.fixed = tc_false;
}

TCDEF void tc_get_mouse_delta(int *x, int *y) {
  if (x) *x = Core.input.mouseState.fixX - Core.input.mouseState.x;
  if (y) *y = Core.input.mouseState.fixY - Core.input.mouseState.y;
}

TCDEF void tc_get_mouse_scroll(float *x, float *y) {
  if (x) *x = Core.input.mouseState.scrollX;
  if (y) *y = Core.input.mouseState.scrollY;
}

TCDEF tc_bool tc_is_mouse_down(TC_MOUSEBUTTON button) {
  return Core.input.mouseState.down[button];
}

TCDEF tc_bool tc_is_mouse_pressed(TC_MOUSEBUTTON button) {
  return !Core.input.mouseState.pressed[button] && Core.input.mouseState.down[button];
}

TCDEF tc_bool tc_is_mouse_up(TC_MOUSEBUTTON button) {
  return !tc_is_mouse_down(button);
}

TCDEF tc_bool tc_is_mouse_released(TC_MOUSEBUTTON button) {
  return Core.input.mouseState.pressed[button] && !Core.input.mouseState.down[button];
}

#endif

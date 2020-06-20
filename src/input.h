#ifndef TC_INPUT_H
#define TC_INPUT_H

#include "tinycoffee.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef enum {
  TC_INPUT_INIT_ALL = 0,
  TC_INIT_KEYBOARD = 1 << 0,
  TC_INIT_MOUSE = 1 << 1,
  TC_INIT_JOYSTICK = 1 << 2
} TC_INPUT_FLAGS_;

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
} TC_KEYBOARD_KEY_;

typedef enum {
  MOUSE_BUTTON_LEFT =   0,
  MOUSE_BUTTON_RIGHT =  1,
  MOUSE_BUTTON_MIDDLE = 2,
  MOUSE_BUTTON_LAST =   3
} TC_MOUSE_BUTTON_;

typedef struct {
  struct {
    tc_bool buttonDown[MOUSE_BUTTON_LAST];
    tc_bool prevButton[MOUSE_BUTTON_LAST];
    int x;
    int y;
    double fixX;
    double fixY;
    float scrollX;
    float scrollY;
    tc_bool fixed;
    tc_bool active;
  } mouseState;
  struct {
    tc_bool keyDown[KEY_LAST];
		tc_bool prevKey[KEY_LAST];
		tc_bool active;
  } keyboardState;
} tc_input;

TCDEF tc_input tc_init_input(TC_INPUT_FLAGS_ flags);
TCDEF void tc_input_poll(tc_input *input);

TCDEF tc_bool tc_input_key_down(tc_input input, TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_input_key_pressed(tc_input input, TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_input_key_up(tc_input input, TC_KEYBOARD_KEY_ key);
TCDEF tc_bool tc_input_key_released(tc_input input, TC_KEYBOARD_KEY_ key);

TCDEF tc_bool tc_input_mouse_down(tc_input input, TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_input_mouse_pressed(tc_input input, TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_input_mouse_up(tc_input input, TC_MOUSE_BUTTON_ button);
TCDEF tc_bool tc_input_mouse_released(tc_input input, TC_MOUSE_BUTTON_ button);

TCDEF void tc_input_get_mouse_pos(tc_input input, int *x, int *y);
TCDEF void tc_input_get_mouse_posv(tc_input input, vec2 *mousePos);

TCDEF void tc_input_fix_mouse_pos(tc_input *input);
TCDEF void tc_input_unfix_mouse_pos(tc_input *input);
TCDEF void tc_input_get_mouse_delta(tc_input input, int *x, int *y);
TCDEF void tc_input_get_mouse_deltav(tc_input input, vec2 *deltaPos);

#endif /* TC_INPU_H */

#if defined(TC_INPUT_IMPLEMENTATION)

TCDEF tc_input tc_init_input(TC_INPUT_FLAGS_ flags) {
  tc_input input = {0};
  memset(input.keyboardState.keyDown, 0, KEY_LAST);
	memset(input.mouseState.buttonDown, 0, MOUSE_BUTTON_LAST);
	input.mouseState.x = 0;
	input.mouseState.y = 0;
	input.mouseState.fixX = 0;
	input.mouseState.fixY = 0;
	input.mouseState.fixed = 0;
	if (flags == TC_INPUT_INIT_ALL) {
	  input.keyboardState.active = tc_true;
	  input.mouseState.active = tc_true;
	}

	if (flags & TC_INIT_KEYBOARD) input.keyboardState.active = tc_true;
	if (flags & TC_INIT_MOUSE) input.mouseState.active = tc_true;

  return input;
}

TCDEF void tc_input_poll(tc_input *input) {
  for (int i = 0; i < KEY_LAST; i++)
		input->keyboardState.prevKey[i] = input->keyboardState.keyDown[i];

	for (int i = 0; i < MOUSE_BUTTON_LAST; i++)
		input->mouseState.prevButton[i] = input->mouseState.buttonDown[i];
}

TCDEF tc_bool tc_input_key_down(tc_input input, TC_KEYBOARD_KEY_ key) {
  if (!input.keyboardState.active)
  return input.keyboardState.keyDown[key];
}
TCDEF tc_bool tc_input_key_pressed(tc_input input, TC_KEYBOARD_KEY_ key) {
  return !input.keyboardState.prevKey[key] && input.keyboardState.keyDown[key];
}
TCDEF tc_bool tc_input_key_up(tc_input input, TC_KEYBOARD_KEY_ key) {
  return !input.keyboardState.keyDown[key];
}
TCDEF tc_bool tc_input_key_released(tc_input input, TC_KEYBOARD_KEY_ key) {
  return input.keyboardState.prevKey[key] && !input.keyboardState.keyDown[key];
}

TCDEF tc_bool tc_input_mouse_down(tc_input input, TC_MOUSE_BUTTON_ button) {
  return input.mouseState.buttonDown[button];
}
TCDEF tc_bool tc_input_mouse_pressed(tc_input input, TC_MOUSE_BUTTON_ button) {
  return !input.mouseState.prevButton[button] && input.mouseState.buttonDown[button];
}
TCDEF tc_bool tc_input_mouse_up(tc_input input, TC_MOUSE_BUTTON_ button) {
  return !input.mouseState.buttonDown[button];
}
TCDEF tc_bool tc_input_mouse_released(tc_input input, TC_MOUSE_BUTTON_ button) {
  return input.mouseState.prevButton[button] && !input.mouseState.buttonDown[button];
}

TCDEF void tc_input_get_mouse_pos(tc_input input, int *x, int *y) {
  if (x) *x = input.mouseState.x;
  if (y) *y = input.mouseState.y;
}
TCDEF void tc_input_get_mouse_posv(tc_input input, vec2 *mousePos) {
  if (mousePos) {
    mousePos->x = input.mouseState.x;
    mousePos->y = input.mouseState.y;
  }
}

TCDEF void tc_input_fix_mouse_pos(tc_input *input) {
  ASSERT(input != NULL);
  input->mouseState.fixX = input->mouseState.x;
  input->mouseState.fixY = input->mouseState.y;
  input->mouseState.fixed = tc_true;
}
TCDEF void tc_input_unfix_mouse_pos(tc_input *input) {
  ASSERT(input != NULL);
  input->mouseState.fixed = tc_false;
}
TCDEF void tc_input_get_mouse_delta(tc_input input, int *x, int *y) {
  if (x) *x = input.mouseState.fixX - input.mouseState.x;
  if (y) *y = input.mouseState.fixY - input.mouseState.y;
}
TCDEF void tc_input_get_mouse_deltav(tc_input input, vec2 *deltaPos) {
  if (deltaPos) {
    deltaPos->x = input.mouseState.fixX - input.mouseState.x;
    deltaPos->y = input.mouseState.fixY - input.mouseState.y;
  }
}

#endif /* TC_INPUT_IMPLEMENTATION */

#ifndef TICO_INPUT_H
#define TICO_INPUT_H

#include "tico.h"

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

  KEY_LAST = KEY_MENU,
  KEY_COUNT
} TIC_KEY_;

typedef enum {
  MOUSE_LEFT =   0,
  MOUSE_RIGHT =  1,
  MOUSE_MIDDLE = 2,
  MOUSE_BUTTON_LAST = 3,
  MOUSE_BUTTON_COUNT
} TIC_MOUSEBUTTON_;

typedef enum {
  TIC_JOY_1  = 0,
  TIC_JOY_2  = 1,
  TIC_JOY_3  = 2,
  TIC_JOY_4  = 3,
  TIC_JOY_5  = 4,
  TIC_JOY_6  = 5,
  TIC_JOY_7  = 6,
  TIC_JOY_8  = 7,
  TIC_JOY_9  = 8,
  TIC_JOY_10 = 9,
  TIC_JOY_11 = 10,
  TIC_JOY_12 = 11,
  TIC_JOY_13 = 12,
  TIC_JOY_14 = 13,
  TIC_JOY_15 = 14,
  TIC_JOY_16 = 15,
  TIC_JOY_COUNT,
  TIC_JOY_LAST = TIC_JOY_16,
} TIC_JOYSTICKS_;

typedef enum {
  JOY_BUTTON_A =             0,
  JOY_BUTTON_B =             1,
  JOY_BUTTON_X =             2,
  JOY_BUTTON_Y =             3,
  JOY_BUTTON_LEFT_BUMPER =   4,
  JOY_BUTTON_RIGHT_BUMPER =  5,
  JOY_BUTTON_BACK =          6,
  JOY_BUTTON_START =         7,
  JOY_BUTTON_GUIDE =         8,
  JOY_BUTTON_LEFT_THUMB =    9,
  JOY_BUTTON_RIGHT_THUMB =   10,
  JOY_BUTTON_DPAD_UP =       11,
  JOY_BUTTON_DPAD_RIGHT =    12,
  JOY_BUTTON_DPAD_DOWN =     13,
  JOY_BUTTON_DPAD_LEFT =     14,
  JOY_BUTTON_COUNT,
  JOY_BUTTON_LAST =          JOY_BUTTON_DPAD_LEFT,
} TIC_JOYSTICK_BUTTON_;

typedef enum {
  JOY_AXIS_LEFT_X  =         0,
  JOY_AXIS_LEFT_Y =          1,
  JOY_AXIS_RIGHT_X =         2,
  JOY_AXIS_RIGHT_Y =         3,
  JOY_AXIS_LEFT_TRIGGER =    4,
  JOY_AXIS_RIGHT_TRIGGER =   5,
  JOY_AXIS_COUNT,
  JOY_AXIS_LAST =            JOY_AXIS_RIGHT_TRIGGER,
} TIC_JOYSTICK_AXES_;

typedef enum {
  TIC_INPUT_INIT_ALL = 0,
  TIC_INIT_KEYBOARD = 1,
  TIC_INIT_MOUSE = 1 << 2,
  TIC_INIT_JOYSTICK = 1 << 3
} TIC_INPUT_FLAGS_;

typedef struct {
  struct {
    map_int_t mouseButtonNames;
    map_int_t keyNames;
    map_int_t joyButtonNames;
    map_int_t joyAxisNames;
  } names;
  struct {
    int x;
    int y;
    int fixX;
    int fixY;
    float scrollX;
    float scrollY;
    tc_Vec2 scroll;
    tc_Vec2 scroll_delta;
    tc_Vec2 pos;
    tc_Vec2 pos_delta;
    tc_bool down[MOUSE_BUTTON_LAST];
    tc_bool pressed[MOUSE_BUTTON_LAST];
    tc_bool fixed;
    tc_bool active;
  } mouseState;
  struct {
    tc_bool down[KEY_LAST];
    tc_bool pressed[KEY_LAST];
    tc_bool active;
  } keyState;
  struct {
    tc_bool down[JOY_BUTTON_COUNT];
    tc_bool pressed[JOY_BUTTON_COUNT];
    float axis[JOY_AXIS_COUNT];
    tc_bool active;
  } joystickState[TIC_JOY_COUNT];
} tc_Input;

TIC_API tc_bool tico_input_init(tc_Input *input, TIC_INPUT_FLAGS_ flags);
TIC_API void tico_input_terminate(tc_Input *input);
TIC_API int tico_input_update(tc_Input *input);


/******************
 * Internal API
 ******************/

void INTERNAL_API(tico_input_update_mouse_scroll, tc_Input *input, float x, float y);
void INTERNAL_API(tico_input_update_key, tc_Input *input, TIC_KEY_ key, int action);
void INTERNAL_API(tico_input_update_mouse_pos, tc_Input *input, int x, int y);
void INTERNAL_API(tico_input_update_joy_button, tc_Input *input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button, int action);
void INTERNAL_API(tico_input_update_mouse_button, tc_Input *input, TIC_MOUSEBUTTON_ button, int action);

int INTERNAL_API(tico_input_get_key_code, tc_Input* input, const char *name);
int INTERNAL_API(tico_input_get_joy_btncode, tc_Input* input, const char *name);
int INTERNAL_API(tico_input_get_joy_axiscode, tc_Input* input, const char *name);
int INTERNAL_API(tico_input_get_mouse_code, tc_Input* input, const char *name);

tc_bool INTERNAL_API(tico_input_key_down, tc_Input* input, TIC_KEY_ key);
tc_bool INTERNAL_API(tico_input_key_pressed, tc_Input* input, TIC_KEY_ key);
tc_bool INTERNAL_API(tico_input_key_up, tc_Input* input, TIC_KEY_ key);
tc_bool INTERNAL_API(tico_input_key_released, tc_Input* input, TIC_KEY_ key);

tc_bool INTERNAL_API(tico_input_mouse_down, tc_Input* input, TIC_MOUSEBUTTON_ button);
tc_bool INTERNAL_API(tico_input_mouse_pressed, tc_Input* input, TIC_MOUSEBUTTON_ button);
tc_bool INTERNAL_API(tico_input_mouse_up, tc_Input* input, TIC_MOUSEBUTTON_ button);
tc_bool INTERNAL_API(tico_input_mouse_released, tc_Input* input, TIC_MOUSEBUTTON_ button);
void INTERNAL_API(tico_input_mouse_set_cursor, tc_Input* input, int cursor);

tc_bool INTERNAL_API(tico_input_joy_down, tc_Input* input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
tc_bool INTERNAL_API(tico_input_joy_up, tc_Input* input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
tc_bool INTERNAL_API(tico_input_joy_pressed, tc_Input* input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);
tc_bool INTERNAL_API(tico_input_joy_released, tc_Input* input, TIC_JOYSTICKS_ jid, TIC_JOYSTICK_BUTTON_ button);

void INTERNAL_API(tico_input_get_mouse_pos, tc_Input* input, int *x, int *y);
void INTERNAL_API(tico_input_fix_mouse, tc_Input* input);
void INTERNAL_API(tico_input_release_mouse, tc_Input* input);
void INTERNAL_API(tico_input_get_mouse_delta, tc_Input* input, int *x, int *y);
void INTERNAL_API(tico_input_get_mouse_scroll, tc_Input* input, float *x, float *y);
void INTERNAL_API(tico_input_get_mouse_scroll_delta, tc_Input* input, float *x, float *y);

#endif // TICO_INPUT_H
#include "../../tico.h"

static void tic_wren_joystick_is_down(WrenVM *vm) {
	int jid = wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);

	int code = tic_input_get_joy_btncode(name);
	int down = tic_input_joy_down(jid, code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_joystick_is_pressed(WrenVM *vm) {
	int jid = wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);

	int code = tic_input_get_joy_btncode(name);
	int down = tic_input_joy_pressed(jid, code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_joystick_is_up(WrenVM *vm) {
	int jid = wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);

	int code = tic_input_get_joy_btncode(name);
	int down = tic_input_joy_up(jid, code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_joystick_is_released(WrenVM *vm) {
	int jid = wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);

	int code = tic_input_get_joy_btncode(name);
	int down = tic_input_joy_released(jid, code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_joystick_left_axis(WrenVM *vm) {

}

static void tic_wren_joystick_get_name(WrenVM *vm) {
	int jid = wrenGetSlotDouble(vm, 1);

	const char *name = glfwGetGamepadName(jid);
	if (!name) wrenSetSlotString(vm, 0, "none");
	else wrenSetSlotString(vm, 0, name);
}

static void tic_wren_joystick_right_axis(WrenVM *vm) {}

static tc_WrenLib wrenJoystickLib[] = {
	{"s_isDown(_,_)", tic_wren_joystick_is_down},
	{"s_isPressed(_,_)", tic_wren_joystick_is_pressed},
	{"s_isUp(_,_)", tic_wren_joystick_is_up},
	{"s_isReleased(_,_)", tic_wren_joystick_is_released},
	{"s_leftAxis(_,_)", tic_wren_joystick_left_axis},
	{"s_rightAxis(_,_)", tic_wren_joystick_right_axis},
	{"s_getName(_)", tic_wren_joystick_get_name},
};

static void tic_wren_mouse_is_down(WrenVM *vm) {
	int code = wrenGetSlotDouble(vm, 1);

	int down = tic_input_mouse_down(code - 1);

	wrenSetSlotBool(vm, 0, down);
}
static void tic_wren_mouse_is_pressed(WrenVM *vm) {
	int code = wrenGetSlotDouble(vm, 1);

	int down = tic_input_mouse_pressed(code - 1);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_mouse_is_up(WrenVM *vm) {
	int code = wrenGetSlotDouble(vm, 1);

	int down = tic_input_mouse_up(code - 1);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_mouse_is_released(WrenVM *vm) {
	int code = wrenGetSlotDouble(vm, 1);

	int down = tic_input_mouse_released(code - 1);

	wrenSetSlotBool(vm, 0, down);
}

static tc_WrenLib wrenMouseLib[] = {
	{"s_isDown(_)", tic_wren_mouse_is_down},
	{"s_isPressed(_)", tic_wren_mouse_is_pressed},
	{"s_isUp(_)", tic_wren_mouse_is_up},
	{"s_isReleased(_)", tic_wren_mouse_is_released},
};

static void tic_wren_keyboard_is_down(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);

	int code = tic_input_get_key_code(name);
	int down = tic_input_key_down(code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_keyboard_is_pressed(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);

	int code = tic_input_get_key_code(name);
	int down = tic_input_key_pressed(code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_keyboard_is_up(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);

	int code = tic_input_get_key_code(name);
	int down = tic_input_key_up(code);

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_keyboard_is_released(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);

	int code = tic_input_get_key_code(name);
	int down = tic_input_key_released(code);

	wrenSetSlotBool(vm, 0, down);
}

static tc_WrenLib wrenKeyboardLib[] = {
	{"s_isDown(_)", tic_wren_keyboard_is_down},
	{"s_isPressed(_)", tic_wren_keyboard_is_pressed},
	{"s_isUp(_)", tic_wren_keyboard_is_up},
	{"s_isReleased(_)", tic_wren_keyboard_is_released}
};

static void tic_wren_input_is_down(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	int down = -1;
	if (strstr(name, "mouse")) {
		int code = tic_input_get_mouse_code(name);
		down = tic_input_mouse_down(code);
	} else if (strstr(name, "jp")) {
		int jid = 0;
		if (wrenGetSlotCount(vm) > 2) jid = wrenGetSlotDouble(vm, 2);
		int code = tic_input_get_joy_btncode(name);
		down = tic_input_joy_down(jid, code);
	} else {
		int code = tic_input_get_key_code(name);
		down = tic_input_key_down(code);
	}

	wrenSetSlotBool(vm, 0, down);
}

static void tic_wren_input_is_pressed(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	int pressed = -1;
	if (strstr("mouse", name)) {
		int code = tic_input_get_mouse_code(name);
		pressed = tic_input_mouse_pressed(code);
	} else if (strstr("jp", name)) {
		int jid = 0;
		if (wrenGetSlotCount(vm) > 2) jid = wrenGetSlotDouble(vm, 2);
		int code = tic_input_get_joy_btncode(name);
		pressed = tic_input_joy_pressed(jid, code);
	} else {
		int code = tic_input_get_key_code(name);
		pressed = tic_input_key_pressed(code);
	}

	wrenSetSlotBool(vm, 0, pressed);
}

static void tic_wren_input_is_up(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	int up = -1;
	if (strstr("mouse", name)) {
		int code = tic_input_get_mouse_code(name);
		up = tic_input_mouse_up(code);
	} else if (strstr("jp", name)) {
		int jid = 0;
		if (wrenGetSlotCount(vm) > 2) jid = wrenGetSlotDouble(vm, 2);
		int code = tic_input_get_joy_btncode(name);
		up = tic_input_joy_up(jid, code);
	} else {
		int code = tic_input_get_key_code(name);
		up = tic_input_key_up(code);
	}

	wrenSetSlotBool(vm, 0, up);
}

static void tic_wren_input_is_released(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	int released = -1;
	if (strstr("mouse", name)) {
		int code = tic_input_get_mouse_code(name);
		released = tic_input_mouse_released(code);
	} else if (strstr("jp", name)) {
		int jid = 0;
		if (wrenGetSlotCount(vm) > 2) jid = wrenGetSlotDouble(vm, 2);
		int code = tic_input_get_joy_btncode(name);
		released = tic_input_joy_released(jid, code);
	} else {
		int code = tic_input_get_key_code(name);
		released = tic_input_key_released(code);
	}

	wrenSetSlotBool(vm, 0, released);
}

static tc_WrenLib wrenInputLib[] = {
	{"s_isDown(_)", tic_wren_input_is_down},
	{"s_isPressed(_)", tic_wren_input_is_pressed},
	{"s_isUp(_)", tic_wren_input_is_up},
	{"s_isReleased(_)", tic_wren_input_is_released},
	{"s_isDown(_,_)", tic_wren_input_is_down},
	{"s_isPressed(_,_)", tic_wren_input_is_pressed},
	{"s_isUp(_,_)", tic_wren_input_is_up},
	{"s_isReleased(_,_)", tic_wren_input_is_released},
};
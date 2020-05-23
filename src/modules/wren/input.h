#include "module.h"

static const char *tcWrenInputModule =
"class MouseButton {\n"
" static LEFT { 0}\n"
" static RIGHT { 1}\n"
" static MIDDLE {2}\n"
"}\n"
"class Key {\n"
" static /*KEY_*/UNKNOWN {       -1}\n"
" static /*KEY_*/SPACE {         32}\n"
" static /*KEY_*/APOSTROPHE {    39}\n"
" static /*KEY_*/COMMA {         44}\n"
" static /*KEY_*/MINUS {         45}\n"
" static /*KEY_*/PERIOD {        46}\n"
" static /*KEY_*/SLASH {         47}\n"
" static /*KEY_*/D0 {             48}\n"
" static /*KEY_*/D1 {             49}\n"
" static /*KEY_*/D2 {             50}\n"
" static /*KEY_*/D3 {             51}\n"
" static /*KEY_*/D4 {             52}\n"
" static /*KEY_*/D5 {             53}\n"
" static /*KEY_*/D6 {             54}\n"
" static /*KEY_*/D7 {             55}\n"
" static /*KEY_*/D8 {             56}\n"
" static /*KEY_*/D9 {             57}\n"
" static /*KEY_*/SEMICOLON {     59}\n"
" static /*KEY_*/EQUAL {         61}\n"
" static /*KEY_*/A {             65}\n"
" static /*KEY_*/B {             66}\n"
" static /*KEY_*/C {             67}\n"
" static /*KEY_*/D {             68}\n"
" static /*KEY_*/E {             69}\n"
" static /*KEY_*/F {             70}\n"
" static /*KEY_*/G {             71}\n"
" static /*KEY_*/H {             72}\n"
" static /*KEY_*/I {             73}\n"
" static /*KEY_*/J {             74}\n"
" static /*KEY_*/K {             75}\n"
" static /*KEY_*/L {             76}\n"
" static /*KEY_*/M {             77}\n"
" static /*KEY_*/N {             78}\n"
" static /*KEY_*/O {             79}\n"
" static /*KEY_*/P {             80}\n"
" static /*KEY_*/Q {             81}\n"
" static /*KEY_*/R {             82}\n"
" static /*KEY_*/S {             83}\n"
" static /*KEY_*/T {             84}\n"
" static /*KEY_*/U {             85}\n"
" static /*KEY_*/V {             86}\n"
" static /*KEY_*/W {             87}\n"
" static /*KEY_*/X {             88}\n"
" static /*KEY_*/Y {             89}\n"
" static /*KEY_*/Z {             90}\n"
" static /*KEY_*/LEFT_BRACKET {  91}\n"
" static /*KEY_*/BACKSLASH {     92}\n"
" static /*KEY_*/RIGHT_BRACKET { 93}\n"
" static /*KEY_*/GRAVE_ACCENT {  96}\n"
" static /*KEY_*/WORLD_1 {       161}\n"
" static /*KEY_*/WORLD_2 {       162}\n"
" static /*KEY_*/ESCAPE {        256}\n"
" static /*KEY_*/ENTER {         257}\n"
" static /*KEY_*/TAB {           258}\n"
" static /*KEY_*/BACKSPACE {     259}\n"
" static /*KEY_*/INSERT {        260}\n"
" static /*KEY_*/DELETE {        261}\n"
" static /*KEY_*/RIGHT {         262}\n"
" static /*KEY_*/LEFT {          263}\n"
" static /*KEY_*/DOWN {          264}\n"
" static /*KEY_*/UP {            265}\n"
" static /*KEY_*/PAGE_UP {       266}\n"
" static /*KEY_*/PAGE_DOWN {     267}\n"
" static /*KEY_*/HOME {          268}\n"
" static /*KEY_*/END {           269}\n"
" static /*KEY_*/CAPS_LOCK {     280}\n"
" static /*KEY_*/SCROLL_LOCK {   281}\n"
" static /*KEY_*/NUM_LOCK {      282}\n"
" static /*KEY_*/PRINT_SCREEN {  283}\n"
" static /*KEY_*/PAUSE {         284}\n"
" static /*KEY_*/F1 {            290}\n"
" static /*KEY_*/F2 {            291}\n"
" static /*KEY_*/F3 {            292}\n"
" static /*KEY_*/F4 {            293}\n"
" static /*KEY_*/F5 {            294}\n"
" static /*KEY_*/F6 {            295}\n"
" static /*KEY_*/F7 {            296}\n"
" static /*KEY_*/F8 {            297}\n"
" static /*KEY_*/F9 {            298}\n"
" static /*KEY_*/F10 {           299}\n"
" static /*KEY_*/F11 {           300}\n"
" static /*KEY_*/F12 {           301}\n"
" static /*KEY_*/F13 {           302}\n"
" static /*KEY_*/F14 {           303}\n"
" static /*KEY_*/F15 {           304}\n"
" static /*KEY_*/F16 {           305}\n"
" static /*KEY_*/F17 {           306}\n"
" static /*KEY_*/F18 {           307}\n"
" static /*KEY_*/F19 {           308}\n"
" static /*KEY_*/F20 {           309}\n"
" static /*KEY_*/F21 {           310}\n"
" static /*KEY_*/F22 {           311}\n"
" static /*KEY_*/F23 {           312}\n"
" static /*KEY_*/F24 {           313}\n"
" static /*KEY_*/F25 {           314}\n"
" static /*KEY_*/KP_0 {          320}\n"
" static /*KEY_*/KP_1 {          321}\n"
" static /*KEY_*/KP_2 {          322}\n"
" static /*KEY_*/KP_3 {          323}\n"
" static /*KEY_*/KP_4 {          324}\n"
" static /*KEY_*/KP_5 {          325}\n"
" static /*KEY_*/KP_6 {          326}\n"
" static /*KEY_*/KP_7 {          327}\n"
" static /*KEY_*/KP_8 {          328}\n"
" static /*KEY_*/KP_9 {          329}\n"
" static /*KEY_*/KP_DECIMAL {    330}\n"
" static /*KEY_*/KP_DIVIDE {     331}\n"
" static /*KEY_*/KP_MULTIPLY {   332}\n"
" static /*KEY_*/KP_SUBTRACT {   333}\n"
" static /*KEY_*/KP_ADD {        334}\n"
" static /*KEY_*/KP_ENTER {      335}\n"
" static /*KEY_*/KP_EQUAL {      336}\n"
" static /*KEY_*/LEFT_SHIFT {    340}\n"
" static /*KEY_*/LEFT_CONTROL {  341}\n"
" static /*KEY_*/LEFT_ALT {      342}\n"
" static /*KEY_*/LEFT_SUPER {    343}\n"
" static /*KEY_*/RIGHT_SHIFT {   344}\n"
" static /*KEY_*/RIGHT_CONTROL { 345}\n"
" static /*KEY_*/RIGHT_ALT {     346}\n"
" static /*KEY_*/RIGHT_SUPER {   347}\n"
" static /*KEY_*/MENU {          348}\n"
"}\n"
"class Input {\n"
" foreign static isKeyDown(key)\n"
" foreign static isKeyPressed(key)\n"
" foreign static isKeyUp(key)\n"
" foreign static isKeyReleased(key)\n"
" foreign static isMouseDown(button)\n"
" foreign static isMousePressed(button)\n"
" foreign static isMouseUp(button)\n"
" foreign static isMouseReleased(button)\n"
"}\n";

/******************
 * Input module
 ******************/

static void wren_is_key_down(WrenVM *vm)
{
	int key = wrenGetSlotDouble(vm, 1);
	wrenSetSlotBool(vm, 0, tc_is_key_down(key));
}

static void wren_is_key_pressed(WrenVM *vm)
{
	int key = wrenGetSlotDouble(vm, 1);
	wrenSetSlotBool(vm, 0, tc_is_key_pressed(key));
}

static void wren_is_key_up(WrenVM *vm) {
  int key = wrenGetSlotDouble(vm, 1);
  wrenSetSlotBool(vm, 0, tc_is_key_up(key));
}

static void wren_is_key_released(WrenVM *vm) {
  int key = wrenGetSlotDouble(vm, 1);
  wrenSetSlotBool(vm, 0, tc_is_key_released(key));
}

static void wren_is_mouse_down(WrenVM *vm)
{
	int button = wrenGetSlotDouble(vm, 1);
	int down = tc_is_mouse_down(button);
	wrenSetSlotBool(vm, 0, down);
}

static void wren_is_mouse_pressed(WrenVM *vm)
{
	int button = wrenGetSlotDouble(vm, 1);
	int down = tc_is_mouse_pressed(button);
	wrenSetSlotBool(vm, 0, down);
}

static void wren_is_mouse_up(WrenVM *vm) {
  int button = wrenGetSlotDouble(vm, 1);
  int up = tc_is_mouse_up(button);
  wrenSetSlotBool(vm, 0, up);
}

static void wren_is_mouse_released(WrenVM *vm) {
  int button = wrenGetSlotDouble(vm, 1);
  wrenSetSlotBool(vm, 0, tc_is_mouse_released(button));
}

static tc_wren_lib wrenInputLib[] = {
		{"s_isMouseDown(_)", wren_is_mouse_down},
		{"s_isMousePressed(_)", wren_is_mouse_pressed},
    {"s_isMouseUp(_)", wren_is_mouse_up},
    {"s_isMouseReleased(_)", wren_is_mouse_released},
    {"s_isKeyUp(_)", wren_is_key_up},
    {"s_isKeyReleased(_)", wren_is_key_released},
		{"s_isKeyPressed(_)", wren_is_key_pressed},
		{"s_isKeyDown(_)", wren_is_key_down},
		{NULL, NULL}};

#include "module.h"

static const char *tcWrenInputModule =
"class MouseButton {\n"
" static LEFT { 0}\n"
" static RIGHT { 1}\n"
" static MIDDLE {2}\n"
"}\n"
"class Key {\n"
" static Unknown {       -1}\n"
" static Space {         32}\n"
" static Apostrophe {    39}\n"
" static Comma {         44}\n"
" static Minus {         45}\n"
" static Period {        46}\n"
" static Slash {         47}\n"
" static D0 {             48}\n"
" static D1 {             49}\n"
" static D2 {             50}\n"
" static D3 {             51}\n"
" static D4 {             52}\n"
" static D5 {             53}\n"
" static D6 {             54}\n"
" static D7 {             55}\n"
" static D8 {             56}\n"
" static D9 {             57}\n"
" static Semicolon {     59}\n"
" static Equal {         61}\n"
" static A {             65}\n"
" static B {             66}\n"
" static C {             67}\n"
" static D {             68}\n"
" static E {             69}\n"
" static F {             70}\n"
" static G {             71}\n"
" static H {             72}\n"
" static I {             73}\n"
" static J {             74}\n"
" static K {             75}\n"
" static L {             76}\n"
" static M {             77}\n"
" static N {             78}\n"
" static O {             79}\n"
" static P {             80}\n"
" static Q {             81}\n"
" static R {             82}\n"
" static S {             83}\n"
" static T {             84}\n"
" static U {             85}\n"
" static V {             86}\n"
" static W {             87}\n"
" static X {             88}\n"
" static Y {             89}\n"
" static Z {             90}\n"
" static Left_Bracket {  91}\n"
" static Backslash {     92}\n"
" static Right_Bracket { 93}\n"
" static Grave_Accent {  96}\n"
" static World_1 {       161}\n"
" static World_2 {       162}\n"
" static Escape {        256}\n"
" static Enter {         257}\n"
" static Tab {           258}\n"
" static Backspace {     259}\n"
" static Insert {        260}\n"
" static Delete {        261}\n"
" static Right {         262}\n"
" static Left {          263}\n"
" static Down {          264}\n"
" static Up {            265}\n"
" static Page_Up {       266}\n"
" static Page_Down {     267}\n"
" static Home {          268}\n"
" static End {           269}\n"
" static Caps_Lock {     280}\n"
" static Scroll_Lock {   281}\n"
" static Num_Lock {      282}\n"
" static Print_Screen {  283}\n"
" static Pause {         284}\n"
" static F1 {            290}\n"
" static F2 {            291}\n"
" static F3 {            292}\n"
" static F4 {            293}\n"
" static F5 {            294}\n"
" static F6 {            295}\n"
" static F7 {            296}\n"
" static F8 {            297}\n"
" static F9 {            298}\n"
" static F10 {           299}\n"
" static F11 {           300}\n"
" static F12 {           301}\n"
" static F13 {           302}\n"
" static F14 {           303}\n"
" static F15 {           304}\n"
" static F16 {           305}\n"
" static F17 {           306}\n"
" static F18 {           307}\n"
" static F19 {           308}\n"
" static F20 {           309}\n"
" static F21 {           310}\n"
" static F22 {           311}\n"
" static F23 {           312}\n"
" static F24 {           313}\n"
" static F25 {           314}\n"
" static KP_0 {          320}\n"
" static KP_1 {          321}\n"
" static KP_2 {          322}\n"
" static KP_3 {          323}\n"
" static KP_4 {          324}\n"
" static KP_5 {          325}\n"
" static KP_6 {          326}\n"
" static KP_7 {          327}\n"
" static KP_8 {          328}\n"
" static KP_9 {          329}\n"
" static KP_Decimal {    330}\n"
" static KP_Divide {     331}\n"
" static KP_Multiply {   332}\n"
" static KP_Subtract {   333}\n"
" static KP_Add {        334}\n"
" static KP_Enter {      335}\n"
" static KP_Equal {      336}\n"
" static Left_Shift {    340}\n"
" static Left_Control {  341}\n"
" static Left_Alt {      342}\n"
" static Left_Super {    343}\n"
" static Right_Shift {   344}\n"
" static Right_Control { 345}\n"
" static Right_Alt {     346}\n"
" static Right_Super {   347}\n"
" static Menu {          348}\n"
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

static tc_WrenLib wrenInputLib[] = {
		{"s_isMouseDown(_)", wren_is_mouse_down},
		{"s_isMousePressed(_)", wren_is_mouse_pressed},
    {"s_isMouseUp(_)", wren_is_mouse_up},
    {"s_isMouseReleased(_)", wren_is_mouse_released},
    {"s_isKeyUp(_)", wren_is_key_up},
    {"s_isKeyReleased(_)", wren_is_key_released},
		{"s_isKeyPressed(_)", wren_is_key_pressed},
		{"s_isKeyDown(_)", wren_is_key_down},
		{NULL, NULL}};

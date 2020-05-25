#include "module.h"

static const char *tcWrenMathModule =
"foreign class Vector2 {\n"
" construct new(x, y) {}\n"
" foreign x\n"
" foreign y\n"
// " foreign =(other)\n"
" foreign +(other)\n"
" foreign -(other)\n"
" foreign *(value)\n"
" foreign /(value)\n"
" foreign x=(value)\n"
" foreign y=(value)\n"
" foreign dot(other)\n"
" foreign static zero()\n"
"}\n"
"class Transform {\n"
" position { _position }\n"
" position=(value) { _position = value }\n"
" scale { _size }\n"
" scale=(value) { _size = value }\n"
" angle { _angle }\n"
" angle=(value) { _angle = value }\n"
" construct new(x, y) {\n"
"  _position = Vector2.new(x, y)\n"
"  _size = Vector2.new(1,1)\n"
"  _angle = 0\n"
" }\n"
"}\n";

WrenHandle *vec2Handle;

static void wren_init_vec2(WrenVM *vm) {
  wrenEnsureSlots(vm, 1);
  // wrenInterpret(vm, "teste", "import \"tico.math\"");
  wrenGetVariable(vm, "tico.math", "Vector2", 0);
  vec2Handle = wrenGetSlotHandle(vm, 0);
}

// Vector2 class
static void wren_vec2_allocate(WrenVM *vm) {
  vec2 *vec = (vec2*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(*vec));
  vec->x = 0;
  vec->y = 0;
  if (wrenGetSlotCount(vm) > 1) {
    float x = wrenGetSlotDouble(vm, 1);
    float y = wrenGetSlotDouble(vm, 2);
    vec->x = x;
    vec->y = y;
  }
}

static void wren_vec2_finalize(void *data) {
  free(data);
}

static void wren_vec2_x(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, v->x);
}

static void wren_vec2_y(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, v->y);
}

static void wren_vec2_add(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  vec2 *u = wrenGetSlotForeign(vm, 1);

  wrenSetSlotHandle(vm, 1, vec2Handle);
  vec2 *w = (vec2*)wrenSetSlotNewForeign(vm, 0, 1, sizeof(vec2));
  vec2_add(w, *v, *u);
}

static void wren_vec2_sub(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  vec2 *u = wrenGetSlotForeign(vm, 1);

  wrenSetSlotHandle(vm, 1, vec2Handle);
  vec2 *w = wrenSetSlotNewForeign(vm, 0, 1, sizeof(vec2));
  vec2_sub(w, *v, *u);
}

static void wren_vec2_mul(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  float val = wrenGetSlotDouble(vm, 1);

  wrenSetSlotHandle(vm, 1, vec2Handle);
  vec2 *w = wrenSetSlotNewForeign(vm, 0, 1, sizeof(vec2));
  vec2_mul(w, *v, val);
}

static void wren_vec2_div(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  float val = wrenGetSlotDouble(vm, 1);

  wrenSetSlotHandle(vm, 1, vec2Handle);
  vec2 *w = wrenSetSlotNewForeign(vm, 0, 1, sizeof(vec2));
  vec2_div(w, *v, val);
}

static void wren_vec2_copy(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  vec2 *u = wrenGetSlotForeign(vm, 1);

  v->x = u->x;
  v->y = u->y;
}

static void wren_vec2_x_setter(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  float x = wrenGetSlotDouble(vm, 1);

  v->x = x;
}

static void wren_vec2_y_setter(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  float y = wrenGetSlotDouble(vm, 1);

  v->y = y;
}

static void wren_vec2_dot(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  vec2 *u = wrenGetSlotForeign(vm, 1);

  float dot = vec2_dot(*v, *u);

  wrenSetSlotDouble(vm, 0, dot);
}

static void wren_vec2_length(WrenVM *vm) {
  vec2 *v = wrenGetSlotForeign(vm, 0);
  float length = vec2_length(*v);

  wrenSetSlotDouble(vm, 0, length);
}

static void wren_vec2_zero(WrenVM *vm) {
  vec2 *v = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*v));
  v->x = 0;
  v->y = 0;
}

static tc_wren_lib wrenVector2Lib[] = {
  {"x", wren_vec2_x},
  {"y", wren_vec2_y},
//   {"=(_)", wren_vec2_copy},
  {"x=(_)", wren_vec2_x_setter},
  {"y=(_)", wren_vec2_y_setter},
  {"+(_)", wren_vec2_add},
  {"-(_)", wren_vec2_sub},
  {"*(_)", wren_vec2_mul},
  {"/(_)", wren_vec2_div},
  {"dot(_)", wren_vec2_dot},
  {"length()", wren_vec2_length},
  {"s_zero()", wren_vec2_zero}
};

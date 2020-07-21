#include "../../tico.h"

static void tic_wren_math_rad(WrenVM *vm) {
    float a = wrenGetSlotDouble(vm, 1);

    float rad = deg2rad(a);

    wrenSetSlotDouble(vm, 0, rad);
}

static void tic_wren_math_deg(WrenVM *vm) {
    float a = wrenGetSlotDouble(vm, 1);

    float deg = rad2deg(a);

    wrenSetSlotDouble(vm, 0, deg);
}

static tc_WrenLib wrenMathLib[] = {
    {"s_rad(_)", tic_wren_math_rad},
    {"s_deg(_)", tic_wren_math_deg}
};
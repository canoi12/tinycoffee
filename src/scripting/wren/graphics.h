#include "../../tico.h"

/***************
 * Image
 ***************/

static void tic_wren_image_allocate(WrenVM *vm) {
  tc_Image *image = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*image));

  const char *filename = wrenGetSlotString(vm, 1);

  *image = tic_image_load(filename);

}
static void tic_wren_image_finalize(void *data) {
  tic_image_destroy((tc_Image*)data);
  TIC_FREE(data);
}
static void tic_wren_image_width(WrenVM *vm) {
  tc_Image *image = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, image->width);
}

static void tic_wren_image_height(WrenVM *vm) {
  tc_Image *image = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, image->height);
}

static void tic_wren_image_size(WrenVM *vm) {
  tc_Image *image = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 1, image->width);
  wrenSetSlotDouble(vm, 2, image->height);
  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 1);
  wrenInsertInList(vm, 0, -1, 2);
}

static void tic_wren_image_draw(WrenVM *vm) {
  tc_Image *image = wrenGetSlotForeign(vm, 0);

  float x = wrenGetSlotDouble(vm, 1);
  float y = wrenGetSlotDouble(vm, 2);
  tc_Color color = wrenSlotOptColor(vm, 3, WHITE);

  tic_image_draw(*image, x, y, color);
}

static tc_WrenLib wrenImageLib[] = {
  {"draw(_,_)", tic_wren_image_draw},
  {"draw(_,_,_)", tic_wren_image_draw},
  {"draw(_,_,_,_,_,_,_)", tic_wren_image_draw},
  {"draw(_,_,_,_,_,_,_,_)", tic_wren_image_draw},
  {"getWidth()", tic_wren_image_width},
  {"getHeight()", tic_wren_image_height},
  {"getSize()", tic_wren_image_size},
};

/***************
 * Canvas
 ***************/

static void tic_wren_canvas_allocate(WrenVM *vm) {
  tc_Canvas *canvas = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*canvas));
  int width = wrenGetSlotDouble(vm, 1);
  int height = wrenGetSlotDouble(vm, 2);

  *canvas = tic_canvas_create(width, height);
}
static void tic_wren_canvas_finalize(void *data) {
  tic_canvas_destroy((tc_Canvas*)data);

  TIC_FREE(data);
}
static void tic_wren_canvas_width(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, canvas->width);

}
static void tic_wren_canvas_height(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 0, canvas->height);
}

static void tic_wren_canvas_size(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  wrenSetSlotDouble(vm, 1, canvas->width);
  wrenSetSlotDouble(vm, 2, canvas->height);
  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 1);
  wrenInsertInList(vm, 0, -1, 2);
}

static void tic_wren_canvas_draw(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  float x = wrenGetSlotDouble(vm, 1);
  float y = wrenGetSlotDouble(vm, 2);
  tc_Color color = wrenSlotOptColor(vm, 3, WHITE);

  tic_canvas_draw(*canvas, x, y, color);
}

static void tic_wren_canvas_auto(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  tic_canvas_draw_auto(*canvas);
}

static void tic_wren_canvas_attach(WrenVM *vm) {
  tc_Canvas *canvas = wrenGetSlotForeign(vm, 0);

  tic_canvas_attach(*canvas);
}

static void tic_wren_canvas_detach(WrenVM *vm) {
  tic_canvas_detach();
}

static tc_WrenLib wrenCanvasLib[] = {
  {"draw(_,_)", tic_wren_canvas_draw},
  {"draw(_,_,_)", tic_wren_canvas_draw},
  {"draw(_,_,_,_,_,_,_)", tic_wren_canvas_draw},
  {"draw(_,_,_,_,_,_,_,_)", tic_wren_canvas_draw},
  {"auto()", tic_wren_canvas_auto},
  {"getWidth()", tic_wren_canvas_width},
  {"getHeight()", tic_wren_canvas_height},
  {"getSize()", tic_wren_canvas_size},
  {"attach()", tic_wren_canvas_attach},
  {"detach()", tic_wren_canvas_detach},
};

/***************
 * Shader
 ***************/

static void tic_wren_shader_allocate(WrenVM *vm) {

}
static void tic_wren_shader_finalize(void *data) {
  tic_shader_destroy((tc_Shader*)data);
  TIC_FREE(data);
}
static void tic_wren_shader_attach(WrenVM *vm) {
  tc_Shader *shader = (tc_Shader*)wrenGetSlotForeign(vm, 0);

  tic_shader_attach(*shader);
}
static void tic_wren_shader_detach(WrenVM *vm) {
  tic_shader_detach();
}

static void tic_wren_shader_send_float(WrenVM *vm) {
  tc_Shader *shader = (tc_Shader*)wrenGetSlotForeign(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  int type = wrenGetSlotType(vm, 2);

  if (type == WREN_TYPE_NUM) {
    float value = wrenGetSlotDouble(vm, 2);
    tic_shader_send(*shader, name, &value, TIC_UNIFORM_FLOAT);
  } else if (type == WREN_TYPE_LIST) {
    int count = wrenGetListCount(vm, 2);
    float value[count];
    for (int i = 0; i < count; i++) {
      wrenGetListElement(vm, 2, i, 0);
      value[i] = wrenGetSlotDouble(vm, 0);
    }

    tic_shader_send_count(*shader, name, count, value, TIC_UNIFORM_FLOAT);
  }

}

static void tic_wren_shader_send_vec2(WrenVM *vm) {
  tc_Shader *shader = (tc_Shader*)wrenGetSlotForeign(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);

  wrenGetListElement(vm, 2, 0, 0);
  int type = wrenGetSlotType(vm, 0);
  if (type == WREN_TYPE_NUM) {
    float value[2];
    for (int i = 0; i < 2; i++) {
      wrenGetListElement(vm, 2, i, 0);
      value[i] = wrenGetSlotDouble(vm, 0);
    }
    tic_shader_send(*shader, name, value, TIC_UNIFORM_VEC2);
  } else if (type == WREN_TYPE_LIST) {
    int count = wrenGetListCount(vm, 2);
    float value[count][2];
    for (int i = 0; i < count; i++) {
      wrenGetListElement(vm, 2, i, 0);
      wrenGetListElement(vm, 0, 0, 1);
      value[i][0] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 1, 1);
      value[i][1] = wrenGetSlotDouble(vm, 1);
    }

    tic_shader_send_count(*shader, name, count, value, TIC_UNIFORM_VEC2);
  }
}

static void tic_wren_shader_send_vec3(WrenVM *vm) {
  tc_Shader *shader = (tc_Shader*)wrenGetSlotForeign(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);

  wrenGetListElement(vm, 2, 0, 0);
  int type = wrenGetSlotType(vm, 0);
  if (type == WREN_TYPE_NUM) {
    float value[3];
    for (int i = 0; i < 3; i++) {
      wrenGetListElement(vm, 2, i, 0);
      value[i] = wrenGetSlotDouble(vm, 0);
    }
    tic_shader_send(*shader, name, value, TIC_UNIFORM_VEC3);
  } else if (type == WREN_TYPE_LIST) {
    int count = wrenGetListCount(vm, 2);
    float value[count][3];
    for (int i = 0; i < count; i++) {
      wrenGetListElement(vm, 2, i, 0);
      wrenGetListElement(vm, 0, 0, 1);
      value[i][0] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 1, 1);
      value[i][1] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 2, 1);
      value[i][2] = wrenGetSlotDouble(vm, 1);
    }

    tic_shader_send_count(*shader, name, count, value, TIC_UNIFORM_VEC3);
  }

}
static void tic_wren_shader_send_vec4(WrenVM *vm) {
  tc_Shader *shader = (tc_Shader*)wrenGetSlotForeign(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);

  wrenGetListElement(vm, 2, 0, 0);
  int type = wrenGetSlotType(vm, 0);
  if (type == WREN_TYPE_NUM) {
    float value[4];
    for (int i = 0; i < 4; i++) {
      wrenGetListElement(vm, 2, i, 0);
      value[i] = wrenGetSlotDouble(vm, 0);
    }
    tic_shader_send(*shader, name, value, TIC_UNIFORM_VEC4);
  } else if (type == WREN_TYPE_LIST) {
    int count = wrenGetListCount(vm, 2);
    float value[count][4];
    for (int i = 0; i < count; i++) {
      wrenGetListElement(vm, 2, i, 0);
      
      wrenGetListElement(vm, 0, 0, 1);
      value[i][0] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 1, 1);
      value[i][1] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 2, 1);
      value[i][2] = wrenGetSlotDouble(vm, 1);
      wrenGetListElement(vm, 0, 3, 1);
      value[i][3] = wrenGetSlotDouble(vm, 1);
    }

    tic_shader_send_count(*shader, name, count, value, TIC_UNIFORM_VEC4);
  }

}

static void tic_wren_shader_gba(WrenVM *vm) {
  tc_Shader *shader = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*shader));

  tic_shader_new_gba(shader);
}

static void tic_wren_shader_outline(WrenVM *vm) {
  tc_Shader *shader = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*shader));

  tic_shader_new_outline(shader);
}

static tc_WrenLib wrenShaderLib[] = {
  {"attach()", tic_wren_shader_attach},
  {"detach()", tic_wren_shader_detach},
  {"sendFloat(_,_)", tic_wren_shader_send_float},
  {"sendVec2(_,_)", tic_wren_shader_send_vec2},
  {"sendVec3(_,_)", tic_wren_shader_send_vec3},
  {"sendVec4(_,_)", tic_wren_shader_send_vec4},
  {"s_outline()", tic_wren_shader_outline},
  {"s_GBA()", tic_wren_shader_gba},
};

/***************
 * Graphics
 ***************/

static void tic_wren_graphics_draw_rectangle(WrenVM *vm) {
  float x = wrenGetSlotDouble(vm, 1);
  float y = wrenGetSlotDouble(vm, 2);
  float width = wrenGetSlotDouble(vm, 3);
  float height = wrenGetSlotDouble(vm, 4);
  tc_Color color = wrenSlotOptColor(vm, 5, WHITE);

  tic_graphics_draw_rectangle(x, y, width, height, color);
}

static void tic_wren_graphics_fill_rectangle(WrenVM *vm) {
  float x = wrenGetSlotDouble(vm, 1);
  float y = wrenGetSlotDouble(vm, 2);
  float width = wrenGetSlotDouble(vm, 3);
  float height = wrenGetSlotDouble(vm, 4);
  tc_Color color = wrenSlotOptColor(vm, 5, WHITE);

  tic_graphics_fill_rectangle(x, y, width, height, color);
}

static void tic_wren_graphics_print(WrenVM *vm) {
  const char *text = wrenGetSlotString(vm, 1);
  float x = wrenGetSlotDouble(vm, 2);
  float y = wrenGetSlotDouble(vm, 3);
  tc_Color color = wrenSlotOptColor(vm, 4, WHITE);

  tic_graphics_draw_text(text, x, y, color);
}

static void tic_wren_graphics_width(WrenVM *vm) {
  wrenSetSlotDouble(vm, 0, Core.window.width);
}

static void tic_wren_graphics_height(WrenVM *vm) {
  wrenSetSlotDouble(vm, 0, Core.window.height);
}

static void tic_wren_graphics_size(WrenVM *vm) {
  wrenEnsureSlots(vm, 3);
  wrenSetSlotDouble(vm, 1, Core.window.width);
  wrenSetSlotDouble(vm, 2, Core.window.height);

  wrenSetSlotNewList(vm, 0);
  wrenInsertInList(vm, 0, -1, 1);
  wrenInsertInList(vm, 0, -1, 2);
}

static void tic_wren_graphics_clear(WrenVM *vm) {
  tc_Color color = BLACK;
  int type = wrenGetSlotType(vm, 1);
  int count = wrenGetSlotCount(vm);
  if (type == WREN_TYPE_LIST) {
    color = wrenSlotOptColor(vm, 1, BLACK);
  } else if (type == WREN_TYPE_NUM) {
    for (int i = 0; i < count; i++) {
      color.data[i] = wrenGetSlotDouble(vm, i+1);
    }
  }

  tic_graphics_clear(color);
}

static void tic_wren_graphics_scissor(WrenVM *vm) {
  if (wrenGetSlotCount(vm) > 1) {
    int x = wrenGetSlotDouble(vm, 1);
    int y = wrenGetSlotDouble(vm, 2);
    int w = wrenGetSlotDouble(vm, 3);
    int h = wrenGetSlotDouble(vm, 4);
    tic_graphics_scissor(x, y, w, h);
  } else {
    tic_graphics_scissor(0, 0, Core.window.width, Core.window.height);
  }
}

static void tic_wren_graphics_push(WrenVM *vm) {}
static void tic_wren_graphics_pop(WrenVM *vm) {}
static void tic_wren_graphics_translate(WrenVM *vm) {}
static void tic_wren_graphics_rotate(WrenVM *vm) {}
static void tic_wren_graphics_scale(WrenVM *vm) {}

static tc_WrenLib wrenGraphicsLib[] = {
  {"s_drawRectangle(_,_,_,_)", tic_wren_graphics_draw_rectangle},
  {"s_drawRectangle(_,_,_,_,_)", tic_wren_graphics_draw_rectangle},
  {"s_fillRectangle(_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillRectangle(_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_drawCircle(_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_drawCircle(_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_drawCircle(_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillCircle(_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillCircle(_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillCircle(_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_drawTriangle(_,_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_drawTriangle(_,_,_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillTriangle(_,_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_fillTriangle(_,_,_,_,_,_,_)", tic_wren_graphics_fill_rectangle},
  {"s_print(_,_,_)", tic_wren_graphics_print},
  {"s_print(_,_,_,_)", tic_wren_graphics_print},
  {"s_printf(_,_,_,_,_)", tic_wren_graphics_print},
  {"s_printf(_,_,_,_,_,_)", tic_wren_graphics_print},
  {"s_clear()", tic_wren_graphics_clear},
  {"s_clear(_)", tic_wren_graphics_clear},
  {"s_clear(_,_,_)", tic_wren_graphics_clear},
  {"s_clear(_,_,_,_)", tic_wren_graphics_clear},
  {"s_scissor()", tic_wren_graphics_scissor},
  {"s_scissor(_,_,_,_)", tic_wren_graphics_scissor},
  {"s_push()", tic_wren_graphics_push},
  {"s_pop()", tic_wren_graphics_pop},
  {"s_translate(_,_)", tic_wren_graphics_translate},
  {"s_rotate(_)", tic_wren_graphics_rotate},
  {"s_scale(_,_)", tic_wren_graphics_scale},
  {"s_getWidth()", tic_wren_graphics_width},
  {"s_getHeight()", tic_wren_graphics_height},
  {"s_getSize()", tic_wren_graphics_size},
};

#ifndef TICO_WREN_H
#define TICO_WREN_H

#include "tinycoffee.h"

#include "external/wren/src/include/wren.h"
#include "scripts/wren/module.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef struct {
  WrenVM *vm;
  WrenConfiguration config;
  WrenHandle *loadHandle;
  WrenHandle *updateHandle;
  WrenHandle *drawHandle;

  WrenHandle *classHandle;
  tc_bool mainLoaded;
} tc_Wren;

TCDEF tc_Wren tc_init_wren(tc_Config *config);
TCDEF void tc_wren_game_conf(WrenVM *vm, tc_Config *config);
TCDEF void tc_wren_game_load(tc_Wren wren);
TCDEF void tc_wren_game_update(tc_Wren wren);
TCDEF void tc_wren_game_draw(tc_Wren wren);

TCDEF WrenForeignMethodFn tc_wren_bind_methods(WrenVM *vm, const char *module, const char *className, bool isStatic, const char *signature);
TCDEF WrenForeignClassMethods tc_wren_bind_class(WrenVM *vm, const char *module, const char *className);

TCDEF WrenForeignMethodFn tc_wren_read_from_lib(const tc_uint8 *signature, tc_WrenLib *lib);
TCDEF WrenForeignMethodFn tc_wren_read_from_class(const tc_uint8 *signature, tc_bool isStatic, tc_WrenClass wrenClass);

TCDEF tc_Color wrenGetSlotColorList(WrenVM *vm, int slot);
TCDEF tc_Rectf wrenGetSlotRectList(WrenVM *vm, int slot);

#endif /* TC_WREN_H */

#if defined(TICO_WREN_IMPLEMENTATION)

#include "scripts/wren/graphics.h"
#include "scripts/wren/timer.h"
#include "scripts/wren/input.h"
#include "scripts/wren/audio.h"
#include "scripts/wren/math.h"
#include "scripts/wren/tico.h"

static void tc_wren_write(WrenVM *vm, const char *str) {
  printf("%s", str);
}

static void tc_wren_error(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message) {
  printf("wren: %s:%d error: %s\n", module, line, message);
}

// static const char *tc_wren_resolve_module(WrenVM *vm, const char *importer, const char *name) {
//   if (strstr(name, "tico.")) {
//     return name;
//   }
//   size_t size = strlen(name) + strlen(".wren");
//   char *rname = tc_replace_char((char*)name, '.', '/');

// //   name = tc_replace_char(name, '.', '/');
//   char *path = malloc(size);
//   sprintf(path, "%s.wren", rname);

//   return path;
// }

static char * tc_wren_load_module(WrenVM *vm, const char *name) {
  size_t size = strlen(name) + strlen(".wren");
  tc_uint8 *rname = malloc(size);
  strcpy(rname, name);
  tc_replace_char(rname, '.', '/');
  strcat(rname, ".wren");

  tc_uint8 *buffer = tc_read_file(rname, NULL);
  free(rname);

  return buffer;
}

TCDEF tc_Wren tc_init_wren(tc_Config *config) {
  tc_Wren wren;
  wrenInitConfiguration(&wren.config);
  // wren.config.resolveModuleFn = tc_wren_resolve_module;
  wren.config.writeFn = tc_wren_write;
  wren.config.errorFn = tc_wren_error;
  wren.config.loadModuleFn = tc_wren_load_module;
  wren.config.bindForeignMethodFn = tc_wren_bind_methods;
  wren.config.bindForeignClassFn = tc_wren_bind_class;

  wren.vm = wrenNewVM(&wren.config);
  tc_bool mainExists = tc_false;

  tc_WrenModule wrenModules[] = {
    {"tico", tcWrenTicoModule},
    {"tico.graphics", tcWrenGraphicsModule},
    {"tico.input", tcWrenInputModule},
    {"tico.timer", tcWrenTimerModule},
    {"tico.audio", tcWrenAudioModule},
    {"tico.math", tcWrenMathModule},
    {NULL, NULL}
  };
  for (tc_WrenModule *m = wrenModules; m->name; m++) {
    // tc_WrenModule mod = wrenModules[i];
    wrenInterpret(wren.vm, m->name, m->buffer);
    TRACELOG("module %s loaded", m->name);
  }

  // printf("%d\n", CORE.packed);
  mainExists = tc_file_exists("main.wren");
  if (mainExists) {
    wren.loadHandle = wrenMakeCallHandle(wren.vm, "load()");
    wren.updateHandle = wrenMakeCallHandle(wren.vm, "update(_)");
    wren.drawHandle = wrenMakeCallHandle(wren.vm, "draw()");

    tc_uint8 *main = tc_read_file("main.wren", NULL);

    wrenInterpret(wren.vm, "main", main);
    free(main);
    wrenInterpret(wren.vm, "tico_main", "import \"main\" for Game\nvar g = Game.load()");

    wrenEnsureSlots(wren.vm, 1);
    wrenGetVariable(wren.vm, "tico_main", "g", 0);
    wren.classHandle = wrenGetSlotHandle(wren.vm, 0);
//     wren_config_init(config);

//     WrenHandle *configHandle = wrenMakeCallHandle(wren.vm, "config()");
//     WrenInterpretResult result = wrenCall(wren.vm, configHandle);

    wren.mainLoaded = tc_true;
  } else {
    TRACEERR("main.wren not found");
    wren.mainLoaded = tc_false;
  }

  TRACELOG("Wren module loaded");

  return wren;
}

TCDEF void tc_wren_game_load(tc_Wren wren) {
  if (!wren.mainLoaded) return;
  wrenSetSlotHandle(wren.vm, 0, wren.classHandle);

  wrenCall(wren.vm, wren.loadHandle);
}

TCDEF void tc_wren_game_update(tc_Wren wren) {
  if (!wren.mainLoaded) return;
  wrenSetSlotHandle(wren.vm, 0, wren.classHandle);
  wrenSetSlotDouble(wren.vm, 1, tc_get_delta());
  wrenCall(wren.vm, wren.updateHandle);
}

TCDEF void tc_wren_game_draw(tc_Wren wren) {
  if (!wren.mainLoaded) return;
  wrenSetSlotHandle(wren.vm, 0, wren.classHandle);
  wrenCall(wren.vm, wren.drawHandle);
}

TCDEF WrenForeignMethodFn tc_wren_read_from_lib(const tc_uint8 *signature, tc_WrenLib *lib) {
  if (!lib) return NULL;
  for (tc_WrenLib *l = lib; l; l++) {
    if (!strcmp(l->name, signature))
      return l->fun;
  }

  return NULL;
}

TCDEF WrenForeignMethodFn tc_wren_read_from_class(const tc_uint8 *signature, tc_bool isStatic, tc_WrenClass class) {
  size_t size = strlen(signature) + 2;
  tc_uint8 sSignature[size];
  memcpy(sSignature, signature, size);
  sSignature[size - 1] = '\0';
  if (isStatic) sprintf(sSignature, "s_%s", signature);

  return tc_wren_read_from_lib(sSignature, class.lib);
}

TCDEF tc_Color wrenGetSlotColorList(WrenVM *vm, int slot) {
  tc_Color color = WHITE;
  int size = wrenGetListCount(vm, slot);
  for (int i = 0; i < size; i++) {
    wrenGetListElement(vm, slot, i, 0);
    color.data[i] = wrenGetSlotDouble(vm, 0);
  }

  return color;
}

TCDEF tc_Rectf wrenGetSlotRectList(WrenVM *vm, int slot) {
  tc_Rectf rect = {0};
  int size = wrenGetListCount(vm, slot);
  for (int i = 0; i < size; i++) {
    wrenGetListElement(vm, slot, i, 0);
    rect.data[i] = wrenGetSlotDouble(vm, 0);
  }

  return rect;
}

/* Wren system methods */

tc_WrenClass tcWrenClasses[] = {
  registerWrenClassInline("Render", NULL, NULL, wrenRenderLib),
  registerWrenClassInline("Input", NULL, NULL, wrenInputLib),
  registerWrenClassInline("Timer", NULL, NULL, wrenTimerLib),
  registerWrenClassInline("Image", wren_image_allocate, wren_image_finalize, wrenImageLib),
  registerWrenClassInline("ColorF", wren_color_allocate, wren_color_finalize, wrenColorLib),
  registerWrenClassInline("Sound", wren_sound_allocate, wren_sound_finalize, wrenSoundLib),
  registerWrenClassInline("Audio", NULL, NULL, wrenAudioLib),
  registerWrenClassInline("Canvas", wren_canvas_allocate, wren_canvas_finalize, wrenCanvasLib),
  // registerWrenClassInline("Vector2", wren_vec2_allocate, wren_vec2_finalize, wrenVector2Lib),
  registerWrenClassInline("Config", NULL, NULL, wrenConfigLib),
  // registerWrenClassInline("Camera", wren_camera_allocate, wren_camera_finalize, wrenCameraLib),
  registerWrenClassInline("Font", wren_font_allocate, wren_font_finalize, wrenFontLib),
  {NULL}
};

TCDEF WrenForeignMethodFn tc_wren_bind_methods(WrenVM *vm, const char *module, const char *className, bool isStatic, const char *signature) {
  for (tc_WrenClass *class = tcWrenClasses; class; class++) {
    if (!strcmp(className, class->name))
      return tc_wren_read_from_class(signature, isStatic, *class);
  }

  return NULL;
}

TCDEF WrenForeignClassMethods tc_wren_bind_class(WrenVM *vm, const char *module, const char *className) {
  WrenForeignClassMethods methods;
  for (tc_WrenClass *class = tcWrenClasses; class; class++) {
    if (!strcmp(className, class->name)) {
      methods.allocate = class->allocate;
      methods.finalize = class->finalize;
      return methods;
    }
  }

  methods.allocate = NULL;
  methods.finalize = NULL;

  return methods;
}

#endif /* TC_WREN_IMPLEMENTATION */

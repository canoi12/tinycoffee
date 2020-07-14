#include "tico.h"

#define registerWrenClass(name, allocate, finalize, lib) \
tc_wren_class wren##name##Class = { \
  #name, \
  allocate, \
  finalize, \
  lib \
}

#define registerWrenClassInline(name, allocate, finalize, lib) \
{ name, allocate, finalize, lib }

#define wrenInitSlot(n) int slot = (n)
#define nextSlot() ++slot

typedef struct {
  char *name;
  const char *buffer;
  size_t size;
} tc_WrenModule;

typedef struct {
  char *name;
  WrenForeignMethodFn fun;
} tc_WrenLib;

typedef struct {
  char *name;
  WrenForeignMethodFn allocate;
  WrenFinalizerFn finalize;
  tc_WrenLib *lib;
} tc_WrenClass;

typedef struct {
	WrenHandle *r;
	WrenHandle *g;
	WrenHandle *b;
	WrenHandle *a;
} tc_WrenColorHandle;

typedef struct {
	WrenHandle *x;
	WrenHandle *y;
	WrenHandle *z;
	WrenHandle *a;
} tc_WrenVecHandle;

static tc_WrenColorHandle colorHandle;
static tc_WrenVecHandle vecHandle;

static tc_Color wrenSlotOptColor(WrenVM *vm, int slot, tc_Color opt) {
	tc_Color color = WHITE;
	if (wrenGetSlotType(vm, slot) == WREN_TYPE_LIST) {
		int count = wrenGetListCount(vm, slot);
		for (int i = 0; i < count; i++) {
			wrenGetListElement(vm, slot, i, slot+1);
			color.data[i] = wrenGetSlotDouble(vm, slot+1);
		}
	} else {
		color = opt;
	}

	return color;
}

#include "scripting/wren/tico.wren.h"
#include "scripting/wren/init.wren.h"
#include "scripting/wren/audio.wren.h"
#include "scripting/wren/filesystem.wren.h"
#include "scripting/wren/graphics.wren.h"
#include "scripting/wren/input.wren.h"

#include "scripting/wren/audio.h"
#include "scripting/wren/filesystem.h"
#include "scripting/wren/graphics.h"
#include "scripting/wren/input.h"

static void tic_wren_print(WrenVM *vm, const char *str) {
  printf("%s", str);
}

static void tic_wren_error(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message) {
  LOG("wren: %s:%d error: %s", module, line, message);
  char msg[2048];
  sprintf(msg, "wren: %s:%d error: %s", module, line, message);
  tic_error(msg);
//   Core.wren.mainLoaded = tc_false;
  // if (module && message) {
  //   wrenEnsureSlots(vm, 4);
  //   wrenSetSlotHandle(vm, 0, Core.wren.classHandle);
  //   wrenSetSlotString(vm, 1, module);
  //   wrenSetSlotDouble(vm, 2, line);
  //   wrenSetSlotString(vm, 3, message);
  //   wrenCall(vm, Core.wren.errorHandle);
  // }
//   wrenEnsureSlots(vm, 0);
}

static char* tic_wren_load_module(WrenVM *vm, const char *name) {
  size_t size = strlen(name) + strlen(".wren");
  char rname[size];
  strcpy(rname, name);
  tic_replace_char(rname, '.', '/');
  strcat(rname, ".wren");

  char *buffer = tic_filesystem_read_file(rname, NULL);

  return buffer;
}

static WrenForeignMethodFn tic_wren_read_from_lib(const char *signature, tc_WrenLib *lib) {
  if (!lib) return NULL;
  for (tc_WrenLib *l = lib; l; l++) {
    if (!strcmp(l->name, signature)) return l->fun;
  }
  return NULL;
}

static WrenForeignMethodFn tic_wren_read_from_class(const char *signature, tc_bool isStatic, tc_WrenClass tcClass) {
  size_t size = strlen(signature) + 2;
  char sSignature[size];
  memcpy(sSignature, signature, size);
  sSignature[size - 1] = '\0';
  if (isStatic) sprintf(sSignature, "s_%s", signature);

  return tic_wren_read_from_lib(sSignature, tcClass.lib);
}

tc_WrenClass tcWrenClasses[] = {
  registerWrenClassInline("Audio", NULL, NULL, wrenAudioLib),
  registerWrenClassInline("Sound", tic_wren_sound_allocate, tic_wren_sound_finalize, wrenSoundLib),
  registerWrenClassInline("Filesystem", NULL, NULL, wrenFilesystemLib),
  registerWrenClassInline("Graphics", NULL, NULL, wrenGraphicsLib),
  registerWrenClassInline("Image", tic_wren_image_allocate, tic_wren_image_finalize, wrenImageLib),
  registerWrenClassInline("Canvas", tic_wren_canvas_allocate, tic_wren_canvas_finalize, wrenCanvasLib),
  registerWrenClassInline("Shader", tic_wren_shader_allocate, tic_wren_shader_finalize, wrenShaderLib),
  registerWrenClassInline("Keyboard", NULL, NULL, wrenKeyboardLib),
  registerWrenClassInline("Input", NULL, NULL, wrenInputLib),
  registerWrenClassInline("Mouse", NULL, NULL, wrenMouseLib),
  registerWrenClassInline("Joystick", NULL, NULL, wrenJoystickLib),
  {NULL}
};

static WrenForeignMethodFn tic_wren_bind_methods(WrenVM *vm, const char *module, const char *className, bool isStatic, const char *signature) {
  for (tc_WrenClass *tcClass = tcWrenClasses; tcClass; tcClass++) {
    if (!strcmp(className, tcClass->name))
      return tic_wren_read_from_class(signature, isStatic, *tcClass);
  }

  return NULL;
}

static WrenForeignClassMethods tic_wren_bind_class(WrenVM *vm, const char *module, const char *className) {
  WrenForeignClassMethods methods;
  for (tc_WrenClass *tcClass = tcWrenClasses; tcClass; tcClass++) {
    if (!strcmp(className, tcClass->name)) {
      methods.allocate = tcClass->allocate;
      methods.finalize = tcClass->finalize;
      return methods;
    }
  }

  methods.allocate = NULL;
  methods.finalize = NULL;

  return methods;
}

tc_bool tic_wren_init() {
  tc_Wren *wren = &Core.wren;
  wrenInitConfiguration(&wren->config);
  wren->config.writeFn = tic_wren_print;
  wren->config.errorFn = tic_wren_error;
  wren->config.loadModuleFn = tic_wren_load_module;
  wren->config.bindForeignClassFn = tic_wren_bind_class;
  wren->config.bindForeignMethodFn = tic_wren_bind_methods;

  wren->vm = wrenNewVM(&wren->config);

  tc_WrenModule wrenModules[] = {
    {"tico", tico_wren, tico_wren_size},
    {"tico.audio", audio_wren, audio_wren_size},
    {"tico.filesystem", filesystem_wren, filesystem_wren_size},
    {"tico.graphics", graphics_wren, graphics_wren_size},
    {"tico.input", input_wren, input_wren_size},
    {NULL, NULL}
  };

  for (tc_WrenModule *mod = wrenModules; mod->name; mod++) {
    char buffer[mod->size + 1];
    memcpy(buffer, mod->buffer, mod->size);
    buffer[mod->size] = '\0';
    wrenInterpret(wren->vm, mod->name, buffer);
  }
  Core.state.load = tic_wren_load;
  Core.state.step = tic_wren_step;
  wren->mainLoaded = tc_true;

  colorHandle.r = wrenMakeCallHandle(wren->vm, "r");
  colorHandle.g = wrenMakeCallHandle(wren->vm, "g");
  colorHandle.b = wrenMakeCallHandle(wren->vm, "b");
  colorHandle.a = wrenMakeCallHandle(wren->vm, "a");

  wren->loadHandle = wrenMakeCallHandle(wren->vm, "load()");
  wren->stepHandle = wrenMakeCallHandle(wren->vm, "step(_)");
  wren->errorHandle = wrenMakeCallHandle(wren->vm, "error(_,_,_)");

  WrenHandle *initHandle = wrenMakeCallHandle(wren->vm, "init()");

  wrenInterpret(wren->vm, "init", init_wren);

  wrenEnsureSlots(wren->vm, 1);
  wrenGetVariable(wren->vm, "init", "TicoInit", 0);
  wren->classHandle = wrenGetSlotHandle(wren->vm, 0);

  wrenCall(wren->vm, initHandle);
  wrenReleaseHandle(wren->vm, initHandle);

  TRACELOG("Wren module loaded");

  return tc_true;
}

void tic_wren_terminate() {
	tc_Wren *wren = &Core.wren;
	wrenReleaseHandle(wren->vm, wren->loadHandle);
	wrenReleaseHandle(wren->vm, wren->stepHandle);
	wrenReleaseHandle(wren->vm, wren->errorHandle);

	wrenReleaseHandle(wren->vm, colorHandle.r);
	wrenReleaseHandle(wren->vm, colorHandle.g);
	wrenReleaseHandle(wren->vm, colorHandle.b);
	wrenReleaseHandle(wren->vm, colorHandle.a);

	wrenReleaseHandle(wren->vm, vecHandle.x);
	wrenReleaseHandle(wren->vm, vecHandle.y);
	wrenReleaseHandle(wren->vm, vecHandle.z);
	wrenReleaseHandle(wren->vm, vecHandle.a);
}

int tic_wren_load() {
  if (!Core.wren.mainLoaded) return 0;
  wrenSetSlotHandle(Core.wren.vm, 0, Core.wren.classHandle);
  wrenCall(Core.wren.vm, Core.wren.loadHandle);
}

int tic_wren_step() {
  if (!Core.wren.mainLoaded) return 0;
  wrenSetSlotHandle(Core.wren.vm, 0, Core.wren.classHandle);
  wrenSetSlotDouble(Core.wren.vm, 1, tic_timer_get_delta());
  wrenCall(Core.wren.vm, Core.wren.stepHandle);
}

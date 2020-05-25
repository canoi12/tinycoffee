#ifndef WREN_MODULE_H
#define WREN_MODULE_H

// #include "tinycoffee.h"

#include "../../external/wren/src/include/wren.h"

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
  const char *name;
  const char *buffer;
} tc_wren_module;

typedef struct {
  char *name;
  WrenForeignMethodFn fun;
} tc_wren_lib;

typedef struct {
  char *name;
  WrenForeignMethodFn allocate;
  WrenFinalizerFn finalize;
  tc_wren_lib *lib;
} tc_wren_class;

#endif // WREN_MODULE_H

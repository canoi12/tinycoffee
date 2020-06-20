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

#endif // WREN_MODULE_H

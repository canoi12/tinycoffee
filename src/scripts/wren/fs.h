#include "module.h"

/**********************
 * File class
 **********************/

static void wren_file_allocate(WrenVM *vm) {
  FILE **file = wrenSetSlotNewForeign(vm, 0, 0, sizeof(FILE*));
  const char *filename = wrenGetSlotString(vm, 1);
  *file = fopen(filename, "w+");
}

static void wren_close_file(FILE **file) {
  if (*file == NULL) return;
  fclose(*file);
  *file = NULL;
}

static void wren_file_finalize(void *data) {
  wren_close_file((FILE**)data);
}

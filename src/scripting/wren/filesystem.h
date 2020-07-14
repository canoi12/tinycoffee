#include "../../tico.h"

/****************
 * Filesystem
 ****************/

static void tic_wren_filesystem_read(WrenVM *vm) {
  const char *filename = wrenGetSlotString(vm, 1);

  char *buffer = tic_filesystem_read_file(filename, NULL);

  wrenSetSlotString(vm, 0, buffer);
  free(buffer);
}

static void tic_wren_filesystem_write(WrenVM *vm) {
  const char *filename = wrenGetSlotString(vm, 1);
  const char *text = wrenGetSlotString(vm, 2);

  tic_filesystem_write_file(filename, text, strlen(text), 0);
}

static void tic_wren_filesystem_mkdir(WrenVM *vm) {
  const char *name = wrenGetSlotString(vm, 1);

  tic_filesystem_mkdir(name);
}

static void tic_wren_filesystem_rmdir(WrenVM *vm) {
  const char *name = wrenGetSlotString(vm, 1);

  tic_filesystem_rmdir(name);
}

static void tic_wren_filesystem_file_exists(WrenVM *vm) {
  const char *filename = wrenGetSlotString(vm, 1);

  int exists = tic_filesystem_file_exists(filename);

  wrenSetSlotBool(vm, 0, exists);
}

static void tic_wren_filesystem_get_path(WrenVM *vm) {
  wrenSetSlotString(vm, 0, tic_filesystem_get_path());
}

static void tic_wren_filesystem_resolve_path(WrenVM *vm) {
  const char *filename = wrenGetSlotString(vm, 1);

  char buffer[256];
  tic_filesystem_resolve_path(buffer, filename);

  wrenSetSlotString(vm, 0, buffer);
}

static tc_WrenLib wrenFilesystemLib[] = {
  {"s_read(_)", tic_wren_filesystem_read},
  {"s_write(_,_)", tic_wren_filesystem_write},
  {"s_fileExists(_)", tic_wren_filesystem_file_exists},
  {"s_getPath()", tic_wren_filesystem_get_path},
  {"s_resolvePath(_)", tic_wren_filesystem_resolve_path},
  {"s_mkdir(_)", tic_wren_filesystem_mkdir},
  {"s_rmdir(_)", tic_wren_filesystem_rmdir},
};

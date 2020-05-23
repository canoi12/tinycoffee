#ifndef TC_MODULES_H
#define TC_MODULES_H

typedef struct {
  void (*moduleLoad)();
  void (*moduleUpdate)();
  void (*moduleDraw)();
} tc_module;

#endif

#include "module.h"

static const char* tcWrenTicoModule =
"class WindowFlags {\n"
" static NoResizable {1}\n"
"}\n"
"class Config {\n"
// " construct new() {}\n"
" foreign static title=(value)\n"
" foreign static width=(value)\n"
" foreign static height=(value)\n"
"}\n"
"class Tico {\n"
" config() {}\n"
" construct load() {}\n"
" update(dt) {}\n"
" draw() {}\n"
" windowResize(width, height) {}\n"
" keyDown(key, action) {}\n"
"}\n";
// "foreign class Camera {\n"
// " construct new(x, y, w, h) {}\n"
// " foreign attach()\n"
// " foreign detach()\n"
// " foreign move(vector)\n"
// " foreign move(x, y)\n"
// " foreign setPos(x, y)\n"
// " foreign follow(pos)\n"
// " foreign center=(value)\n"
// " foreign zoom=(value)\n"
// "}\n";

static tc_Config *wconf;

typedef struct {
  int x;
  int y;
  int w;
  int h;
  vec2 center;
  vec2 scale;
} tc_Camera;

// static tc_Camera tc_create_camera(int x, int y, int w, int h) {}
// static void tc_attach_camera(tc_Camera camera) {}
// static void tc_detach_camera(void) {}

// static void wren_config_allocate(WrenVM *vm) {
//   tc_config *config = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*config));
//   config->width = 0;
//   config->height = 0;
//   config->windowFlags = 0;
// }

// static void wren_config_finalize(void *data) {
//   free(data);
// }

// static void wren_camera_allocate(WrenVM *vm) {
//   tc_Camera *camera = wrenSetSlotNewForeign(vm, 0, 0, sizeof(*camera));
//   float x = wrenGetSlotDouble(vm, 1);
//   float y = wrenGetSlotDouble(vm, 2);
//   float w = wrenGetSlotDouble(vm, 3);
//   float h = wrenGetSlotDouble(vm, 4);

//   *camera = tc_create_camera(x, y, w, h);
// }

// static void wren_camera_finalize(void *data) {
//   free(data);
// }

// static void wren_camera_position(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);
//   float x = wrenGetSlotDouble(vm, 1);
//   float y = wrenGetSlotDouble(vm, 2);
//   camera->x = x;
//   camera->y = y;
// }

// static void wren_camera_center(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);
//   vec2 *v = wrenGetSlotForeign(vm, 1);
//   camera->center.x = v->x;
//   camera->center.y = v->y;
// }

// static void wren_camera_follow(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);
//   vec2 *v = wrenGetSlotForeign(vm, 1);

//   camera->x = -v->x;
//   camera->y = -v->y;
// }

// static void wren_camera_zoom(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);
//   float zoom = wrenGetSlotDouble(vm, 1);

//   camera->scale.x = zoom;
//   camera->scale.y = zoom;
// }

// static void wren_camera_move(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);
//   if (wrenGetSlotType(vm, 1) == WREN_TYPE_FOREIGN) {
//     vec2 *vec = wrenGetSlotForeign(vm, 1);
// //     vec2_add(&camera->position, camera->position, *vec);
//     camera->x = vec->x;
//     camera->y = vec->y;
//   } else {
//     float dx = wrenGetSlotDouble(vm, 1);
//     float dy = wrenGetSlotDouble(vm, 2);
//     camera->x += dx;
//     camera->y += dy;
//   }
// }

// static void wren_camera_attach(WrenVM *vm) {
//   tc_Camera *camera = wrenGetSlotForeign(vm, 0);

//   tc_attach_camera(camera);
// }

// static void wren_camera_detach(WrenVM *vm) {
//   tc_detach_camera();
// }

// static tc_WrenLib wrenCameraLib[] = {
//   {"move(_)", wren_camera_move},
//   {"move(_,_)", wren_camera_move},
//   {"attach()", wren_camera_attach},
//   {"detach()", wren_camera_detach},
//   {"setPos(_,_)", wren_camera_position},
//   {"center=(_)", wren_camera_center},
//   {"follow(_)", wren_camera_follow},
//   {"zoom=(_)", wren_camera_zoom}
// };

// Config

static void wren_config_init(tc_Config *config) {
  // wconf = malloc(sizeof(*wconf));
  // memcpy(wconf, config, sizeof(tc_config));
  wconf = config;
}

static void wren_config_title(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  const char *title = wrenGetSlotString(vm, 1);
  strcpy(wconf->title, title);
}

static void wren_config_width(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  int width = wrenGetSlotDouble(vm, 1);
  wconf->width = width;
}

static void wren_config_height(WrenVM *vm) {
  // tc_config *config = wrenGetSlotForeign(vm, 0);
  int height = wrenGetSlotDouble(vm, 1);
  wconf->height = height;
}

static tc_WrenLib wrenConfigLib[] = {
  {"s_title=(_)", wren_config_title},
  {"s_width=(_)", wren_config_width},
  {"s_height=(_)", wren_config_height}
};

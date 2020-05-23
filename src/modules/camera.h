#ifndef TC_CAMERA_H
#define TC_CAMERA_H

#include "../tinycoffee.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef struct {
  vec2 position;
  vec2 size;
  vec2 scale;
  vec2 center;
  float angle;
  matrix view;
} tc_camera;

TCDEF tc_camera tc_create_camera(vec2 pos, vec2 size);
TCDEF void tc_destroy_camera(tc_camera *camera);

TCDEF void tc_attach_camera(tc_camera *camera);
TCDEF void tc_detach_camera();

#endif // TC_CAMERA_H

#if defined(TC_CAMERA_IMPLEMENTATION)

TCDEF tc_camera tc_create_camera(vec2 pos, vec2 size) {
  tc_camera camera = {0};

  camera.position = pos;
  camera.size = size;
  camera.scale = vec2_new(1, 1);
  camera.center = vec2_new(0, 0);
  camera.angle = 0;
  matrix_identity(&camera.view);

  return camera;
}

TCDEF void tc_destroy_camera(tc_camera *camera) {
  free(camera);
}


#endif


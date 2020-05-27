#ifndef TC_CAMERA_H
#define TC_CAMERA_H

#include "../tinycoffee.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

TCDEF tc_camera tc_create_camera(tc_int32 x, tc_int32 y, tc_int32 w, tc_int32 h);
TCDEF void tc_destroy_camera(tc_camera *camera);

TCDEF void tc_camera_rotate(float angle);
TCDEF void tc_camera_zoom_in(float zoom);
TCDEF void tc_camera_zoom_out(float zoom);

TCDEF void tc_attach_camera(tc_camera *camera);
TCDEF void tc_detach_camera();

#endif // TC_CAMERA_H

#if defined(TC_CAMERA_IMPLEMENTATION)

TCDEF tc_camera tc_create_camera(tc_int32 x, tc_int32 y, tc_int32 w, tc_int32 h) {
  tc_camera camera = {0};

  camera.x = x;
  camera.y = y;
  camera.width = w;
  camera.height= h;
  camera.scale = vec2_new(1, 1);
  camera.center = vec2_new(0, 0);
  camera.angle = 0;
  matrix_identity(&camera.view);

  return camera;
}

TCDEF void tc_destroy_camera(tc_camera *camera) {
  free(camera);
}

TCDEF void tc_attach_camera(tc_camera *camera) {
  tc_reset_batch(&CORE.render);
  matrix_identity(&camera->view);
  matrix_translate(&camera->view, camera->x*camera->scale.x, camera->y*camera->scale.y, 0);

  matrix_rotate_z(&camera->view, camera->angle);
  matrix_scale(&camera->view, camera->scale.x, camera->scale.y, 1);
  matrix_translate(&camera->view, camera->center.x/camera->scale.x, camera->center.y/camera->scale.y, 0);

  CORE.render.state.camera = &camera->view;
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  glScissor(0, view[3] - camera->height, camera->width, camera->height);
  glEnable(GL_SCISSOR_TEST);
}

TCDEF void tc_detach_camera() {
  tc_reset_batch(&CORE.render);
  glDisable(GL_SCISSOR_TEST);
  CORE.render.state.camera = NULL;
}

#endif


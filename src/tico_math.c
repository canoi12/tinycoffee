#include "tico.h"

#include "linmath.h"

#define vec_implement(n) \
void tico_vec##n##_add(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    vec##n##_add(v->data, a.data, b.data); \
} \
void tico_vec##n##_sub(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    vec##n##_sub(v->data, a.data, b.data); \
} \
void tico_vec##n##_mul(tc_Vec##n *v, const tc_Vec##n a, const float number) { \
  for (int i = 0; i < n; i++) \
    vec##n##_scale(v->data, a.data, number); \
} \
void tico_vec##n##_div(tc_Vec##n *v, const tc_Vec##n a, const float number) { \
  for (int i = 0; i < n; i++) \
    vec##n##_div(v->data, a.data, number); \
} \
float tico_vec##n##_dot(const tc_Vec##n a, const tc_Vec##n b) { \
  float d = vec##n##_mul_inner(a.data, b.data); \
  return d; \
} \
void tico_vec##n##_print(const tc_Vec##n v) { \
  printf("tc_Vec%d(", n); \
  for (int i = 0; i < n-1; i++) \
    printf("%f, ", v.data[i]); \
  printf("%f)\n", v.data[n-1]); \
} \
int tico_vec##n##_equals(const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    if (a.data[i] != b.data[i]) return 0; \
  return 1; \
} \
float tico_vec##n##_len(const tc_Vec##n v) { \
  float len = vec##n##_len(v.data); \
  return len; \
} \
void tico_vec##n##_normalize(tc_Vec##n *v, const tc_Vec##n u) { \
  float k = 1.f / vec##n##_len(u.data); \
  tico_vec##n##_mul(v, u, k); \
} \
float tico_vec##n##_distance(const tc_Vec##n a, const tc_Vec##n b) { \
  tc_Vec##n s; \
  tico_vec##n##_sub(&s, a, b); \
  float dist = tico_vec##n##_len(s); \
  return dist; \
} \
void tico_vec##n##_lerp(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b, const float t) { \
  for (int i=0; i<n; i++) \
    r->data[i] = tico_lerp(a.data[i], b.data[i], t); \
} \

float tico_lerp(float a, float b, float t) {
  return (1-t)*a + t*b;
}

float tico_round(float n) {
  return round(n);
}

float tico_distance(float a, float b) {
  float dx = a*a;
  float dy = b*b;
  return sqrtf(dx+dy);
}

int tico_sign(float a) {
  a = tico_round(a);
  return a < 0 ? -1 : (a == 0 ? 0 : 1);
}

// float tico_clamp(float v, float lo, float hi) {
//   float mi = tico_min(v, hi);
//   float ma = tico_max(lo, mi);
//   return ma;
// }

float tico_angle(float x0, float y0, float x1, float y1) {
  tc_Vec2 v0 = tico_vec2(x0, y0);
  tc_Vec2 v1 = tico_vec2(x1, y1);
  tc_Vec2 s;
  tico_vec2_sub(&s, v0, v1);
  return tico_vec2_angle(s);
}

/********************
 * Vec2
 ********************/
vec_implement(2);
float tico_vec2_perpdot(tc_Vec2 v, tc_Vec2 w) {
  return (v.x * w.y) - (v.y * w.x);
}

void tico_vec2_normal(tc_Vec2 *a, tc_Vec2 v)
{
  a->x = -v.y;
  a->y = -v.x;
}

void tico_vec2_projection(tc_Vec2 *r, const tc_Vec2 v, const tc_Vec2 u) {
  float dot = tico_vec2_dot(v, u);
  float len = tico_vec2_len(u);
  float mag = len * len;
//   tc_Vec2 out;
  tico_vec2_mul(r, u, (dot / mag));
}

float tico_vec2_angle(tc_Vec2 v) {
  return atan2f(v.y, v.x);
}

/*******************
 * Vec3
 *******************/
vec_implement(3);
void tico_vec3_cross(tc_Vec3 *r, const tc_Vec3 a, const tc_Vec3 b) {
  vec3_mul_cross(r->data, a.data, b.data);
}
void tico_vec3_reflect(tc_Vec3 *r, tc_Vec3 const v, tc_Vec3 const n) {
  vec3_reflect(r->data, v.data, n.data);
}

/******************
 * Vec4
 ******************/
vec_implement(4);
void tico_vec4_cross(tc_Vec4 *r, tc_Vec4 a, tc_Vec4 b) {
  vec4_mul_cross(r->data, a.data, b.data);
}

void tico_vec4_reflect(tc_Vec4 *r, tc_Vec4 v, tc_Vec4 n) {
  vec4_reflect(r->data, v.data, n.data);
}

/*******************
 * Matrix
 *******************/

void tico_matrix_identity(tc_Matrix *m) {
  mat4x4_identity(m->data);
}

void tico_matrix_print(tc_Matrix m) {
  for (int i = 0; i < 4; i++) {
    tico_vec4_print(m.v[i]);
  }
}

void tico_matrix_clone(tc_Matrix *m, tc_Matrix s) {
  mat4x4_dup(m->data, s.data);
}

int tico_matrix_equals(tc_Matrix a, tc_Matrix b) {
  for (int i = 0; i < 4; i++) {
    if (!tico_vec4_equals(a.v[i], b.v[i])) return 0;
  }
  return 1;
}

void tico_matrix_row(tc_Vec4 *v, tc_Matrix m, int row) {
  mat4x4_row(v->data, m.data, row);
}

void tico_matrix_col(tc_Vec4 *v, tc_Matrix m, int col) {
  mat4x4_col(v->data, m.data, col);
}

void tico_matrix_transpose(tc_Matrix *m, tc_Matrix n) {
  mat4x4_transpose(m->data, n.data);
}

void tico_matrix_add(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_add(m->data, a.data, b.data);
}

void tico_matrix_sub(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_sub(m->data, a.data, b.data);
}

void tico_matrix_scale(tc_Matrix *r, tc_Matrix m, float a) {
  mat4x4_scale(r->data, m.data, a);
}

void tico_matrix_scale_aniso(tc_Matrix *m, tc_Matrix a, float x, float y, float z) {
  mat4x4_scale_aniso(m->data, a.data, x, y, z);
}

void tico_matrix_mul(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_mul(m->data, a.data, b.data);
}

void tico_matrix_mul_vec4(tc_Vec4 *r, tc_Matrix a, tc_Vec4 b) {
  mat4x4_mul_vec4(r->data, a.data, b.data);
}

void tico_matrix_translate(tc_Matrix *m, float x, float y, float z) {
  mat4x4_translate(m->data, x, y, z);
}

void tico_matrix_translate_in_place(tc_Matrix *r, float x, float y, float z) {
  mat4x4_translate_in_place(r->data, x, y, z);
}

void tico_matrix_from_vec3_mul_outer(tc_Matrix *m, tc_Vec3 a, tc_Vec3 b) {
  mat4x4_from_vec3_mul_outer(m->data, a.data, b.data);
}

void tico_matrix_rotate(tc_Matrix *m, tc_Matrix a, float x, float y, float z, float angle) {
  mat4x4_rotate(m->data, a.data, x, y, z, angle);
}

void tico_matrix_rotate_x(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_X(m->data, a.data, angle);
}

void tico_matrix_rotate_y(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_Y(m->data, a.data, angle);
}

void tico_matrix_rotate_z(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_Z(m->data, a.data, angle);
}

void tico_matrix_invert(tc_Matrix *m, tc_Matrix a) {
  mat4x4_invert(m->data, a.data);
}


void tico_matrix_translatev(tc_Matrix *m, tc_Vec3 v) {
  m->m03 += v.x;
  m->m13 += v.y;
  m->m23 += v.z;
}

void tico_matrix_scaleV(tc_Matrix *m, tc_Vec3 v) {
  for (int i = 0; i < 3; i++) {
    m->data[i][i] *= v.data[i];
  }
}

void tico_matrix_rotate2D(tc_Matrix *r, tc_Matrix m, float angle) {
  tico_matrix_rotate_z(r, m, angle);
}

void tico_matrix_orthonormalize(tc_Matrix *r, tc_Matrix m) {
  mat4x4_orthonormalize(r->data, m.data);
}

void tico_matrix_frustum(tc_Matrix *m, float l, float r, float b, float t, float n, float f) {
  mat4x4_frustum(m->data, l, r, b, t, n, f);
}

void tico_matrix_ortho(tc_Matrix *m, float l, float r, float b, float t, float n, float f) {
  mat4x4_ortho(m->data, l, r, b, t, n, f);
}

void tico_matrix_perspective(tc_Matrix *m, float y_fov, float aspect, float n, float f) {
  mat4x4_perspective(m->data, y_fov, aspect, n, f);
}

void tico_matrix_look_at(tc_Matrix *m, tc_Vec3 eye, tc_Vec3 center, tc_Vec3 up) {
  mat4x4_look_at(m->data, eye.data, center.data, up.data);
}
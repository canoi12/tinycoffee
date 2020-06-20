#ifndef TICO_MATH_H
#define TICO_MATH_H

#define LINMATH_NO_INLINE
#include "external/linmath.h"

#define tc_vec2_new(x, y) ((tc_Vec2){(x), (y)})
#define tc_vec2_zero() ((tc_Vec2){0, 0})
#define tc_vec2_from_vec3(v) ((tc_Vec2){(v.x), (v.y)})

#define tc_vec3_new(x, y, z) ((tc_Vec3){(x), (y), (z)})
#define tc_vec3_from_vec4(v) ((tc_Vec3){(v.x), (v.y), (v.z)})

#define tc_vec4_new(x, y, z, w)((tc_Vec4){(x), (y), (z), (w)})

#ifndef TCDEF
  #define TCDEF
#endif

#include <math.h>

#define vec2_field() \
  float x; \
  float y; \

#define vec3_field() \
  vec2_field() \
  float z; \

#define vec4_field() \
  vec3_field() \
  float w;

#define vec_init(n) \
typedef struct tc_Vec##n { \
  union { \
    vec##n data; \
    struct { \
      vec##n##_field() \
    }; \
  }; \
} tc_Vec##n; \
TCDEF void tc_vec##n##_add(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_sub(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_mul(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TCDEF void tc_vec##n##_div(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TCDEF float tc_vec##n##_dot(const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_print(const tc_Vec##n v); \
TCDEF int tc_vec##n##_equals(const tc_Vec##n a, const tc_Vec##n b); \
TCDEF float tc_vec##n##_len(const tc_Vec##n v); \
TCDEF void tc_vec##n##_normalize(tc_Vec##n *v, const tc_Vec##n u); \
TCDEF float tc_vec##n##_distance(const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_min(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_max(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TCDEF void tc_vec##n##_lerp(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b, const float t);

#define vec_implement(n) \
TCDEF void tc_vec##n##_add(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    vec##n##_add(v->data, a.data, b.data); \
} \
TCDEF void tc_vec##n##_sub(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    vec##n##_sub(v->data, a.data, b.data); \
} \
TCDEF void tc_vec##n##_mul(tc_Vec##n *v, const tc_Vec##n a, const float number) { \
  for (int i = 0; i < n; i++) \
    vec##n##_scale(v->data, a.data, number); \
} \
TCDEF void tc_vec##n##_div(tc_Vec##n *v, const tc_Vec##n a, const float number) { \
  for (int i = 0; i < n; i++) \
    vec##n##_div(v->data, a.data, number); \
} \
TCDEF float tc_vec##n##_dot(const tc_Vec##n a, const tc_Vec##n b) { \
  float d = vec##n##_mul_inner(a.data, b.data); \
  return d; \
} \
TCDEF void tc_vec##n##_print(const tc_Vec##n v) { \
  printf("tc_Vec%d(", n); \
  for (int i = 0; i < n-1; i++) \
    printf("%f, ", v.data[i]); \
  printf("%f)\n", v.data[n-1]); \
} \
TCDEF int tc_vec##n##_equals(const tc_Vec##n a, const tc_Vec##n b) { \
  for (int i = 0; i < n; i++) \
    if (a.data[i] != b.data[i]) return 0; \
  return 1; \
} \
TCDEF float tc_vec##n##_len(const tc_Vec##n v) { \
  float len = vec##n##_len(v.data); \
  return len; \
} \
TCDEF void tc_vec##n##_normalize(tc_Vec##n *v, const tc_Vec##n u) { \
  float k = 1.f / vec##n##_len(u.data); \
  tc_vec##n##_mul(v, u, k); \
} \
TCDEF float tc_vec##n##_distance(const tc_Vec##n a, const tc_Vec##n b) { \
  tc_Vec##n s; \
  tc_vec##n##_sub(&s, a, b); \
  float dist = tc_vec##n##_len(s); \
  return dist; \
} \
TCDEF void tc_vec##n##_lerp(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b, const float t) { \
  for (int i=0; i<n; i++) \
    r->data[i] = tc_lerp(a.data[i], b.data[i], t); \
} \

TCDEF float tc_lerp(float a, float b, float t);
TCDEF float tc_round(float n);
TCDEF float tc_distance(float a, float b);
TCDEF int tc_sign(float a);
TCDEF float tc_clamp(float v, float lo, float hi);
TCDEF float tc_angle(float x0, float y0, float x1, float y1);

// Vec2 defitions
vec_init(2)
TCDEF float tc_vec2_perpdot(const tc_Vec2 v, const tc_Vec2 w);
TCDEF void tc_vec2_normal(tc_Vec2 *r, const tc_Vec2 v);
TCDEF void tc_vec2_projection(tc_Vec2 *r, const tc_Vec2 v, const tc_Vec2 w);
TCDEF float tc_vec2_angle(const tc_Vec2 v);
// Vec3 definitions
vec_init(3)
TCDEF void tc_vec3_cross(tc_Vec3 *r, const tc_Vec3 a, const tc_Vec3 b);
TCDEF void tc_vec3_reflect(tc_Vec3 *r, tc_Vec3 const v, tc_Vec3 const n);
// Vec4 defitions
vec_init(4)
TCDEF void tc_vec4_cross(tc_Vec4 *r, const tc_Vec4 a, const tc_Vec4 b);
TCDEF void tc_vec4_reflect(tc_Vec4 *r, const tc_Vec4 v, const tc_Vec4 n);

typedef struct matrix_s {
  union {
    mat4x4 data;
    tc_Vec4 v[4];
    struct {
      float m00, m10, m20, m30;
      float m01, m11, m21, m31;
      float m02, m12, m22, m32;
      float m03, m13, m23, m33;
    };
  };
} tc_Matrix;

// Matrix defitions
TCDEF void tc_matrix_identity(tc_Matrix *m);
TCDEF void tc_matrix_clone(tc_Matrix *m, tc_Matrix s);
TCDEF int tc_matrix_equals(tc_Matrix a, tc_Matrix b);
TCDEF void tc_matrix_print(tc_Matrix m);
TCDEF void tc_matrix_row(tc_Vec4 *v, tc_Matrix m, int row);
TCDEF void tc_matrix_transpose(tc_Matrix *m, tc_Matrix n);
TCDEF void tc_matrix_translate(tc_Matrix *r, float x, float y, float z);
TCDEF void tc_matrix_translate_in_place(tc_Matrix *r, float x, float y, float z);
TCDEF void tc_matrix_translatev(tc_Matrix *r, tc_Vec3 v);
TCDEF void tc_matrix_scale(tc_Matrix *r, tc_Matrix m, float s);
TCDEF void tc_matrix_scale_aniso(tc_Matrix *m, tc_Matrix a, float x, float y, float z);
TCDEF void tc_matrix_scaleV(tc_Matrix *r, tc_Vec3 v);
TCDEF void tc_matrix_from_vec3_mul_outer(tc_Matrix *m, tc_Vec3 a, tc_Vec3 b);
TCDEF void tc_matrix_mul_vec4(tc_Vec4 *v, tc_Matrix m, tc_Vec4 n);
TCDEF void tc_matrix_mul(tc_Matrix *r, tc_Matrix m, tc_Matrix n);
TCDEF void tc_matrix_rotate(tc_Matrix *r, tc_Matrix m, float x, float y, float z, float angle);
TCDEF void tc_matrix_rotate_x(tc_Matrix *r, tc_Matrix m, float angle);
TCDEF void tc_matrix_rotate_y(tc_Matrix *r, tc_Matrix m, float angle);
TCDEF void tc_matrix_rotate_z(tc_Matrix *r, tc_Matrix m, float angle);
TCDEF void tc_matrix_rotate2D(tc_Matrix *r, tc_Matrix m, float angle);
TCDEF void tc_matrix_invert(tc_Matrix *r, tc_Matrix m);
TCDEF void tc_matrix_orthonormalize(tc_Matrix *r, tc_Matrix m);
TCDEF void tc_matrix_frustum(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TCDEF void tc_matrix_ortho(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TCDEF void tc_matrix_perspective(tc_Matrix *m, float y_fov, float aspect, float n, float f);
TCDEF void tc_matrix_look_at(tc_Matrix *m, tc_Vec3 eye, tc_Vec3 center, tc_Vec3 up);

#endif /* TICO_MATH_H */

#if defined(TICO_MATH_IMPLEMENTATION)

#include "external/linmath.h"

TCDEF float tc_lerp(float a, float b, float t) {
  return (1-t)*a + t*b;
}

TCDEF float tc_round(float n) {
  return round(n);
}

TCDEF float tc_distance(float a, float b) {
  float dx = a*a;
  float dy = b*b;
  return sqrtf(dx+dy);
}

TCDEF int tc_sign(float a) {
  a = tc_round(a);
  return a < 0 ? -1 : (a == 0 ? 0 : 1);
}

TCDEF float tc_clamp(float v, float lo, float hi) {
  float mi = tc_min(v, hi);
  float ma = tc_max(lo, mi);
  return ma;
}

TCDEF float tc_angle(float x0, float y0, float x1, float y1) {
  tc_Vec2 v0 = tc_vec2_new(x0, y0);
  tc_Vec2 v1 = tc_vec2_new(x1, y1);
  tc_Vec2 s;
  tc_vec2_sub(&s, v0, v1);
  return tc_vec2_angle(s);
}

vec_implement(2);

TCDEF float tc_vec2_perpdot(tc_Vec2 v, tc_Vec2 w) {
  return (v.x * w.y) - (v.y * w.x);
}

TCDEF void tc_vec2_normal(tc_Vec2 *a, tc_Vec2 v)
{
  a->x = -v.y;
  a->y = -v.x;
}

TCDEF void tc_vec2_projection(tc_Vec2 *r, const tc_Vec2 v, const tc_Vec2 u) {
  float dot = tc_vec2_dot(v, u);
  float len = tc_vec2_len(u);
  float mag = len * len;
//   tc_Vec2 out;
  tc_vec2_mul(r, u, (dot / mag));
}

TCDEF float tc_vec2_angle(tc_Vec2 v) {
  return atan2f(v.y, v.x);
}

vec_implement(3);
TCDEF void tc_vec3_cross(tc_Vec3 *r, const tc_Vec3 a, const tc_Vec3 b) {
  vec3_mul_cross(r->data, a.data, b.data);
}
TCDEF void tc_vec3_reflect(tc_Vec3 *r, tc_Vec3 const v, tc_Vec3 const n) {
  vec3_reflect(r->data, v.data, n.data);
}

vec_implement(4);

TCDEF void tc_vec4_cross(tc_Vec4 *r, tc_Vec4 a, tc_Vec4 b) {
  vec4_mul_cross(r->data, a.data, b.data);
}

TCDEF void tc_vec4_reflect(tc_Vec4 *r, tc_Vec4 v, tc_Vec4 n) {
  vec4_reflect(r->data, v.data, n.data);
}

TCDEF void tc_matrix_identity(tc_Matrix *m) {
  mat4x4_identity(m->data);
}

TCDEF void tc_matrix_print(tc_Matrix m) {
  for (int i = 0; i < 4; i++) {
    tc_vec4_print(m.v[i]);
  }
}

TCDEF void tc_matrix_clone(tc_Matrix *m, tc_Matrix s) {
  mat4x4_dup(m->data, s.data);
}

TCDEF int tc_matrix_equals(tc_Matrix a, tc_Matrix b) {
  for (int i = 0; i < 4; i++) {
    if (!tc_vec4_equals(a.v[i], b.v[i])) return 0;
  }
  return 1;
}

TCDEF void tc_matrix_row(tc_Vec4 *v, tc_Matrix m, int row) {
  mat4x4_row(v->data, m.data, row);
}

TCDEF void tc_matrix_col(tc_Vec4 *v, tc_Matrix m, int col) {
  mat4x4_col(v->data, m.data, col);
}

TCDEF void tc_matrix_transpose(tc_Matrix *m, tc_Matrix n) {
  mat4x4_transpose(m->data, n.data);
}

TCDEF void tc_matrix_add(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_add(m->data, a.data, b.data);
}

TCDEF void tc_matrix_sub(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_sub(m->data, a.data, b.data);
}

TCDEF void tc_matrix_scale(tc_Matrix *r, tc_Matrix m, float a) {
  mat4x4_scale(r->data, m.data, a);
}

TCDEF void tc_matrix_scale_aniso(tc_Matrix *m, tc_Matrix a, float x, float y, float z) {
  mat4x4_scale_aniso(m->data, a.data, x, y, z);
}

TCDEF void tc_matrix_mul(tc_Matrix *m, tc_Matrix a, tc_Matrix b) {
  mat4x4_mul(m->data, a.data, b.data);
}

TCDEF void tc_matrix_mul_vec4(tc_Vec4 *r, tc_Matrix a, tc_Vec4 b) {
  mat4x4_mul_vec4(r->data, a.data, b.data);
}

TCDEF void tc_matrix_translate(tc_Matrix *m, float x, float y, float z) {
  mat4x4_translate(m->data, x, y, z);
}

TCDEF void tc_matrix_translate_in_place(tc_Matrix *r, float x, float y, float z) {
  mat4x4_translate_in_place(r->data, x, y, z);
}

TCDEF void tc_matrix_from_vec3_mul_outer(tc_Matrix *m, tc_Vec3 a, tc_Vec3 b) {
  mat4x4_from_vec3_mul_outer(m->data, a.data, b.data);
}

TCDEF void tc_matrix_rotate(tc_Matrix *m, tc_Matrix a, float x, float y, float z, float angle) {
  mat4x4_rotate(m->data, a.data, x, y, z, angle);
}

TCDEF void tc_matrix_rotate_x(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_X(m->data, a.data, angle);
}

TCDEF void tc_matrix_rotate_y(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_Y(m->data, a.data, angle);
}

TCDEF void tc_matrix_rotate_z(tc_Matrix *m, tc_Matrix a, float angle) {
  mat4x4_rotate_Z(m->data, a.data, angle);
}

TCDEF void tc_matrix_invert(tc_Matrix *m, tc_Matrix a) {
  mat4x4_invert(m->data, a.data);
}


TCDEF void tc_matrix_translatev(tc_Matrix *m, tc_Vec3 v) {
  m->m03 += v.x;
  m->m13 += v.y;
  m->m23 += v.z;
}

TCDEF void tc_matrix_scaleV(tc_Matrix *m, tc_Vec3 v) {
  for (int i = 0; i < 3; i++) {
    m->data[i][i] *= v.data[i];
  }
}

TCDEF void tc_matrix_rotate2D(tc_Matrix *r, tc_Matrix m, float angle) {
  tc_matrix_rotate_z(r, m, angle);
}

TCDEF void tc_matrix_orthonormalize(tc_Matrix *r, tc_Matrix m) {
  mat4x4_orthonormalize(r->data, m.data);
}

TCDEF void tc_matrix_frustum(tc_Matrix *m, float l, float r, float b, float t, float n, float f) {
  mat4x4_frustum(m->data, l, r, b, t, n, f);
}

TCDEF void tc_matrix_ortho(tc_Matrix *m, float l, float r, float b, float t, float n, float f) {
  mat4x4_ortho(m->data, l, r, b, t, n, f);
}

TCDEF void tc_matrix_perspective(tc_Matrix *m, float y_fov, float aspect, float n, float f) {
  mat4x4_perspective(m->data, y_fov, aspect, n, f);
}

TCDEF void tc_matrix_look_at(tc_Matrix *m, tc_Vec3 eye, tc_Vec3 center, tc_Vec3 up) {
  mat4x4_look_at(m->data, eye.data, center.data, up.data);
}

#endif

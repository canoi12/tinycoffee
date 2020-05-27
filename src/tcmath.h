#ifndef TC_MATH_H
#define TC_MATH_H

#include "tinycoffee.h"

#define vec2_new(x, y) ((vec2){(x), (y)})
#define vec2_zero() ((vec2){0, 0})
#define vec2_from_vec3(v) ((vec2){(v.x), (v.y)})

#define vec3_new(x, y, z) ((vec3){(x), (y), (z)})
#define vec3_from_vec4(v) ((vec3){(v.x), (v.y), (v.z)})

#define vec4_new(x, y, z, w)((vec4){(x), (y), (z), (w)})

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
typedef struct vec##n { \
  union { \
    float data[n]; \
    struct { \
      vec##n##_field() \
    }; \
  }; \
} vec##n; \
TCDEF void vec##n##_add(vec##n *v, vec##n a, vec##n b); \
TCDEF void vec##n##_sub(vec##n *v, vec##n a, vec##n b); \
TCDEF void vec##n##_mul(vec##n *v, vec##n a, float number); \
TCDEF void vec##n##_div(vec##n *v, vec##n a, float number); \
TCDEF float vec##n##_dot(vec##n a, vec##n b); \
TCDEF void vec##n##_print(vec##n v); \
TCDEF int vec##n##_equals(vec##n a, vec##n b); \
TCDEF float vec##n##_length(vec##n v); \
TCDEF void vec##n##_normalize(vec##n *v, vec##n u); \
TCDEF float vec##n##_distance(vec##n a, vec##n b); \

#define vec_implement(n) \
TCDEF void vec##n##_add(vec##n *v, vec##n a, vec##n b) { \
  for (int i = 0; i < n; i++) \
    v->data[i] = a.data[i] + b.data[i]; \
} \
TCDEF void vec##n##_sub(vec##n *v, vec##n a, vec##n b) { \
  for (int i = 0; i < n; i++) \
    v->data[i] = a.data[i] - b.data[i]; \
} \
TCDEF void vec##n##_mul(vec##n *v, vec##n a, float number) { \
  for (int i = 0; i < n; i++) \
    v->data[i] = a.data[i] * number; \
} \
TCDEF void vec##n##_div(vec##n *v, vec##n a, float number) { \
  for (int i = 0; i < n; i++) \
    v->data[i] = a.data[i] / number; \
} \
TCDEF float vec##n##_dot(vec##n a, vec##n b) { \
  float d = 0; \
  for (int i = 0; i < n; i++) \
    d += a.data[i] * b.data[i]; \
  return d; \
} \
TCDEF void vec##n##_print(vec##n v) { \
  printf("vec%d(", n); \
  for (int i = 0; i < n-1; i++) \
    printf("%f, ", v.data[i]); \
  printf("%f)\n", v.data[n-1]); \
} \
TCDEF int vec##n##_equals(vec##n a, vec##n b) { \
  for (int i = 0; i < n; i++) \
    if (a.data[i] != b.data[i]) return 0; \
  return 1; \
} \
TCDEF float vec##n##_length(vec##n v) { \
  float len = 0; \
  for (int i = 0; i < n; i++) \
    len += v.data[i] * v.data[i]; \
  return sqrtf(len); \
} \
TCDEF void vec##n##_normalize(vec##n *v, vec##n u) { \
  float k = 1.f / vec##n##_length(u); \
  vec##n##_mul(v, u, k); \
} \
TCDEF float vec##n##_distance(vec##n a, vec##n b) { \
  vec##n s; \
  vec##n##_sub(&s, a, b); \
  float dist = vec##n##_length(s); \
  return dist; \
} \

// Vec2 defitions
vec_init(2)
TCDEF float vec2_perpdot(vec2 v, vec2 w);
TCDEF vec2 vec2_normal(vec2 v);
TCDEF vec2 vec2_projection(vec2 v, vec2 w);
TCDEF float vec2_angle(vec2 v);
// Vec3 definitions
vec_init(3)
// Vec4 defitions
vec_init(4)

typedef struct matrix_s {
  union {
    float data[4][4];
    vec4 v[4];
    struct {
      float m00, m10, m20, m30;
      float m01, m11, m21, m31;
      float m02, m12, m22, m32;
      float m03, m13, m23, m33;
    };
  };
} matrix;

// Matrix defitions
TCDEF void matrix_identity(matrix *m);
TCDEF void matrix_clone(matrix *m, matrix s);
TCDEF void matrix_row(vec4 *v, matrix m, int row);
TCDEF void matrix_translate(matrix *m, float x, float y, float z);
TCDEF void matrix_translatev(matrix *m, vec3 v);
TCDEF void matrix_scale(matrix *m, float x, float y, float z);
TCDEF void matrix_scaleV(matrix *m, vec3 v);
TCDEF void matrix_rotate(matrix *m, float x, float y, float z, float angle);
TCDEF void matrix_mul_vec4(vec4 *v, matrix m, vec4 a);
TCDEF void matrix_mul(matrix *m, matrix a, matrix b);
TCDEF void matrix_rotate_x(matrix *m, float angle);
TCDEF void matrix_rotate_y(matrix *m, float angle);
TCDEF void matrix_rotate_z(matrix *m, float angle);
TCDEF void matrix_rotate2D(matrix *m, float angle);
TCDEF void matrix_ortho(matrix *m, float l, float r, float b, float t, float n, float f);

#endif /* TC_MATH_H */

#if defined(TC_MATH_IMPLEMENTATION)

vec_implement(2);

TCDEF float vec2_perpdot(vec2 v, vec2 w) {
  return (v.x * w.y) - (v.y * w.x);
}

TCDEF vec2 vec2_normal(vec2 v)
{
  return vec2_new(-v.y, v.x);
}

TCDEF vec2 vec2_projection(vec2 v, vec2 u) {
  float dot = vec2_dot(v, u);
  float len = vec2_length(u);
  float mag = len * len;
  vec2 r;
  vec2_mul(&r, u, (dot / mag));
  return r;
}

TCDEF float vec2_angle(vec2 v) {
  return atan2f(v.y, v.x);
}

vec_implement(3);
vec_implement(4);

TCDEF void matrix_identity(matrix *m) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m->data[i][j] = i == j ? 1.f : 0.f;
    }
  }
}

TCDEF void matrix_clone(matrix *m, matrix s) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m->data[i][j] = s.data[i][j];
    }
  }
}

TCDEF void matrix_row(vec4 *v, matrix m, int row) {
  for (int i = 0; i < 4; i++)
    v->data[i] = m.data[i][row];
}

TCDEF void matrix_translate(matrix *m, float x, float y, float z) {
  vec4 t = vec4_new(x, y, z, 0);
  vec4 row;
  for (int i = 0; i < 4; i++) {
    matrix_row(&row, *m, i);
    m->data[3][i] += vec4_dot(row, t);
  }
}

TCDEF void matrix_translatev(matrix *m, vec3 v) {
  m->m03 += v.x;
  m->m13 += v.y;
  m->m23 += v.z;
}

TCDEF void matrix_scale(matrix *m, float x, float y, float z) {

  matrix u;
  matrix_clone(&u, *m);
  vec4_mul(&m->v[0], m->v[0], x);
  vec4_mul(&m->v[1], m->v[1], y);
  vec4_mul(&m->v[2], m->v[2], z);
  for (int i = 0; i < 3; i++)
    m->data[3][i] = u.data[3][i];
}

TCDEF void matrix_scaleV(matrix *m, vec3 v) {
  for (int i = 0; i < 3; i++) {
    m->data[i][i] *= v.data[i];
  }
}

TCDEF void matrix_rotate(matrix *m, float x, float y, float z, float angle) {

}

TCDEF void matrix_mul_vec4(vec4 *v, matrix m, vec4 a) {
  for (int i = 0; i < 4; i++) {
    int p = 0;
    for (int j = 0; j < 4; j++) {
      p += m.data[i][j] * a.data[i];
    }
    v->data[i] = p;
  }
}

TCDEF void matrix_mul(matrix *m, matrix a, matrix b) {
  for (int k = 0; k < 4; k++) {
    for (int i = 0; i < 4; i++) {
      int p = 0;
      for (int j = 0; j < 4; j++) {
        p += a.data[j][i] * b.data[k][j];
      }
      m->data[k][i] = p;
    }
  }
}

TCDEF void matrix_rotate_x(matrix *m, float angle) {}
TCDEF void matrix_rotate_y(matrix *m, float angle) {}
TCDEF void matrix_rotate_z(matrix *m, float angle) {
  float c = cosf(angle);
  float s = sinf(angle);
  m->data[0][0] = c;
  m->data[0][1] = -s;
  m->data[1][0] = s;
  m->data[1][1] = c;
}


TCDEF void matrix_rotate2D(matrix *m, float angle) {
  matrix_rotate_z(m, angle);
}

TCDEF void matrix_ortho(matrix *m, float l, float r, float b, float t, float n, float f) {
  m->data[0][0] = 2.f/(r-l);
	m->data[0][1] = m->data[0][2] = m->data[0][3] = 0.f;

	m->data[1][1] = 2.f/(t-b);
	m->data[1][0] = m->data[1][2] = m->data[1][3] = 0.f;

	m->data[2][2] = -2.f/(f-n);
	m->data[2][0] = m->data[2][1] = m->data[2][3] = 0.f;

	m->data[3][0] = -(r+l)/(r-l);
	m->data[3][1] = -(t+b)/(t-b);
	m->data[3][2] = -(f+n)/(f-n);
	m->data[3][3] = 1.f;
}

#endif

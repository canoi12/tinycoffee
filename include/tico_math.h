#include "tico.h"

/*======================
 * Math Module
 *======================*/

#define tico_vec2(x, y) ((tc_Vec2){(x), (y)})
#define tico_vec2_zero() ((tc_Vec2){0, 0})
#define tico_vec2_from_vec3(v) ((tc_Vec2){(v.x), (v.y)})

#define tico_vec3(x, y, z) ((tc_Vec3){(x), (y), (z)})
#define tico_vec3_from_vec4(v) ((tc_Vec3){(v.x), (v.y), (v.z)})

#define tico_vec4(x, y, z, w)((tc_Vec4){(x), (y), (z), (w)})

#define vec2_field() \
  float x; \
  float y; \

#define vec3_field() \
  vec2_field() \
  float z; \

#define vec4_field() \
  vec3_field() \
  float w;

#define vec_struct(n) \
typedef struct tc_Vec##n { \
  union { \
    float data[n]; \
    struct { \
      vec##n##_field() \
    }; \
  }; \
} tc_Vec##n; \

vec_struct(2);
vec_struct(3);
vec_struct(4);

typedef struct tc_Matrix {
  union {
    float data[4][4];
    tc_Vec4 v[4];
    struct {
      float m00, m10, m20, m30;
      float m01, m11, m21, m31;
      float m02, m12, m22, m32;
      float m03, m13, m23, m33;
    };
  };
} tc_Matrix;


#define vecn_init(n) \
TIC_API void tico_vec##n##_add(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_sub(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_mul(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TIC_API void tico_vec##n##_div(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TIC_API float tico_vec##n##_dot(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_print(const tc_Vec##n v); \
TIC_API int tico_vec##n##_equals(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API float tico_vec##n##_len(const tc_Vec##n v); \
TIC_API void tico_vec##n##_normalize(tc_Vec##n *v, const tc_Vec##n u); \
TIC_API float tico_vec##n##_distance(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_min(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_max(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tico_vec##n##_lerp(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b, const float t);

vecn_init(2);
TIC_API float tico_vec2_perpdot(const tc_Vec2 v, const tc_Vec2 w);
TIC_API void tico_vec2_normal(tc_Vec2 *r, const tc_Vec2 v);
TIC_API void tico_vec2_projection(tc_Vec2 *r, const tc_Vec2 v, const tc_Vec2 w);
TIC_API float tico_vec2_angle(const tc_Vec2 v);
vecn_init(3);
TIC_API void tico_vec3_cross(tc_Vec3 *r, const tc_Vec3 a, const tc_Vec3 b);
TIC_API void tico_vec3_reflect(tc_Vec3 *r, tc_Vec3 const v, tc_Vec3 const n);
vecn_init(4);
TIC_API void tico_vec4_cross(tc_Vec4 *r, const tc_Vec4 a, const tc_Vec4 b);
TIC_API void tico_vec4_reflect(tc_Vec4 *r, const tc_Vec4 v, const tc_Vec4 n);

// Matrix defitions
TIC_API void tico_matrix_identity(tc_Matrix *m);
TIC_API void tico_matrix_clone(tc_Matrix *m, tc_Matrix s);
TIC_API int tico_matrix_equals(tc_Matrix a, tc_Matrix b);
TIC_API void tico_matrix_print(tc_Matrix m);
TIC_API void tico_matrix_row(tc_Vec4 *v, tc_Matrix m, int row);
TIC_API void tico_matrix_transpose(tc_Matrix *m, tc_Matrix n);
TIC_API void tico_matrix_translate(tc_Matrix *r, float x, float y, float z);
TIC_API void tico_matrix_translate_in_place(tc_Matrix *r, float x, float y, float z);
TIC_API void tico_matrix_translatev(tc_Matrix *r, tc_Vec3 v);
TIC_API void tico_matrix_scale(tc_Matrix *r, tc_Matrix m, float s);
TIC_API void tico_matrix_scale_aniso(tc_Matrix *m, tc_Matrix a, float x, float y, float z);
TIC_API void tico_matrix_scaleV(tc_Matrix *r, tc_Vec3 v);
TIC_API void tico_matrix_from_vec3_mul_outer(tc_Matrix *m, tc_Vec3 a, tc_Vec3 b);
TIC_API void tico_matrix_mul_vec4(tc_Vec4 *v, tc_Matrix m, tc_Vec4 n);
TIC_API void tico_matrix_mul(tc_Matrix *r, tc_Matrix m, tc_Matrix n);
TIC_API void tico_matrix_rotate(tc_Matrix *r, tc_Matrix m, float x, float y, float z, float angle);
TIC_API void tico_matrix_rotate_x(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tico_matrix_rotate_y(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tico_matrix_rotate_z(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tico_matrix_rotate2D(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tico_matrix_invert(tc_Matrix *r, tc_Matrix m);
TIC_API void tico_matrix_orthonormalize(tc_Matrix *r, tc_Matrix m);
TIC_API void tico_matrix_frustum(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TIC_API void tico_matrix_ortho(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TIC_API void tico_matrix_perspective(tc_Matrix *m, float y_fov, float aspect, float n, float f);
TIC_API void tico_matrix_look_at(tc_Matrix *m, tc_Vec3 eye, tc_Vec3 center, tc_Vec3 up);

TIC_API float tico_lerp(float a, float b, float t);
TIC_API float tico_round(float n);
TIC_API float tico_distance(float a, float b);
TIC_API int tico_sign(float a);
// TIC_API float tico_clamp(float v, float lo, float hi);
TIC_API float tico_angle(float x0, float y0, float x1, float y1);
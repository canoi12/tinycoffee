#ifndef TICO_DEFINITIONS_H
#define TICO_DEFINITIONS_H

#define TICO_VERSION "0.3.0"
#define PACK_NAME "data.pack"
#define PROJECT_FILE "project.tico"

#ifndef TIC_API
	#if defined(_WIN32)
		#if defined(BUILD_SHARE)
	  	#define TIC_API __declspec(dllexport)
    #elif defined(USE_SHARED)
      #define TIC_API __declspec(dllimport)
    #else
      #define TIC_API
    #endif
  #else
    #define TIC_API
  #endif
#endif

#ifndef TIC_MALLOC
  #define TIC_MALLOC malloc
#endif
#ifndef TIC_FREE
  #define TIC_FREE free
#endif
#ifndef TIC_CALLOC
  #define TIC_CALLOC calloc
#endif
#ifndef TIC_REALLOC
  #define TIC_REALLOC realloc
#endif

#define STR(x) #x
#define ASSERT(s) if (!(s)) {TRACELOG("Assertion '%s' failed", STR(s)); exit(-1);}

#define TRACEERR(message...) tic_tracelog(1, __FILE__, __PRETTY_FUNCTION__, __LINE__, message)
#define ERRLOG(message...) tic_log(1, message)

#define TRACELOG(message...) tic_tracelog(0, __FILE__, __PRETTY_FUNCTION__, __LINE__, message)
#define LOG(message...) tic_log(0, message)

typedef int tc_bool;
#define tc_false 0
#define tc_true 1

#define PI 3.14159
#define deg2rad(a) ((a) * PI/180)
#define rad2deg(a) ((a) * 180/PI)

#define tico_max(a,b) ((a) > (b) ? (a) : (b))
#define tico_min(a,b) ((a) < (b) ? (a) : (b))
#define tico_clamp(v, mi, ma) (tico_min((ma), tico_max((mi), (v))))

#define tico_rgb(r, g, b) tico_rgba((r), (g), (b), 255)
#define tico_rgba(r, g, b, a) ((tc_Color){(r), (g), (b), (a)})
#define tico_color(r, g, b, a) tico_rgba((r), (g), (b), (a))
#define tico_color3(r, g, b) tico_rgb((r), (g), (b))

#define WHITE tico_rgb(255, 255, 255)
#define BLACK tico_rgb(0, 0, 0)
#define BLUE tico_rgb(48, 52, 109)
#define RED tico_rgb(208, 70, 72)
#define GREEN tico_rgb(52, 101, 36)
#define GRAY tico_rgb(78, 74, 78)
#define BROWN tico_rgb(133, 76, 48)
#define BG tico_rgb(75, 90, 90)

#define tico_vertex(x, y, s, t) ((tc_Vertex){(x), (y), 1.f, 1.f, 1.f, 1.f, (s), (t)})
#define tico_vertexc(x, y, col, s, t) ((tc_Vertex){(x), (y), (col.r)/255.f, (col.g)/255.f, (col.b)/255.f, (col.a)/255.f, (s), (t)})

#define tico_rectf(x, y, w, h) ((tc_Rectf){(x), (y), (w), (h)})
#define tico_rect(x, y, w, h) ((tc_Rect){(x), (y), (w), (h)})

#define LUA_CLASS(name) #name

#define INTERNAL_API(function,...) \
  function##_internal(__VA_ARGS__)

#define PUBLIC_API(function,...) \
  function (__VA_ARGS__)

// #define PUBLIC_API_IMPL(function, ...)
  // #define teste tico_graphics_draw_internal()

// #define PUBLI

typedef unsigned char byte;

#define rect_t(T) \
  struct { \
    union { \
      T data[4]; \
      struct { \
        T x; \
        T y; \
        T w; \
        T h; \
      }; \
    }; \
  }

typedef rect_t(int) tc_Rect;
typedef rect_t(float) tc_Rectf;

typedef struct tc_Color {
  union
  {
    byte data[4];
    struct
    {
      byte r;
      byte g;
      byte b;
      byte a;
    };
  };
} tc_Color;

#include "GL/gl3w.h"
#define GLFW_INCLUDE_NONE
#include "glfw/include/GLFW/glfw3.h"

#endif // TICO_DEFINITIONS_H
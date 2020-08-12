#ifndef TICO_H
#define TICO_H

// #define WREN_LANG

// #define WREN_LANG


#ifndef TICO_NO_INCLUDE
  #include <stdio.h>
  #include <stdlib.h>
  #include <math.h>
  #include <string.h>
  #include <time.h>

  #include "external/GL/gl3w.h"

  #define GLFW_INCLUDE_NONE
  #include "external/glfw/include/GLFW/glfw3.h"

  #include "external/cjson/cJSON.h"

  #include "external/hashmap.h"

  #include "external/miniaudio.h"

  #include "external/stb_truetype.h"

  #include "external/map.h"

  #ifndef LUAJIT
    #include "external/lua/src/lua.h"
    #include "external/lua/src/lauxlib.h"
    #include "external/lua/src/lualib.h"
  #else
    #include "external/luajit/src/lua.h"
    #include "external/luajit/src/lauxlib.h"
    #include "external/luajit/src/lualib.h"
    #include "external/luajit/src/luaconf.h"
    #include "external/luajit/src/luajit.h"
  #endif

  #ifdef WREN_LANG
  #include "external/wren/src/include/wren.h"
  #endif
#else
  typedef void* GLFWwindow;
  typedef void* cJSON;
  typedef void* hashmap;
  typedef void* ma_decoder;
  typedef void* stbtt_fontinfo;
  typedef void* mu_Context;

  #ifdef WREN_LANG
  typedef void* WrenVM;
  #endif
#endif

#define TICO_VERSION "0.2.0"

#include "utils.h"

#ifndef TIC_API
  #if defined(_WIN32)
    #if defined(BUILD_SHARED)
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

#define DEFAULT_MAX_QUADS 10000
#define DEFAULT_MAX_VERTICES 4 * DEFAULT_MAX_QUADS
#define DEFAULT_MAX_INDICES 6 * DEFAULT_MAX_QUADS
#define MAX_DRAW_CALLS 128
#define MATRIX_STACK_SIZE 32

#define SHADER_STACK_SIZE 32
#define CANVAS_STACK_SIZE 32

#define DEFAULT_MAX_TEXTURES 64
#define DEFAULT_MAX_SOUNDS 32
#define DEFAULT_MAX_FONTS 32


#define MAXFONTCHAR 256

#ifndef AUDIO_DEVICE_FORMAT
  #define AUDIO_DEVICE_FORMAT ma_format_f32
#endif

#ifndef AUDIO_DEVICE_CHANNELS
  #define AUDIO_DEVICE_CHANNELS 2
#endif

#ifndef AUDIO_DEVICE_SAMPLE_RATE
  #define AUDIO_DEVICE_SAMPLE_RATE 44100
#endif

#ifndef MAX_AUDIO_BUFFER_CHANNELS
  #define MAX_AUDIO_BUFFER_CHANNELS 255
#endif

typedef int tc_bool;

#define tc_false 0
#define tc_true 1

typedef unsigned char byte;
typedef unsigned char tc_uint8;

#define tic_vertex(x, y, s, t) ((tc_Vertex){(x), (y), 1.f, 1.f, 1.f, 1.f, (s), (t)})
#define tic_vertexc(x, y, col, s, t) ((tc_Vertex){(x), (y), (col.r)/255.f, (col.g)/255.f, (col.b)/255.f, (col.a)/255.f, (s), (t)})
#define tic_rgba(r, g, b, a) ((tc_Color){(r), (g), (b), (a)})
#define tic_rgb(r, g, b) ((tc_Color){(r), (g), (b), 255})
#define tic_color(r, g, b, a) tic_rgba(r, g, b, a)
#define tic_color3(r, g, b) tic_rgb(r, g, b)
#define tic_rectf(x, y, w, h) ((tc_Rectf){(x), (y), (w), (h)})
#define tic_rect(x, y, w, h) ((tc_Rect){(x), (y), (w), (h)})

#define tic_vec2_new(x, y) ((tc_Vec2){(x), (y)})
#define tic_vec2_zero() ((tc_Vec2){0, 0})
#define tic_vec2_from_vec3(v) ((tc_Vec2){(v.x), (v.y)})

#define tic_vec3_new(x, y, z) ((tc_Vec3){(x), (y), (z)})
#define tic_vec3_from_vec4(v) ((tc_Vec3){(v.x), (v.y), (v.z)})

#define tic_vec4_new(x, y, z, w)((tc_Vec4){(x), (y), (z), (w)})

#define PI 3.14159

#define deg2rad(a) ((a) * PI/180)
#define rad2deg(a) ((a) * 180/PI)

#define LUA_CLASS(name) #name
#define WHITE tic_rgb(255, 255, 255)
#define BLACK tic_rgb(0, 0, 0)
#define BLUE tic_rgb(48, 52, 109)
#define RED tic_rgb(208, 70, 72)
#define GREEN tic_rgb(52, 101, 36)
#define GRAY tic_rgb(78, 74, 78)
#define BROWN tic_rgb(133, 76, 48)
#define BG tic_rgb(75, 90, 90)

#define tic_max(a, b) ((a) < (b) ? (b) : (a))
#define tic_min(a, b) ((a) < (b) ? (a) : (b))

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

typedef enum {
  TIC_FILE = 0,
  TIC_DIRECTORY,
  TIC_SYMLINK
} TIC_FILE_TYPE;

typedef enum {
  TIC_WRITE = 0,
  TIC_APPEND,
  TIC_BINARY
} TIC_WRITE_MODE;

typedef enum {
  TIC_UNIFORM_INT,
  TIC_UNIFORM_VEC2I,
  TIC_UNIFORM_VEC3I,
  TIC_UNIFORM_VEC4I,
  TIC_UNIFORM_FLOAT,
  TIC_UNIFORM_VEC2,
  TIC_UNIFORM_VEC3,
  TIC_UNIFORM_VEC4,
  TIC_UNIFORM_MATRIX
} TIC_SHADER_UNIFORM_;

typedef enum {
  TIC_TRIANGLES = 0,
  TIC_LINES
} TIC_DRAW_MODE;

typedef enum {
  TIC_INIT_ALL = 0,
  TIC_INIT_GRAPHICS,
  TIC_INIT_INPUT,
  TIC_INIT_FILESYSTEM,
  TIC_INIT_WREN,
  TIC_INIT_LUA
} TIC_INIT_FLAGS;

typedef enum {
  TIC_MODE_EDITOR = 1,
  TIC_MODE_GAME
} TIC_MODE;

typedef enum {
  TIC_RENDERMODE_STRETCH = 1,
  TIC_RENDERMODE_2D
} TIC_RENDER_MODE;

typedef enum {
  TIC_AUDIO_STREAM = 0,
  TIC_AUDIO_STATIC
} TIC_AUDIO_USAGE;

typedef enum {
  KEY_UNKNOWN =       -1,
  KEY_SPACE =         32,
  KEY_APOSTROPHE =    39,  /* ' */
  KEY_COMMA =         44, /* , */
  KEY_MINUS =         45,  /* - */
  KEY_PERIOD =        46,  /* . */
  KEY_SLASH =         47,  /* / */
  KEY_0 =             48,
  KEY_1 =             49,
  KEY_2 =             50,
  KEY_3 =             51,
  KEY_4 =             52,
  KEY_5 =             53,
  KEY_6 =             54,
  KEY_7 =             55,
  KEY_8 =             56,
  KEY_9 =             57,
  KEY_SEMICOLON =     59,  /* ; */
  KEY_EQUAL =         61,  /* = */
  KEY_A =             65,
  KEY_B =             66,
  KEY_C =             67,
  KEY_D =             68,
  KEY_E =             69,
  KEY_F =             70,
  KEY_G =             71,
  KEY_H =             72,
  KEY_I =             73,
  KEY_J =             74,
  KEY_K =             75,
  KEY_L =             76,
  KEY_M =             77,
  KEY_N =             78,
  KEY_O =             79,
  KEY_P =             80,
  KEY_Q =             81,
  KEY_R =             82,
  KEY_S =             83,
  KEY_T =             84,
  KEY_U =             85,
  KEY_V =             86,
  KEY_W =             87,
  KEY_X =             88,
  KEY_Y =             89,
  KEY_Z =             90,
  KEY_LEFT_BRACKET =  91,  /* [ */
  KEY_BACKSLASH =     92,  /* \ */
  KEY_RIGHT_BRACKET = 93,  /* ] */
  KEY_GRAVE_ACCENT =  96,  /* ` */
  KEY_WORLD_1 =       161, /* non-US #1 */
  KEY_WORLD_2 =       162, /* non-US #2 */

/* Function keys */
  KEY_ESCAPE =        256,
  KEY_ENTER =         257,
  KEY_TAB =           258,
  KEY_BACKSPACE =     259,
  KEY_INSERT =        260,
  KEY_DELETE =        261,
  KEY_RIGHT =         262,
  KEY_LEFT =          263,
  KEY_DOWN =          264,
  KEY_UP =            265,
  KEY_PAGE_UP =       266,
  KEY_PAGE_DOWN =     267,
  KEY_HOME =          268,
  KEY_END =           269,
  KEY_CAPS_LOCK =     280,
  KEY_SCROLL_LOCK =   281,
  KEY_NUM_LOCK =      282,
  KEY_PRINT_SCREEN =  283,
  KEY_PAUSE =         284,
  KEY_F1 =            290,
  KEY_F2 =            291,
  KEY_F3 =            292,
  KEY_F4 =            293,
  KEY_F5 =            294,
  KEY_F6 =            295,
  KEY_F7 =            296,
  KEY_F8 =            297,
  KEY_F9 =            298,
  KEY_F10 =           299,
  KEY_F11 =           300,
  KEY_F12 =           301,
  KEY_F13 =           302,
  KEY_F14 =           303,
  KEY_F15 =           304,
  KEY_F16 =           305,
  KEY_F17 =           306,
  KEY_F18 =           307,
  KEY_F19 =           308,
  KEY_F20 =           309,
  KEY_F21 =           310,
  KEY_F22 =           311,
  KEY_F23 =           312,
  KEY_F24 =           313,
  KEY_F25 =           314,
  KEY_KP_0 =          320,
  KEY_KP_1 =          321,
  KEY_KP_2 =          322,
  KEY_KP_3 =          323,
  KEY_KP_4 =          324,
  KEY_KP_5 =          325,
  KEY_KP_6 =          326,
  KEY_KP_7 =          327,
  KEY_KP_8 =          328,
  KEY_KP_9 =          329,
  KEY_KP_DECIMAL =    330,
  KEY_KP_DIVIDE =     331,
  KEY_KP_MULTIPLY =   332,
  KEY_KP_SUBTRACT =   333,
  KEY_KP_ADD =        334,
  KEY_KP_ENTER =      335,
  KEY_KP_EQUAL =      336,
  KEY_LEFT_SHIFT =    340,
  KEY_LEFT_CONTROL =  341,
  KEY_LEFT_ALT =      342,
  KEY_LEFT_SUPER =    343,
  KEY_RIGHT_SHIFT =   344,
  KEY_RIGHT_CONTROL = 345,
  KEY_RIGHT_ALT =     346,
  KEY_RIGHT_SUPER =   347,
  KEY_MENU =          348,

  KEY_LAST = KEY_MENU,
  KEY_COUNT
} TIC_KEY;

typedef enum {
  MOUSE_LEFT =   0,
  MOUSE_RIGHT =  1,
  MOUSE_MIDDLE = 2,
  MOUSE_BUTTON_LAST = 3,
  MOUSE_BUTTON_COUNT
} TIC_MOUSEBUTTON;

typedef enum {
  TIC_JOY_1  = 0,
  TIC_JOY_2  = 1,
  TIC_JOY_3  = 2,
  TIC_JOY_4  = 3,
  TIC_JOY_5  = 4,
  TIC_JOY_6  = 5,
  TIC_JOY_7  = 6,
  TIC_JOY_8  = 7,
  TIC_JOY_9  = 8,
  TIC_JOY_10 = 9,
  TIC_JOY_11 = 10,
  TIC_JOY_12 = 11,
  TIC_JOY_13 = 12,
  TIC_JOY_14 = 13,
  TIC_JOY_15 = 14,
  TIC_JOY_16 = 15,
  TIC_JOY_COUNT,
  TIC_JOY_LAST = TIC_JOY_16,
} TIC_JOYSTICKS;

typedef enum {
  JOY_BUTTON_A =             0,
  JOY_BUTTON_B =             1,
  JOY_BUTTON_X =             2,
  JOY_BUTTON_Y =             3,
  JOY_BUTTON_LEFT_BUMPER =   4,
  JOY_BUTTON_RIGHT_BUMPER =  5,
  JOY_BUTTON_BACK =          6,
  JOY_BUTTON_START =         7,
  JOY_BUTTON_GUIDE =         8,
  JOY_BUTTON_LEFT_THUMB =    9,
  JOY_BUTTON_RIGHT_THUMB =   10,
  JOY_BUTTON_DPAD_UP =       11,
  JOY_BUTTON_DPAD_RIGHT =    12,
  JOY_BUTTON_DPAD_DOWN =     13,
  JOY_BUTTON_DPAD_LEFT =     14,
  JOY_BUTTON_COUNT,
  JOY_BUTTON_LAST =          JOY_BUTTON_DPAD_LEFT,
} TIC_JOYSTICK_BUTTON;

typedef enum {
  JOY_AXIS_LEFT_X  =         0,
  JOY_AXIS_LEFT_Y =          1,
  JOY_AXIS_RIGHT_X =         2,
  JOY_AXIS_RIGHT_Y =         3,
  JOY_AXIS_LEFT_TRIGGER =    4,
  JOY_AXIS_RIGHT_TRIGGER =   5,
  JOY_AXIS_COUNT,
  JOY_AXIS_LAST =            JOY_AXIS_RIGHT_TRIGGER,
} TIC_JOYSTICK_AXES;

typedef enum {
  TIC_INPUT_INIT_ALL = 0,
  TIC_INIT_KEYBOARD = 1,
  TIC_INIT_MOUSE = 1 << 2,
  TIC_INIT_JOYSTICK = 1 << 3
} TIC_INPUT_FLAGS;

typedef enum {
  TIC_WINDOW_DEFAULT = 0,
  TIC_WINDOW_FULLSCREEN = 1 << 1,
  TIC_WINDOW_RESIZABLE = 1 << 2,
  TIC_WINDOW_VSYNC = 1 << 3,
} TIC_WINDOW_FLAGS;

typedef enum {
  TIC_DEFAULT_FRAGMENT = 0,
  TIC_GBA_FRAG_SHADER,
  TIC_COLOR8_FRAG_SHADER,
  TIC_COLOR16_FRAG_SHADER,
  TIC_OUTLINE_FRAG_SHADER
} TIC_FRAGMENT_SHADERS;

typedef enum {
  TIC_DEFAULT_VERTEX = 0
} TIC_VERTEX_SHADERS;

typedef enum {
  TIC_LUA_GAME = 0,
  TIC_WREN_GAME
} TIC_GAME_LANG;

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

typedef struct tc_Color {
  union {
    byte data[4];
    struct {
      byte r;
      byte g;
      byte b;
      byte a;
    };
  };
} tc_Color;

typedef struct tc_Rect {
  union {
    int data[4];
    struct {
      int x;
      int y;
      int w;
      int h;
    };
  };
} tc_Rect;

typedef struct tc_Rectf {
  union {
    float data[4];
    struct {
      float x;
      float y;
      float w;
      float h;
    };
  };
} tc_Rectf;

typedef struct tc_AudioData {
  TIC_AUDIO_USAGE usage;
  unsigned char *data;
  unsigned char *filename;
  size_t size;
  int refs;
} tc_AudioData;

typedef struct {
  ma_decoder decoder;
  unsigned char id;

  float volume;
  float pitch;

  int playing;
  int paused;
  int loop;
  int loaded;

//   TIC_AUDIO_USAGE usage;

//   unsigned char *data;
//   size_t dataSize;

  tc_AudioData *data;

  size_t currentReadPos;
} tc_AudioBuffer;

typedef struct tc_Sound {
  tc_AudioBuffer *audioBuffer;
} tc_Sound;

typedef struct tc_Texture {
  unsigned int id;
  int width;
  int height;
  int filter[2];
  int wrap[2];
  tc_bool packed;
  struct {
    unsigned int id;
    int x;
    int y;
  } pack;
} tc_Texture;

typedef struct tc_TextureData {
  unsigned int id;
} tc_TextureData;

typedef struct tc_Image {
  tc_Texture texture;
  int width;
  int height;
} tc_Image;

typedef struct tc_Font tc_Font;

typedef struct tc_Canvas {
  unsigned int id;
  tc_Texture texture;
  int width;
  int height;
} tc_Canvas;

typedef struct {
  int ax; // advance.x
  int ay; // advance.y

  int bw; // bitmap.width;
  int bh; // bitmap.rows;

  int bl; // bitmap_left;
  int bt; // bitmap_top;

  float tx; // x offset of glyph in texture coordinates
} tc_CharacterInfo;


typedef struct tc_Font {
    tc_Texture texture;
    tc_CharacterInfo c[MAXFONTCHAR];

    tc_uint8 size;

    stbtt_fontinfo info;
    float ptsize;
    float scale;
    int baseline;
    void *data;

    unsigned int atlasVao;
    unsigned int atlasVbo;
} tc_Font;

typedef struct tc_Shader {
  unsigned int program;
} tc_Shader;

enum {
  TIC_CURSOR_ARROW = 0,
  TIC_CURSOR_TEXT,
  TIC_CURSOR_CROSSHAIR,
  TIC_CURSOR_HAND,
  TIC_CURSOR_HRESIZE,
  TIC_CURSOR_VRESIZE
};

typedef struct {
  struct {
    map_int_t mouseButtonNames;
    map_int_t keyNames;
    map_int_t joyButtonNames;
    map_int_t joyAxisNames;
  } names;
  struct {
    int x;
    int y;
    int fixX;
    int fixY;
    float scrollX;
    float scrollY;
    tc_Vec2 scroll;
    tc_Vec2 scroll_delta;
    tc_Vec2 pos;
    tc_Vec2 pos_delta;
    tc_bool down[MOUSE_BUTTON_LAST];
    tc_bool pressed[MOUSE_BUTTON_LAST];
    tc_bool fixed;
    tc_bool active;
    hashmap buttonNames;

    GLFWcursor *cursors[6];
  } mouseState;
  struct {
    tc_bool down[KEY_LAST];
    tc_bool pressed[KEY_LAST];
    tc_bool active;
    hashmap keyNames;
  } keyState;
  struct {
    tc_bool down[JOY_BUTTON_COUNT];
    tc_bool pressed[JOY_BUTTON_COUNT];
    float axis[JOY_AXIS_COUNT];
    tc_bool active;
  } joystickState[TIC_JOY_COUNT];
} tc_Input;

typedef struct {
  char projectPath[256];
  char exePath[256];
  tc_uint8* (*readFile)(const tc_uint8*, size_t*);
  void (*writeFile)(const tc_uint8*, const tc_uint8*, size_t, TIC_WRITE_MODE);
  tc_bool (*fileExists)(const tc_uint8*);
  tc_bool packed;
} tc_Filesystem;

typedef struct tc_Window {
  GLFWwindow *handle;

  char title[256];
  int x;
  int y;
  int width;
  int height;

  tc_Rect bkpMode;

  struct {
    tc_bool fullscreen;
    tc_bool resizable;
    tc_bool vsync;
  } state;
} tc_Window;

typedef struct {
  union {
    float data[8];
    struct {
      float pos[2];
      float color[4];
      float uv[2];
    };
    struct {
      float x;
      float y;
      float r;
      float g;
      float b;
      float a;
      float s;
      float t;
    };
  };
} tc_Vertex;

typedef struct {
  unsigned int indexStart;
  unsigned int indexCount;
  unsigned int vertexCount;
  TIC_DRAW_MODE mode;

  unsigned int textureId;

  tc_Matrix modelview;

  tc_Rect clip;

} tc_DrawCall;

typedef struct {
  unsigned int *indices;
  tc_Vertex *vertices;
  tc_Vertex *verticesPtr;

  tc_DrawCall drawCalls[MAX_DRAW_CALLS];
  unsigned int drawCallIndex;

  unsigned int totalIndexCount;
  unsigned int totalVertexCount;

  unsigned int maxQuads;
  unsigned int maxVertices;
  unsigned int maxIndices;

  unsigned int vao;
  unsigned int vbo[2];

  tc_Rect currentClip;
  tc_Texture currentTexture;
} tc_Batch;

typedef stack_arr_t(tc_Canvas, CANVAS_STACK_SIZE) stack_canvas_t;
typedef stack_arr_t(tc_Shader, SHADER_STACK_SIZE) stack_shader_t;
typedef stack_arr_t(tc_Matrix, MATRIX_STACK_SIZE) stack_matrix_t;

typedef struct tc_Render {
  tc_Batch batch;
  struct {
    tc_Shader defaultShader;
    tc_Shader currentShader;

    int canvasIndex;
    int shaderIndex;
    // tc_Shader shaderStack[SHADER_STACK_SIZE];
    // tc_Canvas canvasStack[CANVAS_STACK_SIZE];
    stack_canvas_t canvasStack;
    stack_shader_t shaderStack;
    stack_matrix_t matrixStack;

    int vertexShaders[32];
    int fragmentShaders[32];

    int defaultVertexShader;
    int defaultFragmentShader;

    unsigned int defaultTextureId;
    unsigned int currentTextureId;
    tc_Texture shapeTexture;

    unsigned int drawCount;
    tc_Canvas currentCanvas;

    tc_Matrix modelview;
    tc_Canvas defaultCanvas;

    tc_Matrix *camera;

    // tc_Matrix matrixStack[MATRIX_STACK_SIZE];
    int currentMatrix;

    void (*backToDefaultCanvas)(void);
  } state;
} tc_Render;

typedef struct {
  double currentTime;
  double lastTime;
  double fpsLastTime;
  float delta;
  int fps;
  int frames;
} tc_Timer;

typedef struct {
  lua_State *L;
  tc_bool mainLoaded;
} tc_Lua;

#ifdef WREN_LANG
typedef struct tc_WrenTraceback tc_WrenTraceback;

struct tc_WrenTraceback {
  char message[512];
  tc_WrenTraceback *next;
};

typedef struct {
  WrenVM *vm;
  WrenConfiguration config;
  WrenHandle *loadHandle;
  WrenHandle *stepHandle;
  WrenHandle *errorHandle;

  WrenHandle *classHandle;
  tc_WrenTraceback *trace;

  tc_bool mainLoaded;
} tc_Wren;
#endif

typedef enum {
  TIC_RESOURCE_IMAGE = (1 << 0),
  TIC_RESOURCE_SOUND = (1 << 1),
  TIC_RESOURCE_FONT = (1 << 2),
  TIC_RESOURCE_TILESET = (1 << 3),
  TIC_RESOURCE_TILEMAP = (1 << 4),
  TIC_RESOURCE_LUA = (1 << 5),
  TIC_RESOURCE_WREN = (1 << 6)
} TIC_RESOURCE_TYPE;

#include "ui/microui.h"
typedef void* mu_Font;

typedef struct {
  struct mu_Context *ctx;
  tc_Font font;
  struct {
    tc_Texture tex;
    tc_Rect rect[9];
  } icons;
  struct {
    int *animation;
    int *tileset;
    int *tilemap;
  } windows;
  unsigned char ** names;
} tc_UI;

typedef struct tc_Config {
  tc_uint8 title[256];
  tc_uint8 path[128];
  int width;
  int height;
  TIC_INIT_FLAGS flags;
  TIC_WINDOW_FLAGS windowFlags;
  TIC_MODE mode;
  TIC_GAME_LANG lang;
  int argc;
  char ** argv;
} tc_Config;

typedef struct tc_Tileset {
  tc_Image *image;
  tc_Vec2 tilesize;
  int tilecount;
  int columns;
  int *bitmasks;
  tc_Rectf *rect;
} tc_Tileset;

typedef struct tc_Tilemap {
  tc_Tileset *tileset;
  tc_Vec2 grid;
  int width;
  int height;
  int count;
  int *data;
} tc_Tilemap;

typedef struct tc_Camera {
  tc_Rectf area;
  tc_Vec2 size;
  tc_Vec2 offset;
  float angle;
} tc_Camera;

// typedef map_t()

typedef struct tc_Animation {
  int length;
  char name[64];
  int *frames;
} tc_Animation;

typedef map_t(tc_Animation) map_anim_t;

typedef struct tc_Sprite {
  char animation[64];
  int frame;
  tc_Image *image;
  int width;
  int height;
  int count;
  map_anim_t animations;
} tc_Sprite;

#define MAX_RESOURCES_DEPS 32

typedef struct tc_Resource tc_Resource;
typedef struct tc_ResourceDep tc_ResourceDep;
typedef struct tc_ResourcePlugin tc_ResourcePlugin;

struct tc_ResourceDep {
  char name[256];
  tc_Resource *res;
  // tc_ResourceDep *next;
};

typedef list_t(tc_ResourceDep) list_resdep_t;

#define RES_UUID_SIZE 8

struct tc_Resource {
  int deps_count;
  int id;
  // int type;
  int changed;
  int lua;
  char type[64];
  char uuid[RES_UUID_SIZE+1];
  char name[256];
  char path[256];
  // tc_Image *image;
  // tc_Tileset *tileset;
  // tc_Tilemap *tilemap;
  // tc_Sound *sound;
  void *data;
  lua_State *L;
#ifdef WREN_LANG
  WrenVM *vm;
#endif
  list_resdep_t deps;
};

// typedef struct tc_Resource {
//   hashmap map;
//   void **pool;
//   int maxSize;
//   TIC_RESOURCE_TYPE type;
// } tc_Resource;

// typedef map_t(tc_Resource) map_resource_t;
// typedef map_t(map_resource_t) map_res_manager_t;
typedef struct tc_ResourceManager tc_ResourceManager;

typedef tc_Resource(*ResourceLoader)(tc_ResourceManager*, tc_Resource*, cJSON*);
typedef cJSON*(*ResourceStore)(tc_ResourceManager*, tc_Resource*);

#include "plugins/resource.h"

typedef map_t(tc_Resource) map_resource_t;
typedef map_t(ResourceLoader) map_resloader_t;
typedef map_t(ResourceStore) map_resstore_t;
typedef map_t(tc_ResourcePlugin) map_resplugin_t;


struct tc_ResourceManager {
  tc_Resource textures;
  tc_Resource fonts;
  tc_Resource sounds;
//   map_res_manager_t resources;
  char path[256];
  map_resource_t resources;
  map_resloader_t resource_loaders;
  map_resstore_t resource_stores;
  map_resplugin_t plugins;
};




#define TOOL_UUID_SIZE 8

typedef struct tc_EditorPlugin tc_EditorPlugin;
typedef struct tc_Tool tc_Tool;

#include "plugins/editor.h"

struct tc_Tool {
  tc_Canvas canvas;
  tc_Resource *res;
  char uuid[TOOL_UUID_SIZE+1];
  int open;
  tc_Rect win_rect;
  float scale;
  int fullscreen;
  int flags;
  char name[64];
  char filename[512];
  void *cnt;
  void *editor;
  // void (*init)(void*, tc_Tool *tool);
  // void (*update)(void*);
  // void (*draw)(void*);
  // void (*draw_panel)(void*);
  EditorPluginCreate create;
  EditorPluginInit init;
  EditorPluginUpdate update;
  EditorPluginDraw draw;
  EditorPluginDrawPanel draw_panel;

  // void (*begin_update)(void*);
  // void (*end_update)(void*);
  // void (*begin_draw)(void*);
  // void (*end_draw)(void*);
  

  // tc_Resource *res;
};

// struct tc_EditorPlugin {
//   void *editor;
//   int size;
//   int (*create)(void*);
//   void (*init)(void*, tc_Tool *tool);
//   void (*update)(void*);
//   void (*draw)(void*);
//   void (*draw_panel)(void*);
// };

// typedef struct tc_TilemapEditor {
//   tc_Tool *tool;
//   tc_Canvas canvas;
//   tc_Canvas bg;
//   tc_Tilemap *map;
//   void (*draw_panel)(void*);
//   char *name;
//   char *filename;
//   char *tileset_name;
//   int cell;
//   float scale;
//   tc_Vec2 size;
//   void *cnt;
// } tc_TilemapEditor;

typedef struct tc_SpriteTool {
  tc_Tool *tool;
  int cell;
  float scale;
  tc_Image *image;
  tc_Vec2 size;
} tc_SpriteTool;

// typedef map_t(mu_Container*) map_container_t;

#define MAX_TOOLS 32

typedef list_t(tc_Tool*) list_tool_t;
typedef map_t(tc_Tool*) map_tool_t;
typedef map_t(tc_EditorPlugin) map_plugin_t;

typedef struct tc_Editor {
  // char active_tab[64];
  tc_Tool *active_tool;
  int fullscreen_window;
  tc_TilesetEditor tileset_editor;
  tc_TilemapEditor tilemap_editor;
  tc_Canvas bg;
  tc_Canvas game_canvas;
  tc_Resource res_create;
  int sidebar_width;
  void *game_cnt;
  map_plugin_t plugins;
  // tc_Tool tools[MAX_TOOLS];
  tc_Tool *game_tool;
  map_tool_t tool;
} tc_Editor;

typedef struct tc_Core {
  tc_Window window;
  tc_Render render;
  tc_Filesystem fs;
  tc_Font defaultFont;
  tc_Input input;
  tc_Config config;
  tc_Timer timer;
  tc_Lua lua;
  tc_UI ui;
  tc_Editor editor;
#ifdef WREN_LANG
  tc_Wren wren;
#endif
  tc_ResourceManager resources;
  struct {
    tc_bool packed;
    TIC_MODE mode;
    TIC_RENDER_MODE renderMode;
    int (*load)();
    int (*step)();
  } state;
} tc_Core;

extern tc_Core Core;

#if defined(__cplusplus)
extern "C" {
#endif

/*********************
 * Core
 *********************/

TIC_API tc_bool tic_init(tc_Config *config);
TIC_API tc_Config tic_config_init(const char *title, int width, int height, int argc, char ** argv);
TIC_API tc_bool tic_config_json_init(tc_Config *config);

TIC_API void tic_update();
TIC_API void tic_poll_events();

TIC_API void tic_error(char *message);

TIC_API void tic_begin_draw();
TIC_API void tic_end_draw();
TIC_API void tic_swap_buffers();

TIC_API void tic_main_loop();

TIC_API void tic_terminate();

/*********************
 * Timer
 *********************/

TIC_API void tic_timer_update();
TIC_API float tic_timer_get_time();
TIC_API float tic_timer_get_delta();
TIC_API int tic_timer_get_fps();
TIC_API void tic_timer_wait(float ms);

/*======================
 * Audio Module
 *======================*/

TIC_API int tic_audio_init();
TIC_API int tic_audio_start_device();
TIC_API int tic_audio_stop_device();
TIC_API void tic_audio_terminate(void);

TIC_API void tic_audio_set_master_volume(float volume);

TIC_API int tic_audio_get_id(tc_AudioBuffer *buffer);
TIC_API tc_AudioBuffer *tic_audio_get_from_id(unsigned int id);

/***************
 * AudioBuffer
 ***************/
TIC_API tc_AudioBuffer *tic_buffer_load(const char *filename, TIC_AUDIO_USAGE usage);
TIC_API void tic_buffer_play(tc_AudioBuffer *audioBuffer);
TIC_API void tic_buffer_stop(tc_AudioBuffer *audioBuffer);
TIC_API void tic_buffer_pause(tc_AudioBuffer *audioBuffer);
TIC_API void tic_buffer_unload(tc_AudioBuffer *audioBuffer);

TIC_API int tic_buffer_is_playing(tc_AudioBuffer *audioBuffer);
TIC_API int tic_buffer_is_paused(tc_AudioBuffer *audioBuffer);

TIC_API void tic_buffer_set_volume(tc_AudioBuffer *audioBuffer, float volume);

/************
 * Sound
 ************/
TIC_API tc_Sound tic_sound_load(const char *filename, TIC_AUDIO_USAGE usage);
TIC_API void tic_sound_unload(tc_Sound sound);

TIC_API void tic_sound_play(tc_Sound sound);
TIC_API void tic_sound_stop(tc_Sound sound);
TIC_API void tic_sound_pause(tc_Sound sound);

TIC_API int tic_sound_is_playing(tc_Sound sound);
TIC_API int tic_sound_is_paused(tc_Sound sound);
TIC_API void tic_sound_set_volume(tc_Sound sound, float volume);

/********************
 * Audio Data
 ********************/

TIC_API void tic_audio_data_free(tc_AudioData *data);

/*======================
 * Math Module
 *======================*/


#define vec_init(n) \
TIC_API void tic_vec##n##_add(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_sub(tc_Vec##n *v, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_mul(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TIC_API void tic_vec##n##_div(tc_Vec##n *v, const tc_Vec##n a, const float number); \
TIC_API float tic_vec##n##_dot(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_print(const tc_Vec##n v); \
TIC_API int tic_vec##n##_equals(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API float tic_vec##n##_len(const tc_Vec##n v); \
TIC_API void tic_vec##n##_normalize(tc_Vec##n *v, const tc_Vec##n u); \
TIC_API float tic_vec##n##_distance(const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_min(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_max(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b); \
TIC_API void tic_vec##n##_lerp(tc_Vec##n *r, const tc_Vec##n a, const tc_Vec##n b, const float t);

vec_init(2);
TIC_API float tic_vec2_perpdot(const tc_Vec2 v, const tc_Vec2 w);
TIC_API void tic_vec2_normal(tc_Vec2 *r, const tc_Vec2 v);
TIC_API void tic_vec2_projection(tc_Vec2 *r, const tc_Vec2 v, const tc_Vec2 w);
TIC_API float tic_vec2_angle(const tc_Vec2 v);
vec_init(3);
TIC_API void tic_vec3_cross(tc_Vec3 *r, const tc_Vec3 a, const tc_Vec3 b);
TIC_API void tic_vec3_reflect(tc_Vec3 *r, tc_Vec3 const v, tc_Vec3 const n);
vec_init(4);
TIC_API void tic_vec4_cross(tc_Vec4 *r, const tc_Vec4 a, const tc_Vec4 b);
TIC_API void tic_vec4_reflect(tc_Vec4 *r, const tc_Vec4 v, const tc_Vec4 n);

// Matrix defitions
TIC_API void tic_matrix_identity(tc_Matrix *m);
TIC_API void tic_matrix_clone(tc_Matrix *m, tc_Matrix s);
TIC_API int tic_matrix_equals(tc_Matrix a, tc_Matrix b);
TIC_API void tic_matrix_print(tc_Matrix m);
TIC_API void tic_matrix_row(tc_Vec4 *v, tc_Matrix m, int row);
TIC_API void tic_matrix_transpose(tc_Matrix *m, tc_Matrix n);
TIC_API void tic_matrix_translate(tc_Matrix *r, float x, float y, float z);
TIC_API void tic_matrix_translate_in_place(tc_Matrix *r, float x, float y, float z);
TIC_API void tic_matrix_translatev(tc_Matrix *r, tc_Vec3 v);
TIC_API void tic_matrix_scale(tc_Matrix *r, tc_Matrix m, float s);
TIC_API void tic_matrix_scale_aniso(tc_Matrix *m, tc_Matrix a, float x, float y, float z);
TIC_API void tic_matrix_scaleV(tc_Matrix *r, tc_Vec3 v);
TIC_API void tic_matrix_from_vec3_mul_outer(tc_Matrix *m, tc_Vec3 a, tc_Vec3 b);
TIC_API void tic_matrix_mul_vec4(tc_Vec4 *v, tc_Matrix m, tc_Vec4 n);
TIC_API void tic_matrix_mul(tc_Matrix *r, tc_Matrix m, tc_Matrix n);
TIC_API void tic_matrix_rotate(tc_Matrix *r, tc_Matrix m, float x, float y, float z, float angle);
TIC_API void tic_matrix_rotate_x(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tic_matrix_rotate_y(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tic_matrix_rotate_z(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tic_matrix_rotate2D(tc_Matrix *r, tc_Matrix m, float angle);
TIC_API void tic_matrix_invert(tc_Matrix *r, tc_Matrix m);
TIC_API void tic_matrix_orthonormalize(tc_Matrix *r, tc_Matrix m);
TIC_API void tic_matrix_frustum(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TIC_API void tic_matrix_ortho(tc_Matrix *m, float l, float r, float b, float t, float n, float f);
TIC_API void tic_matrix_perspective(tc_Matrix *m, float y_fov, float aspect, float n, float f);
TIC_API void tic_matrix_look_at(tc_Matrix *m, tc_Vec3 eye, tc_Vec3 center, tc_Vec3 up);

TIC_API float tic_lerp(float a, float b, float t);
TIC_API float tic_round(float n);
TIC_API float tic_distance(float a, float b);
TIC_API int tic_sign(float a);
TIC_API float tic_clamp(float v, float lo, float hi);
TIC_API float tic_angle(float x0, float y0, float x1, float y1);

/*==============================
 * Graphics Module
 *==============================*/

TIC_API void tic_graphics_push();
TIC_API void tic_graphics_pop();
TIC_API void tic_graphics_origin();
TIC_API void tic_graphics_translate(float x, float y);
TIC_API void tic_graphics_scale(float x, float y);
TIC_API void tic_graphics_rotate(float angle);
TIC_API void tic_graphics_scissor(int x, int y, int width, int height);
TIC_API void tic_graphics_clear(tc_Color color);


/*******************
 * Shapes
 *******************/

TIC_API void tic_graphics_draw_rectangle(float x, float y, int width, int height, tc_Color color);
TIC_API void tic_graphics_fill_rectangle(float x, float y, float width, float height, tc_Color color);
TIC_API void tic_graphics_draw_rect(tc_Rect rect, tc_Color color);
TIC_API void tic_graphics_fill_rect(tc_Rect rect, tc_Color color);

TIC_API void tic_graphics_draw_circle(int x, int y, float radius, tc_Color color);
TIC_API void tic_graphics_fill_circle(int x, int y, float radius, tc_Color color);
TIC_API void tic_graphics_draw_circle_ex(int x, int y, float radius, int sides, tc_Color color);
TIC_API void tic_graphics_fill_circle_ex(int x, int y, float radius, int sides, tc_Color color);

TIC_API void tic_graphics_draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TIC_API void tic_graphics_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);

TIC_API void tic_graphics_draw_line(int x0, int y0, int x1, int y1, tc_Color color);

/********************
 * Text
 ********************/

TIC_API void tic_graphics_draw_text(const char *text, int x, int y, tc_Color color);
TIC_API void tic_graphics_draw_text_scale(const char *text, int x, int y, float sx, float sy, tc_Color color);
TIC_API void tic_graphics_draw_text_ex(const char *text, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TIC_API void tic_graphics_draw_text_font(tc_Font font, const char *text, int x, int y, tc_Color color);
TIC_API void tic_graphics_draw_text_font_scale(tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color);
TIC_API void tic_graphics_draw_text_font_ex(tc_Font font, const char *text, int x, int y, float sx, float sy, tc_Color color);

TIC_API void tic_graphics_draw_text_font_scale_width(tc_Font font, const char *text, int x, int y, int width, float sx, float sy, tc_Color color);

/*******************
 * Texture
 *******************/

TIC_API tc_Texture tic_texture_create(void *data, int width, int height, int mode);
TIC_API tc_Texture tic_texture_create_named(const char *name, void *data, int width, int height, int mode);
TIC_API tc_Texture tic_texture_load(const char *filename);
TIC_API tc_Texture tic_texture_from_memory(void *data, int bufSize);
TIC_API void tic_texture_destroy(tc_Texture *texture);


TIC_API int tic_texture_get_width(tc_Texture texture);
TIC_API int tic_texture_get_height(tc_Texture texture);
TIC_API void tic_texture_get_size(tc_Texture texture, int *width, int *height);

TIC_API void tic_texture_set_filter(tc_Texture *texture, int min, int mag);
TIC_API void tic_texture_set_wrap(tc_Texture *texture, int min, int mag);
TIC_API void tic_texture_get_filter(tc_Texture texture, int *min, int *mag);
TIC_API void tic_texture_get_wrap(tc_Texture texture, int *min, int *mag);

TIC_API void tic_texture_draw(tc_Texture texture, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tic_texture_draw_ex(tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, float sx, float sy, float cx, float cy, tc_Color color);


/*******************
 * Image
 *******************/

TIC_API tc_Image tic_image_create(void *data, int width, int height, int mode);
TIC_API tc_Image tic_image_load(const char *image);
TIC_API tc_Image tic_image_from_memory(void *data, int bufSize);
TIC_API void tic_image_destroy(tc_Image *image);

TIC_API int tic_image_get_width(tc_Image image);
TIC_API int tic_image_get_height(tc_Image image);
TIC_API void tic_image_get_size(tc_Image image, int *width, int *height);

TIC_API void tic_image_set_filter(tc_Image *image, int min, int mag);
TIC_API void tic_image_get_filter(tc_Image image, int *min, int *mag);
TIC_API void tic_image_set_wrap(tc_Image *image, int min, int mag);
TIC_API void tic_image_get_wrap(tc_Image image, int *min, int *mag);

TIC_API void tic_image_draw(tc_Image image, float x, float y, tc_Color color);
TIC_API void tic_image_draw_scale(tc_Image image, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tic_image_draw_ex(tc_Image image, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tic_image_draw_part(tc_Image image, tc_Rectf part, float x, float y, tc_Color color);
TIC_API void tic_image_draw_part_scale(tc_Image image, tc_Rectf part, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tic_image_draw_part_ex(tc_Image image, tc_Rectf part, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

/*******************
 * Canvas
 *******************/

TIC_API tc_Canvas tic_canvas_create(int width, int height);
TIC_API void tic_canvas_destroy(tc_Canvas *canvas);

TIC_API int tic_canvas_get_width(tc_Canvas canvas);
TIC_API int tic_canvas_get_height(tc_Canvas canvas);
TIC_API void tic_canvas_get_size(tc_Canvas canvas, int *width, int *height);

TIC_API void tic_canvas_attach(tc_Canvas canvas);
TIC_API void tic_canvas_detach(void);
TIC_API void tic_canvas_disable(void);

TIC_API void tic_canvas_draw(tc_Canvas canvas, float x, float y, tc_Color color);
TIC_API void tic_canvas_draw_scale(tc_Canvas canvas, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tic_canvas_draw_ex(tc_Canvas canvas, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tic_canvas_draw_part(tc_Canvas canvas, tc_Rectf rect, float x, float y, tc_Color color);
TIC_API void tic_canvas_draw_part_scale(tc_Canvas canvas, tc_Rectf rect, float x, float y, float sx, float sy, tc_Color color);
TIC_API void tic_canvas_draw_part_ex(tc_Canvas canvas, tc_Rectf rect, float x, float y, float angle, float sx, float sy, float cx, float cy, tc_Color color);

TIC_API void tic_canvas_draw_auto(tc_Canvas canvas);

/*******************
 * Shader
 *******************/

TIC_API void tic_shader_init_shaders();

TIC_API tc_Shader tic_shader_create(int vertShader, int fragShader);
TIC_API tc_Shader tic_shader_create_from_string(const char *vertSource, const char *fragSource);
TIC_API tc_Shader tic_shader_create_from_ustring(const char *source);
TIC_API tc_Shader tic_shader_create_from_file(const char *filename);
TIC_API tc_Shader tic_shader_load_default(int *vertexShader, int *fragShader);
TIC_API tc_Shader tic_shader_create_effect(const char * vertEffect, const char * fragEffect);
TIC_API void tic_shader_destroy(tc_Shader *shader);

TIC_API void tic_shader_new_gba(tc_Shader *shader);
TIC_API void tic_shader_new_outline(tc_Shader *shader);

TIC_API int tic_shader_compile(const char *source, int shaderType);
TIC_API int tic_shader_load_program(int vertexShader, int fragShader);

TIC_API void tic_shader_send_world(tc_Shader shader);
TIC_API void tic_shader_send(tc_Shader shader, const char *name, void *data, TIC_SHADER_UNIFORM_ type);
TIC_API void tic_shader_send_count(tc_Shader shader, const char *name, int count, void *data, TIC_SHADER_UNIFORM_ type);

TIC_API void tic_shader_attach(tc_Shader shader);
TIC_API void tic_shader_detach(void);

/*======================
 * Font
 *======================*/

TIC_API tc_Font tic_font_load_default(void);
TIC_API void tic_font_init(tc_Font *font, const void *data, size_t bufSize, int fontSize);
TIC_API tc_Font tic_font_load(const tc_uint8 *filename, int size);
TIC_API tc_Font tic_font_load_from_memory(const void *data, size_t bufSize, int fontSize);
TIC_API tc_Font tic_font_load_from_texture(tc_Texture texture, int fontWidth, int fontHeight);
TIC_API void tic_font_destroy(tc_Font *font);

TIC_API void tic_font_get_rect(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, int width);
TIC_API void tic_font_get_rect_scale(tc_Font font, const int c, float *x, float *y, tc_Vec2 *outPos, tc_Rectf *rect, float sx, float sy, int width);

TIC_API int tic_font_get_text_width(tc_Font font, const tc_uint8 *text, int len);
TIC_API int tic_font_get_text_height(tc_Font font, const tc_uint8 *text, int len);

/*===========================
 * Render Module
 *===========================*/

TIC_API tc_bool tic_render_init(tc_Render *render);
TIC_API void tic_render_destroy(tc_Render *render);

TIC_API void tic_render_push_canvas(tc_Canvas canvas);
TIC_API void tic_render_pop_canvas(void);
TIC_API void tic_render_push_shader(tc_Shader shader);
TIC_API void tic_render_pop_shader(void);

TIC_API tc_DrawCall tic_drawcall_create(int start, int texture, TIC_DRAW_MODE mode, tc_Rect *clip, tc_Matrix modelview);
TIC_API void tic_drawcall_destroy(tc_DrawCall *call);

/*******************
 * Batch
 *******************/

TIC_API tc_Batch tic_batch_create(tc_Texture texture, int maxQuads);
TIC_API void tic_batch_destroy(tc_Batch *batch);

TIC_API tc_Matrix* tic_batch_get_transform(tc_Batch *batch);

TIC_API tc_DrawCall* tic_batch_next_drawcall(tc_Batch *batch, tc_bool inv);

TIC_API void tic_batch_set_texture(tc_Batch *batch, tc_Texture texture);
TIC_API void tic_batch_set_clip(tc_Batch *batch, tc_Rect rect);
TIC_API void tic_batch_set_draw_mode(tc_Batch *batch, TIC_DRAW_MODE mode);
TIC_API void tic_batch_set_transform(tc_Batch *batch, tc_Matrix matrix);

TIC_API void tic_batch_align(tc_Batch *batch);
TIC_API void tic_batch_update_count(tc_Batch *batch, int indexCount, int vertexCount);

TIC_API void tic_batch_add_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tic_batch_add_line_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf src, tc_Color color);
TIC_API void tic_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, float angle, float cx, float cy, tc_Color color);

TIC_API void tic_batch_add_triangle(tc_Batch *batch, int x0, int y0, int x1, int y1, int x2, int y2, tc_Color color);
TIC_API void tic_batch_add_circle(tc_Batch *batch, int x, int y, float radius, int sides, tc_Color color);
TIC_API void tic_batch_add_line(tc_Batch *batch, int x0, int y0, int x1, int y1, tc_Color color);

TIC_API void tic_batch_begin(tc_Batch *batch);
TIC_API void tic_batch_flush(tc_Batch *batch);
TIC_API void tic_batch_draw(tc_Batch *batch);
TIC_API void tic_batch_clear(tc_Batch *batch);

TIC_API tc_bool tic_batch_is_empty(tc_Batch batch);
TIC_API tc_bool tic_batch_is_full(tc_Batch batch, int vertices);
TIC_API void tic_batch_draw_reset(tc_Batch *batch);
TIC_API void tic_batch_reset_if_full(tc_Batch *batch, int vertex);

/*======================
 * Input Module
 *======================*/

TIC_API tc_bool tic_input_init(tc_Input *input, TIC_INPUT_FLAGS flags);
TIC_API void tic_input_destroy(tc_Input *input);

TIC_API void tic_input_update(tc_Input *input);
TIC_API int tic_input_get_key_code(const char *name);
TIC_API int tic_input_get_joy_btncode(const char *name);
TIC_API int tic_input_get_joy_axiscode(const char *name);
TIC_API int tic_input_get_mouse_code(const char *name);

TIC_API tc_bool tic_input_key_down(TIC_KEY key);
TIC_API tc_bool tic_input_key_pressed(TIC_KEY key);
TIC_API tc_bool tic_input_key_up(TIC_KEY key);
TIC_API tc_bool tic_input_key_released(TIC_KEY key);

TIC_API tc_bool tic_input_mouse_down(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_mouse_pressed(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_mouse_up(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_mouse_released(TIC_MOUSEBUTTON button);
TIC_API void tic_input_mouse_set_cursor(int cursor);

TIC_API tc_bool tic_input_joy_down(TIC_JOYSTICKS jid, TIC_JOYSTICK_BUTTON button);
TIC_API tc_bool tic_input_joy_up(TIC_JOYSTICKS jid, TIC_JOYSTICK_BUTTON button);
TIC_API tc_bool tic_input_joy_pressed(TIC_JOYSTICKS jid, TIC_JOYSTICK_BUTTON button);
TIC_API tc_bool tic_input_joy_released(TIC_JOYSTICKS jid, TIC_JOYSTICK_BUTTON button);

TIC_API void tic_input_get_mouse_pos(int *x, int *y);
TIC_API void tic_input_fix_mouse();
TIC_API void tic_input_release_mouse();
TIC_API void tic_input_get_mouse_delta(int *x, int *y);
TIC_API void tic_input_get_mouse_scroll(float *x, float *y);
TIC_API void tic_input_get_mouse_scroll_delta(float *x, float *y);

/*======================
 * Filesystem
 *======================*/

TIC_API int tic_filesystem_init();
TIC_API void tic_filesystem_set_packed(tc_bool packed);
TIC_API tc_bool tic_filesystem_is_packed();

TIC_API void tic_filesystem_set_path(const char *path);
TIC_API tc_uint8* tic_filesystem_get_path();
TIC_API void tic_filesystem_resolve_path(char *outName, const tc_uint8 *filename);
TIC_API tc_uint8* tic_filesystem_get_exe_path();

TIC_API tc_uint8 *tic_filesystem_read_internal_file(const tc_uint8 *filename, size_t *outSize);
TIC_API tc_uint8 *tic_filesystem_read_external_file(const tc_uint8 *filename, size_t *outSize);

TIC_API void tic_filesystem_write_internal_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TIC_WRITE_MODE mode);
TIC_API void tic_filesystem_write_external_file(const tc_uint8 *filename, const tc_uint8* text, size_t size, TIC_WRITE_MODE mode);

TIC_API tc_bool tic_filesystem_internal_file_exists(const tc_uint8 *filename);
TIC_API tc_bool tic_filesystem_external_file_exists(const tc_uint8 *filename);

TIC_API tc_uint8 *tic_filesystem_read_file(const tc_uint8 *filename, size_t *outSize);
TIC_API void tic_filesystem_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TIC_WRITE_MODE mode);
TIC_API void tic_filesystem_delete_file(const tc_uint8 *filename);
TIC_API tc_bool tic_filesystem_file_exists(const tc_uint8 *filename);
TIC_API void tic_filesystem_mkdir(const tc_uint8 *path);
TIC_API void tic_filesystem_rmdir(const tc_uint8 *path);

/*********************
 * JSON
 *********************/

TIC_API cJSON* tic_json_open(const char *filename);
TIC_API cJSON* tic_json_parse(const char *jsonStr);
TIC_API void tic_json_save(const char *filename, cJSON* const json);
TIC_API char* tic_json_print(cJSON* const json);

TIC_API cJSON* tic_json_create();
TIC_API cJSON* tic_json_create_array();
TIC_API void tic_json_delete(cJSON* const json);

TIC_API tc_bool tic_json_is_string(cJSON* const json, char *name);
TIC_API char* tic_json_get_string(cJSON* const json, char *name);
TIC_API char* tic_json_get_opt_string(cJSON* const json, char *name, char *optVal);
TIC_API cJSON* tic_json_set_string(cJSON* const json, char *name, char *value);

TIC_API tc_bool tic_json_is_number(cJSON* const json, char *name);
TIC_API float tic_json_get_number(cJSON* const json, char *name);
TIC_API float tic_json_get_opt_number(cJSON* const json, char *name, float optVal);
TIC_API cJSON* tic_json_set_number(cJSON* const json, char *name, float value);

TIC_API tc_bool tic_json_is_boolean(cJSON* const json, char *name);
TIC_API int tic_json_get_boolean(cJSON* const json, char *name);
TIC_API int tic_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal);
TIC_API cJSON* tic_json_set_boolean(cJSON* const json, char *name, tc_bool value);

TIC_API tc_bool tic_json_is_array(cJSON* const json, char *name);
TIC_API cJSON* tic_json_get_array(cJSON* const json, char *name);
TIC_API cJSON* tic_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray);
TIC_API int tic_json_get_array_size(const cJSON* json);

TIC_API tc_bool tic_json_is_object(cJSON* const json, char *name);
TIC_API cJSON* tic_json_get_object(cJSON* const json, char *name);
TIC_API cJSON* tic_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj);


/*======================
 * Window
 *======================*/

TIC_API tc_bool tic_window_init(tc_Window *window, const char *title, int width, int height, TIC_WINDOW_FLAGS flags);
TIC_API void tic_window_destroy(tc_Window *window);

TIC_API int tic_window_should_close();
TIC_API void tic_window_close();

TIC_API void tic_window_get_pos(int *x, int *y);
TIC_API void tic_window_set_pos(int x, int y);

TIC_API int tic_window_get_width();
TIC_API int tic_window_get_height();
TIC_API void tic_window_set_width(int width);
TIC_API void tic_window_set_height(int height);
TIC_API void tic_window_get_size(int *width, int *height);
TIC_API void tic_window_set_size(int width, int height);

TIC_API tc_uint8* tic_window_get_title();
TIC_API void tic_window_set_title(const tc_uint8 *title);

TIC_API tc_bool tic_window_is_fullscreen();
TIC_API void tic_window_set_fullscreen(tc_bool fullscreen);
TIC_API void tic_window_toggle_fullscreen();

TIC_API tc_bool tic_window_is_resizable();
TIC_API void tic_window_set_resizable(tc_bool fullscreen);
TIC_API void tic_window_toggle_resizable();

TIC_API tc_bool tic_window_get_vsync();
TIC_API void tic_window_set_vsync(tc_bool vsync);

/*======================
 * Resource Manager
 *======================*/

TIC_API void tic_resource_init(tc_Resource *resource, TIC_RESOURCE_TYPE type, int poolSize);
TIC_API void tic_resource_destroy(tc_Resource *resource);
TIC_API tc_bool tic_resource_add(tc_Resource *resource, const char *name, void *data);
TIC_API void* tic_resource_get(tc_Resource *resource, const char *name);
TIC_API tc_bool tic_resource_is_full(tc_Resource *resource);

TIC_API tc_bool tic_resources_init(tc_ResourceManager *manager);
TIC_API int tic_resources_default_loaders(tc_ResourceManager *manager);
TIC_API tc_bool tic_resources_load(tc_ResourceManager *manager, const char *path);
TIC_API void tic_resources_store(tc_ResourceManager *manager);
TIC_API tc_bool tic_resources_destroy(tc_ResourceManager *manager);

TIC_API tc_bool tic_resources_add_texture(const char *name, tc_Texture *texture);
TIC_API tc_Texture* tic_resources_get_texture(const char *name);

// TIC_API tc_bool tic_resources_add_sound(const char *name, tc_AudioData* sound);
// TIC_API tc_AudioData* tic_resources_get_sound(const char *name);

// TIC_API tc_bool tic_resources_add_font(const char *name, tc_Font* sound);
// TIC_API tc_Font* tic_resources_get_font(const char *name);

TIC_API tc_ResourceDep tic_resources_new_dep(const char *name, tc_Resource *res);
TIC_API void tic_resources_push_dep(tc_Resource *res, const char *name, const char *resource_name);
TIC_API void tic_resources_change_dep(tc_ResourceManager *manager, const char *name, const char *res_name);
TIC_API void tic_resources_new_type(tc_ResourceManager *manager, const char *type);

TIC_API void tic_resources_update_resource(tc_Resource *resource);

TIC_API tc_bool tic_resources_add(tc_ResourceManager *manager, const char *type, const char *name, tc_Resource *resource);
TIC_API tc_Resource* tic_resources_get(tc_ResourceManager *manager, const char *type, const char *name);

TIC_API tc_Resource tic_resources_image_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json);
TIC_API tc_Resource tic_resources_tileset_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json);
TIC_API tc_Resource tic_resources_tilemap_loader(tc_ResourceManager *manager, tc_Resource *res, cJSON *json);

TIC_API cJSON* tic_resources_tileset_store(tc_ResourceManager *manager, tc_Resource *res);
TIC_API cJSON* tic_resources_tilemap_store(tc_ResourceManager *manager, tc_Resource *res);

TIC_API tc_bool tic_resources_add_image(tc_ResourceManager *manager, const char *name, tc_Image *image);
TIC_API tc_Image* tic_resources_get_image(tc_ResourceManager *manager, const char *name);
TIC_API void tic_resources_remove_image(tc_ResourceManager *manager, const char *name);

/*======================
 * UI
 *======================*/

#define tic_ui_get_container(label) mu_get_container(Core.ui.ctx, label)
#define tic_ui_get_current_container() mu_get_current_container(Core.ui.ctx)

#define tic_ui_layout_row(n, width, height) mu_layout_row(Core.ui.ctx, n, (width), height)
#define tic_ui_layout_width(width) mu_layout_width(Core.ui.ctx, width)
#define tic_ui_layout_height(height) mu_layout_height(Core.ui.ctx, height)
#define tic_ui_begin_column() mu_layout_begin_column(Core.ui.ctx)
#define tic_ui_end_column() mu_layout_end_column(Core.ui.ctx)
#define tic_ui_layout_next() mu_layout_next(Core.ui.ctx)
#define tic_ui_get_layout() mu_get_layout(Core.ui.ctx)

#define tic_ui_grid(label, size, cell)                 mu_grid(Core.ui.ctx, label, size, cell)
#define tic_ui_grid_ex(label, size, cell, opt)         mu_grid_ex(Core.ui.ctx, label, size, cell, opt)
#define tic_ui_image_grid(label, image, size, cell)    mu_image_grid(Core.ui.ctx, label, image, size, cell)
#define tic_ui_image_grid_ex(label, image, p, size, cell, opt)    mu_image_grid_ex(Core.ui.ctx, label, image, p, size, cell, opt)
#define tic_ui_button(label)                           mu_button(Core.ui.ctx, label)
#define tic_ui_image_button(label, texture)            mu_image_button(Core.ui.ctx, label, texture)
#define tic_ui_slider(value, lo, hi)                   mu_slider(Core.ui.ctx, value, lo, hi)
#define tic_ui_header(label)                           mu_header(Core.ui.ctx, label)
#define tic_ui_header_ex(label, opt)                   mu_header_ex(Core.ui.ctx, label, opt)
#define tic_ui_label(label)                            mu_label(Core.ui.ctx, label)
#define tic_ui_text(text)                              mu_text(Core.ui.ctx, text)
#define tic_ui_image(texId, size, src, color)          mu_image(Core.ui.ctx, texId, size, src, color)
#define tic_ui_number(label, number, step)             mu_number(Core.ui.ctx, label, number, step)
#define tic_ui_checkbox(label, value)                  mu_checkbox(Core.ui.ctx, label, value)
#define tic_ui_checkbox_ex(label, value)               mu_checkbox(Core.ui.ctx, label, value)
#define tic_ui_textbox(buffer, size)                   mu_textbox(Core.ui.ctx, buffer, size)
#define tic_ui_textbox_raw(id, buffer, size, r)        mu_textbox_raw(Core.ui.ctx, buffer, size, id, r, 0)

#define tic_ui_begin_window(title, rect) mu_begin_window(Core.ui.ctx, title, rect)
#define tic_ui_begin_window_ex(title, rect, opt) mu_begin_window(Core.ui.ctx, title, rect)
#define tic_ui_begin_panel(label) mu_begin_panel(Core.ui.ctx, label)
#define tic_ui_begin_panel_ex(label, opt) mu_begin_panel_ex(Core.ui.ctx, label, opt)
#define tic_ui_begin_treenode(label) mu_begin_treenode(Core.ui.ctx, label)
#define tic_ui_begin_treenode_ex(label, opt) mu_begin_treenode_ex(Core.ui.ctx, label, opt)
#define tic_ui_begin_popup(label) mu_begin_popup(Core.ui.ctx, label)
#define tic_ui_open_popup(label) mu_open_popup(Core.ui.ctx, label)

#define tic_ui_end_window() mu_end_window(Core.ui.ctx)
#define tic_ui_end_panel() mu_end_panel(Core.ui.ctx)
#define tic_ui_end_treenode() mu_end_treenode(Core.ui.ctx)
#define tic_ui_end_popup() mu_end_popup(Core.ui.ctx)

#define tic_ui_get_id(name, size) mu_get_id(Core.ui.ctx, name, size);
#define tic_ui_get_focused() Core.ui.ctx->focus
#define tic_ui_get_last() Core.ui.ctx->last_id
#define tic_ui_last_zindex() Core.ui.ctx->last_zindex

TIC_API void tic_ui_init(tc_Font font);
TIC_API void tic_ui_terminate();

TIC_API void tic_ui_begin();
TIC_API void tic_ui_end();

TIC_API void tic_ui_mouse_pos_callback(double posX, double posY);
TIC_API void tic_ui_mouse_btn_callback(int button, int x, int y, int action);
TIC_API void tic_ui_mouse_scroll_callback(double xoff, double yoff);
TIC_API void tic_ui_key_callback(int key, int action);
TIC_API void tic_ui_text_callback(int codepoint);

TIC_API void tic_ui_indent();
TIC_API void tic_ui_unindent();

TIC_API int tic_ui_text_width(mu_Font font, const char *text, int len);
TIC_API int tic_ui_text_height(mu_Font font);

/*======================
 * Engine
 *======================*/

/**********************
 * Tileset
 **********************/

TIC_API tc_Tileset tic_tileset_create(tc_Image *image, int w, int h);
TIC_API tc_Tileset tic_tileset_load(const char *path, tc_Image *image);
TIC_API void tic_tileset_destroy(tc_Tileset *tileset);

TIC_API void tic_tileset_set_bitmask(tc_Tileset *tileset, int index, int bitmask);

TIC_API void tic_tileset_calc_mask_array(tc_Tileset tileset, int bitmask, int *bitmask_array);
TIC_API int tic_tileset_calc_mask(tc_Tileset tileset, int *bitmask_array);

TIC_API void tic_tileset_draw(tc_Tileset tileset, int index, int x, int y, tc_Color color);

TIC_API tc_Rectf tic_tileset_get_rect(tc_Tileset tileset, int index);
  //TIC_API tc_Rectf tic_tileset_get_rect_from_bitmask(tc_Tileset tileset, int bitmask);
TIC_API int tic_tileset_get_from_bitmask(tc_Tileset tileset, int bitmask);

/**********************
 * Tilemap
 **********************/

TIC_API tc_Tilemap tic_tilemap_create(tc_Tileset *tileset, int w, int h);
TIC_API tc_Tilemap tic_tilemap_load(const char *path, tc_Tileset *tileset);
TIC_API void tic_tilemap_resize(tc_Tilemap *tilemap, int w, int h);
TIC_API void tic_tilemap_destroy(tc_Tilemap *map);

TIC_API int tic_tilemap_has_tile(tc_Tilemap map, int x, int y);

TIC_API void tic_tilemap_draw(tc_Tilemap map);
TIC_API void tic_tilemap_draw_part(tc_Tilemap map, tc_Rect part, int x, int y);

TIC_API void tic_tilemap_update(tc_Tilemap *map);
TIC_API int tic_tilemap_autotile(tc_Tilemap tilemap, int index);
TIC_API int tic_tilemap_autotile_pos(tc_Tilemap tilemap, int x, int y);

TIC_API void tic_tilemap_insert(tc_Tilemap *map, int index);
TIC_API void tic_tilemap_remove(tc_Tilemap *map, int index);
TIC_API void tic_tilemap_insert_pos(tc_Tilemap *map, int index);

TIC_API int tic_tilemap_get_index(tc_Tilemap map, int x, int y);
TIC_API void tic_tilemap_get_position(tc_Tilemap map, int index, tc_Vec2 *v);

/***********************
 * Camera
 ***********************/

TIC_API tc_Camera tic_camera_create(float x, float y, float w, float h);
TIC_API void tic_camera_destroy(tc_Camera *camera);

TIC_API void tic_camera_attach(tc_Camera camera);
TIC_API void tic_camera_detach();

TIC_API void tic_camera_set_pos(tc_Camera *camera, tc_Vec2 pos);
TIC_API void tic_camera_set_scale(tc_Camera *camera, tc_Vec2 scale);
TIC_API void tic_camera_set_size(tc_Camera *camera, tc_Vec2 size);

TIC_API void tic_camera_get_pos(tc_Camera camera, tc_Vec2 *pos);
TIC_API void tic_camera_get_scale(tc_Camera camera, tc_Vec2 *scale);
TIC_API void tic_camera_get_size(tc_Camera camera, tc_Vec2 *size);


/*======================
 * Editor
 *======================*/

TIC_API void tic_editor_load(tc_Editor *editor);
TIC_API void tic_editor_destroy(tc_Editor *editor);

TIC_API void tic_editor_default(tc_Editor *editor);
TIC_API void tic_editor_open(tc_Editor *editor, const char *type, const char *name);
TIC_API void tic_editor_add_plugin(tc_Editor *editor, const char *type, tc_EditorPlugin *plugin);

TIC_API void tic_editor_load_state(tc_Editor *editor);
TIC_API void tic_editor_save_state(tc_Editor *editor);

TIC_API tc_Resource* tic_editor_resource_viewer(tc_Editor *editor);

TIC_API void tic_editor_set_tileset(tc_Editor *editor, const char *name, tc_Tileset *tileset);
TIC_API void tic_editor_set_tilemap(tc_Editor *editor, const char *name, tc_Tilemap *tilemap);

TIC_API void tic_editor_new_tileset(tc_Editor *editor, int w, int h);
TIC_API void tic_editor_new_tilemap(tc_Editor *editor, int w, int h);

TIC_API void tic_editor_update(tc_Editor *editor);
TIC_API void tic_editor_draw(tc_Editor *editor);
TIC_API int tic_editor_begin_draw(tc_Editor *editor);
TIC_API void tic_editor_end_draw(tc_Editor *editor);

TIC_API void tic_editor_set_game_canvas(tc_Editor *editor, tc_Canvas *canvas);
TIC_API int tic_editor_is_game_open(tc_Editor *editor);
TIC_API void tic_editor_set_game_open(tc_Editor *editor, int open);

TIC_API void tic_editor_init_tool(tc_Tool *tool, tc_EditorPlugin *plugin);
TIC_API void tic_editor_destroy_tool(tc_Tool *tool);
TIC_API void tic_editor_draw_tool_panel(tc_Tool *tool);

/*******************
 * Game
 *******************/

TIC_API void tic_editor_game_init(tc_GameEditor *editor, tc_Tool *tool);
TIC_API void tic_editor_game_destroy(tc_GameEditor *editor);

TIC_API int tic_editor_game_draw(tc_GameEditor *editor);
TIC_API int tic_editor_game_panel(tc_GameEditor *editor);

/*********************
 * Tileset
 *********************/

TIC_API void tic_editor_tileset_init(tc_TilesetEditor *editor, tc_Tool *tool);
TIC_API void tic_editor_tileset_destroy(tc_TilesetEditor *editor);

TIC_API void tic_editor_tileset_change(tc_TilesetEditor *editor, tc_Tileset *tileset);

TIC_API int tic_editor_tileset_draw(tc_TilesetEditor *editor);
TIC_API void tic_editor_tileset_save(tc_TilesetEditor *editor);

/**********************
 * Tilemap
 **********************/

TIC_API void* tic_editor_tilemap_create(tc_Resource *editor);
TIC_API void tic_editor_tilemap_init(tc_TilemapEditor *editor, tc_Tool *tool);
TIC_API void tic_editor_tilemap_destroy(tc_TilemapEditor *editor);

TIC_API void tic_editor_tilemap_change(tc_TilemapEditor *editor, tc_Tilemap *tilemap);

TIC_API int tic_editor_tilemap_draw(tc_TilemapEditor *editor);
TIC_API void tic_editor_tilemap_save(tc_TilemapEditor *editor);


/*======================
 * Scripting
 *======================*/

/**************
 * Lua
 **************/

TIC_API int luaopen_tico(lua_State *L);

TIC_API tc_bool tic_lua_init();
TIC_API int tic_lua_load();
TIC_API int tic_lua_step();

TIC_API void tic_lua_callback(const char *name);

TIC_API int tic_lua_preload(const char *modName, const char *modCode, int bufSize);

/***************
 * Wren
 ***************/

TIC_API tc_bool tic_wren_init();
TIC_API void tic_wren_terminate();

TIC_API int tic_wren_load();
TIC_API int tic_wren_step();

TIC_API void tic_wren_callback(const char *name);

/**********************
 * Debug
 **********************/

TIC_API void tic_tracelog(int mode, const char *func, const char *file, int line, const char *fmt, ...);
TIC_API void tic_log(int mode, const char *fmt, ...);

/*=======================
 * Utils
 *=======================*/

TIC_API char *tic_replace_char(char *str, tc_uint8 find, tc_uint8 replace);
TIC_API tc_uint8* tic_utf8_codepoint(tc_uint8 *p, int* codepoint);
TIC_API void tic_utf8_encode(tc_uint8* c, int codepoint);
TIC_API int tic_utf8_decode(const tc_uint8 *p);
TIC_API void tic_write_buffer_header(const char *name, const char *text, size_t size);
TIC_API void tic_set_clipboard(const char *text);
TIC_API const char* tic_get_clipboard();

#if defined(__cplusplus)
}
#endif

#endif

#ifndef TICO_H
#define TICO_H


#ifndef TICO_DEF_ONLY
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
#else
  typedef void* GLFWwindow;
  typedef void* cJSON;
  typedef void* hashmap;
  typedef void* ma_decoder;
  typedef void* stbtt_fontinfo;
#endif

#define TICO_VERSION "0.1.4"

#ifndef TIC_API
  #define TIC_API
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

#define DEFAULT_MAX_TEXTURES 32
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
  TIC_UNIFORM_FLOAT,
  TIC_UNIFORM_VEC2,
  TIC_UNIFORM_VEC3,
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
  MOUSE_BUTTON_LAST = MOUSE_MIDDLE,
  MOUSE_BUTTON_COUNT
} TIC_MOUSEBUTTON;

typedef enum {
  JOY_DPAD_LEFT = 0
} TIC_JOYSTICK;

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

typedef struct {
  struct {
    int x;
    int y;
    int fixX;
    int fixY;
    float scrollX;
    float scrollY;
    tc_bool down[MOUSE_BUTTON_LAST];
    tc_bool pressed[MOUSE_BUTTON_LAST];
    tc_bool fixed;
    tc_bool active;
    hashmap buttonNames;
  } mouseState;
  struct {
    tc_bool down[KEY_LAST];
    tc_bool pressed[KEY_LAST];
    tc_bool active;
    hashmap keyNames;
  } keyState;
  struct {
    tc_bool down[4];
    tc_bool pressed[4];
    float axis[2];
    tc_bool active;
  } joystickState[4];
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

typedef struct tc_Render {
  tc_Batch batch;
  struct {
    tc_Shader defaultShader;
    tc_Shader currentShader;

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

    tc_Matrix matrixStack[MATRIX_STACK_SIZE];
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

typedef enum {
  TIC_RESOURCE_TEXTURE = 1,
  TIC_RESOURCE_SOUND,
  TIC_RESOURCE_FONT
} TIC_RESOURCE_TYPE;

typedef struct tc_Resource {
  hashmap map;
  void **pool;
  int maxSize;
  TIC_RESOURCE_TYPE type;
} tc_Resource;

typedef struct tc_ResourceManager {
  tc_Resource textures;
  tc_Resource fonts;
  tc_Resource sounds;
} tc_ResourceManager;


typedef struct tc_Config {
  tc_uint8 title[256];
  tc_uint8 path[128];
  int width;
  int height;
  TIC_INIT_FLAGS flags;
  TIC_WINDOW_FLAGS windowFlags;
  TIC_MODE mode;
  int argc;
  char ** argv;
} tc_Config;

typedef struct tc_Core {
  tc_Window window;
  tc_Render render;
  tc_Filesystem fs;
  tc_Font defaultFont;
  tc_Input input;
  tc_Config config;
  tc_Timer timer;
  tc_Lua lua;
  tc_ResourceManager resources;
  struct {
    tc_bool packed;
    TIC_MODE mode;
    TIC_RENDER_MODE renderMode;
    int (*loadGame)();
    int (*updateGame)();
    int (*drawGame)();
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

TIC_API void tic_begin_draw();
TIC_API void tic_end_draw();
TIC_API void tic_clear(tc_Color color);
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
TIC_API void tic_graphics_translate(int x, int y);
TIC_API void tic_graphics_scale(float x, float y);
TIC_API void tic_graphics_rotate(float angle);
TIC_API void tic_graphics_scissor(int x, int y, int width, int height);

/*******************
 * Shapes
 *******************/

TIC_API void tic_graphics_draw_rectangle(int x, int y, int width, int height, tc_Color color);
TIC_API void tic_graphics_fill_rectangle(int x, int y, int width, int height, tc_Color color);
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
TIC_API void tic_texture_draw_ex(tc_Texture texture, tc_Rectf dest, tc_Rectf source, float angle, float sx, float sy, int cx, int cy, tc_Color color);


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

TIC_API void tic_image_draw(tc_Image image, int x, int y, tc_Color color);
TIC_API void tic_image_draw_scale(tc_Image image, int x, int y, float sx, float sy, tc_Color color);
TIC_API void tic_image_draw_ex(tc_Image image, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TIC_API void tic_image_draw_part(tc_Image image, tc_Rectf part, int x, int y, tc_Color color);
TIC_API void tic_image_draw_part_scale(tc_Image image, tc_Rectf part, int x, int y, float sx, float sy, tc_Color color);
TIC_API void tic_image_draw_part_ex(tc_Image image, tc_Rectf part, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

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

TIC_API void tic_canvas_draw(tc_Canvas canvas, int x, int y, tc_Color color);
TIC_API void tic_canvas_draw_scale(tc_Canvas canvas, int x, int y, float sx, float sy, tc_Color color);
TIC_API void tic_canvas_draw_ex(tc_Canvas canvas, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color);

TIC_API void tic_canvas_draw_part(tc_Canvas canvas, tc_Rectf rect, int x, int y, tc_Color color);
TIC_API void tic_canvas_draw_part_scale(tc_Canvas canvas, tc_Rectf rect, int x, int y, float sx, float sy, tc_Color color);

TIC_API void tic_canvas_draw_auto(tc_Canvas canvas);

/*******************
 * Shader
 *******************/

TIC_API tc_Shader tic_shader_create(int vertShader, int fragShader);
TIC_API tc_Shader tic_shader_create_from_string(const char *vertSource, const char *fragSource);
TIC_API tc_Shader tic_shader_create_from_ustring(const char *source);
TIC_API tc_Shader tic_shader_create_from_file(const char *filename);
TIC_API tc_Shader tic_shader_load_default(int *vertexShader, int *fragShader);
TIC_API void tic_shader_destroy(tc_Shader *shader);

TIC_API int tic_shader_compile(const char *source, int shaderType);
TIC_API int tic_shader_load_program(int vertexShader, int fragShader);

TIC_API void tic_shader_send_world(tc_Shader shader);
TIC_API void tic_shader_send(tc_Shader shader, const char *name, void *data, TIC_SHADER_UNIFORM_ type);

TIC_API void tic_shader_attach(tc_Shader *shader);
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

TIC_API tc_DrawCall tic_drawcall_create(int start, int texture, TIC_DRAW_MODE mode, tc_Rect *clip, tc_Matrix modelview);
TIC_API void tic_drawcall_destroy(tc_DrawCall *call);

/*******************
 * Batch
 *******************/

TIC_API tc_Batch tic_batch_create(tc_Texture texture, int maxQuads);
TIC_API void tic_batch_destroy(tc_Batch *batch);

TIC_API tc_Matrix* tic_batch_get_transform(tc_Batch *batch);

TIC_API void tic_batch_set_texture(tc_Batch *batch, tc_Texture texture);
TIC_API void tic_batch_set_clip(tc_Batch *batch, tc_Rect rect);
TIC_API void tic_batch_set_draw_mode(tc_Batch *batch, TIC_DRAW_MODE mode);
TIC_API void tic_batch_set_transform(tc_Batch *batch, tc_Matrix matrix);

TIC_API void tic_batch_align(tc_Batch *batch);
TIC_API void tic_batch_update_count(tc_Batch *batch, int indexCount, int vertexCount);

TIC_API void tic_batch_add_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, tc_Color color);
TIC_API void tic_batch_add_line_rect(tc_Batch *batch, tc_Rectf dest, tc_Rectf src, tc_Color color);
TIC_API void tic_batch_add_rect_ex(tc_Batch *batch, tc_Rectf dest, tc_Rectf source, float angle, int cx, int cy, tc_Color color);

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
TIC_API int tic_input_get_keycode(const char *name);

TIC_API tc_bool tic_input_is_key_down(TIC_KEY key);
TIC_API tc_bool tic_input_is_key_pressed(TIC_KEY key);
TIC_API tc_bool tic_input_is_key_up(TIC_KEY key);
TIC_API tc_bool tic_input_is_key_released(TIC_KEY key);

TIC_API tc_bool tic_input_is_mouse_down(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_is_mouse_pressed(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_is_mouse_up(TIC_MOUSEBUTTON button);
TIC_API tc_bool tic_input_is_mouse_released(TIC_MOUSEBUTTON button);

TIC_API void tic_input_get_mouse(int *x, int *y);
TIC_API void tic_input_fix_mouse();
TIC_API void tic_input_release_mouse();
TIC_API void tic_input_get_mouse_delta(int *x, int *y);

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
TIC_API void tic_json_delete(cJSON* const json);

TIC_API char* tic_json_get_string(cJSON* const json, char *name);
TIC_API char* tic_json_get_opt_string(cJSON* const json, char *name, char *optVal);
TIC_API cJSON* tic_json_set_string(cJSON* const json, char *name, char *value);

TIC_API float tic_json_get_number(cJSON* const json, char *name);
TIC_API float tic_json_get_opt_number(cJSON* const json, char *name, float optVal);
TIC_API cJSON* tic_json_set_number(cJSON* const json, char *name, float value);

TIC_API int tic_json_get_boolean(cJSON* const json, char *name);
TIC_API int tic_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal);
TIC_API cJSON* tic_json_set_boolean(cJSON* const json, char *name, tc_bool value);

TIC_API cJSON* tic_json_get_array(cJSON* const json, char *name);
TIC_API cJSON* tic_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray);
TIC_API int tic_json_get_array_size(const cJSON* json);

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
TIC_API tc_bool tic_resources_destroy(tc_ResourceManager *manager);

TIC_API tc_bool tic_resources_add_texture(const char *name, tc_Texture *texture);
TIC_API tc_Texture* tic_resources_get_texture(const char *name);

TIC_API tc_bool tic_resources_add_sound(const char *name, tc_AudioData* sound);
TIC_API tc_AudioData* tic_resources_get_sound(const char *name);

TIC_API tc_bool tic_resources_add_font(const char *name, tc_Font* sound);
TIC_API tc_Font* tic_resources_get_font(const char *name);

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

TIC_API int tic_lua_preload(const char *modName, const char *modCode);

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
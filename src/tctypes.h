#ifndef TC_TYPES_H
#define TC_TYPES_H

typedef enum {
  TC_UNIFORM_INT,
  TC_UNIFORM_VEC2I,
  TC_UNIFORM_VEC3I,
  TC_UNIFORM_FLOAT,
  TC_UNIFORM_VEC2,
  TC_UNIFORM_VEC3,
  TC_UNIFORM_MATRIX
} TC_SHADER_UNIFORM_;

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
} tc_vertex;

typedef struct {
  union {
    unsigned char data[4];
    struct {
      unsigned char r;
      unsigned char g;
      unsigned char b;
      unsigned char a;
    };
  };
} tc_color;

typedef struct {
  union {
    float data[4];
    struct {
      float x;
      float y;
      float w;
      float h;
    };
  };
} tc_rect;

typedef struct {
  union {
    int data[4];
    struct {
      int x;
      int y;
      int w;
      int h;
    };
  };
} tc_recti;

typedef struct {
  unsigned int id;
  int width;
  int height;
} tc_texture;

typedef struct {
  tc_texture tex; // texture
  void *data; // image data
} tc_image;

typedef struct {
  int id; // framebuffer id
  tc_texture tex; // texture
  int width; // canvas width
  int height; // canvas height
} tc_canvas; // used separated width and height because framebuffer texture inverts the image

typedef struct {
  unsigned int id;
} tc_shader;

typedef struct {
  double currentTime;
  double lastTime;
  double fpsLastTime;
  float delta;
  int fps;
  int frames;
} tc_timer;

typedef struct {
  tc_texture tex;
} tc_sprite;

#endif // TC_TYPES_H

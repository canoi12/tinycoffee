#ifndef TICO_AUDIO_H
#define TICO_AUDIO_H

#include "definitions.h"

#include "miniaudio.h"

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

typedef enum {
  TIC_AUDIO_STREAM = 0,
  TIC_AUDIO_STATIC
} TIC_AUDIO_USAGE_;

typedef struct tc_AudioData {
  TIC_AUDIO_USAGE_ usage;
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

// typedef struct {
//   struct
//   {
//     ma_context ctx;
//     ma_device device;
//     ma_mutex lock;

//     int isReady;

//   } system;

//   struct
//   {
//     tc_AudioBuffer buffer[MAX_AUDIO_BUFFER_CHANNELS];
//   } multiChannel;
// } tc_Audio;

// TIC_API int tico_audio_init(tc_AudioSystem *system);
// TIC_API int tico_audio_deinit(tc_AudioSystem *system);
/*======================
 * Audio Module
 *======================*/

TIC_API int tico_audio_init();
TIC_API int tico_audio_start_device();
TIC_API int tico_audio_stop_device();
TIC_API void tico_audio_terminate(void);

TIC_API void tico_audio_set_master_volume(float volume);

TIC_API int tico_audio_get_id(tc_AudioBuffer *buffer);
TIC_API tc_AudioBuffer *tico_audio_get_from_id(unsigned int id);

/***************
 * AudioBuffer
 ***************/
TIC_API tc_AudioBuffer *tico_buffer_load(const char *filename, TIC_AUDIO_USAGE_ usage);
TIC_API void tico_buffer_play(tc_AudioBuffer *audioBuffer);
TIC_API void tico_buffer_stop(tc_AudioBuffer *audioBuffer);
TIC_API void tico_buffer_pause(tc_AudioBuffer *audioBuffer);
TIC_API void tico_buffer_unload(tc_AudioBuffer *audioBuffer);

TIC_API int tico_buffer_is_playing(tc_AudioBuffer *audioBuffer);
TIC_API int tico_buffer_is_paused(tc_AudioBuffer *audioBuffer);

TIC_API void tico_buffer_set_volume(tc_AudioBuffer *audioBuffer, float volume);

/************
 * Sound
 ************/
TIC_API tc_Sound tico_sound_load(const char *filename, TIC_AUDIO_USAGE_ usage);
TIC_API void tico_sound_unload(tc_Sound sound);

TIC_API void tico_sound_play(tc_Sound sound);
TIC_API void tico_sound_stop(tc_Sound sound);
TIC_API void tico_sound_pause(tc_Sound sound);

TIC_API int tico_sound_is_playing(tc_Sound sound);
TIC_API int tico_sound_is_paused(tc_Sound sound);
TIC_API void tico_sound_set_volume(tc_Sound sound, float volume);

/********************
 * Audio Data
 ********************/

TIC_API void tico_audio_data_free(tc_AudioData *data);

#endif // TICO_AUDIO_H
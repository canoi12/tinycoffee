#ifndef TC_AUDIO_H
#define TC_AUDIO_H

#include "tinycoffee.h"

#include "external/miniaudio.h"

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

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef enum {
  AUDIO_STREAM = 0,
  AUDIO_STATIC
} TC_AUDIO_USAGE;

typedef struct {
  ma_decoder decoder;
  unsigned char id;

  float volume;
  float pitch;

  int playing;
  int paused;
  int loop;
  int loaded;

  TC_AUDIO_USAGE usage;

  unsigned char *data;
  size_t dataSize;

  size_t currentReadPos;
} tc_audiobuffer;

typedef struct {
  tc_audiobuffer *audioBuffer;
} tc_sound;

typedef struct {
  struct
  {
    ma_context ctx;
    ma_device device;
    ma_mutex lock;

    int isReady;

  } system;

  struct
  {
    tc_audiobuffer buffer[MAX_AUDIO_BUFFER_CHANNELS];
  } multiChannel;
} tc_audio;

/* tc_audio audio; */

/* Audio system */
TCDEF int tc_init_audio();
TCDEF int tc_start_device();
TCDEF int tc_stop_device();
TCDEF void tc_terminate_audio(void);

TCDEF void tc_set_master_volume(float volume);

TCDEF int tc_audio_get_id(tc_audiobuffer *buffer);
TCDEF tc_audiobuffer *tc_get_audio_from_id(unsigned int id);

/* Audio buffer */
TCDEF tc_audiobuffer *tc_load_buffer(const char *filename, TC_AUDIO_USAGE usage);
TCDEF void tc_play_buffer(tc_audiobuffer *audioBuffer);
TCDEF void tc_stop_buffer(tc_audiobuffer *audioBuffer);
TCDEF void tc_pause_buffer(tc_audiobuffer *audioBuffer);
TCDEF void tc_unload_buffer(tc_audiobuffer *audioBuffer);

TCDEF int tc_buffer_is_playing(tc_audiobuffer *audioBuffer);
TCDEF int tc_buffer_is_paused(tc_audiobuffer *audioBuffer);

TCDEF void tc_set_buffer_volume(tc_audiobuffer *audioBuffer, float volume);

/* Sound */
TCDEF tc_sound tc_load_sound(const char *filename, TC_AUDIO_USAGE usage);
TCDEF void tc_unload_sound(tc_sound sound);

TCDEF void tc_play_sound(tc_sound sound);
TCDEF void tc_stop_sound(tc_sound sound);
TCDEF void tc_pause_sound(tc_sound sound);

TCDEF int tc_sound_is_playing(tc_sound sound);
TCDEF int tc_sound_is_paused(tc_sound sound);
TCDEF void tc_sound_set_volume(tc_sound sound, float volume);

#endif

#if defined(TC_AUDIO_IMPLEMENTATION)

#define DR_WAV_IMPLEMENTATION
#include "external/dr_wav.h"

#define DR_FLAC_IMPLEMENTATION
#include "external/dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "external/dr_mp3.h"

#include "external/stb_vorbis.h"

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

static tc_audio audio = {0};

static ma_uint32 tc_read_and_mix_pcm_frames(tc_audiobuffer *audioBuffer, float *pOutputF32, ma_uint32 frameCount)
{

  float temp[4096];
  ma_uint32 tempCapInFrames = ma_countof(temp) / AUDIO_DEVICE_CHANNELS;
  ma_uint32 totalFramesRead = 0;
  ma_decoder *pDecoder = &audioBuffer->decoder;
  float volume = audioBuffer->volume;

  while (totalFramesRead < frameCount)
  {
    ma_uint32 iSample;
    ma_uint32 framesReadThisIteration;
    ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
    ma_uint32 framesToReadThisIteration = tempCapInFrames;
    if (framesToReadThisIteration > totalFramesRemaining)
    {
      framesToReadThisIteration = totalFramesRemaining;
    }

    framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration);

    if (framesReadThisIteration == 0)
    {
      break;
    }

    for (iSample = 0; iSample < framesReadThisIteration * AUDIO_DEVICE_CHANNELS; ++iSample)
    {
      pOutputF32[totalFramesRead * AUDIO_DEVICE_CHANNELS + iSample] += temp[iSample] * volume;
    }

    totalFramesRead += framesReadThisIteration;

    if (framesReadThisIteration < framesToReadThisIteration)
    {
      break;
    }
  }

  return totalFramesRead;
}

static void tc_data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  float *fOutput = (float *)pOutput;

  ma_mutex_lock(&audio.system.lock);
  int i;
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++)
  {
    tc_audiobuffer *audioBuffer = &audio.multiChannel.buffer[i];
    if (audioBuffer->playing && audioBuffer->loaded && !audioBuffer->paused)
    {
      ma_uint32 framesRead = tc_read_and_mix_pcm_frames(audioBuffer, fOutput, frameCount);
      if (framesRead < frameCount)
      {
        if (audioBuffer->loop)
        {
          ma_decoder_seek_to_pcm_frame(&audioBuffer->decoder, 0);
        }
        else
        {
          audioBuffer->playing = TC_FALSE;
        }
      }
    }
  }
  ma_mutex_unlock(&audio.system.lock);

  (void)pInput;
}

TCDEF int tc_init_audio()
{
  ma_context_config ctxConfig = ma_context_config_init();
  ma_result result = ma_context_init(NULL, 0, &ctxConfig, &audio.system.ctx);
  if (result != MA_SUCCESS)
  {
    ERROR("Failed to init audio context");
    return -1;
  }

  ma_device_config devConfig = ma_device_config_init(ma_device_type_playback);
  devConfig.playback.pDeviceID = NULL;
  devConfig.playback.format = AUDIO_DEVICE_FORMAT;
  devConfig.playback.channels = AUDIO_DEVICE_CHANNELS;
  devConfig.sampleRate = AUDIO_DEVICE_SAMPLE_RATE;
  devConfig.pUserData = NULL;
  devConfig.dataCallback = tc_data_callback;

  result = ma_device_init(&audio.system.ctx, &devConfig, &audio.system.device);
  if (result != MA_SUCCESS)
  {
    ERROR("Failed to init device");
    ma_context_uninit(&audio.system.ctx);
    return -1;
  }

  result = tc_start_device();
  if (result != MA_SUCCESS)
  {
    ERROR("Failed to start device");
    ma_context_uninit(&audio.system.ctx);
    ma_device_uninit(&audio.system.device);
    return -1;
  }

  if (ma_mutex_init(&audio.system.ctx, &audio.system.lock) != MA_SUCCESS)
  {
    ERROR("Failed to start mutex");
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.ctx);
    return -1;
  }

  int i;
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++)
  {
    tc_audiobuffer *buffer = &audio.multiChannel.buffer[i];
    buffer->playing = TC_FALSE;
    buffer->volume = 1.f;
    buffer->pitch = 1.f;
    buffer->loaded = TC_FALSE;
    buffer->paused = TC_FALSE;
    buffer->loop = TC_FALSE;
    buffer->id = i;
  }

  return 1;
}

TCDEF int tc_start_device()
{
  return ma_device_start(&audio.system.device);
}

TCDEF int tc_stop_device()
{
  ma_device_stop(&audio.system.device);
}

TCDEF void tc_terminate_audio(void)
{
  if (audio.system.isReady)
  {
    ma_mutex_uninit(&audio.system.lock);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.ctx);
  }
  else
  {
    ERROR("Audio system could not be closed, not initialized");
  }
}

TCDEF void tc_set_master_volume(float volume)
{
  ma_device_set_master_volume(&audio.system.device, volume);
}

TCDEF int tc_sound_get_id(tc_audiobuffer *buffer)
{
  return buffer->id;
}

TCDEF tc_audiobuffer *tc_get_buffer_from_id(unsigned int id)
{
  return &audio.multiChannel.buffer[id];
}

TCDEF tc_audiobuffer *tc_load_buffer(const char *filename, TC_AUDIO_USAGE usage)
{
  int index = 0;
  int i;
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++)
  {
    if (!audio.multiChannel.buffer[i].loaded)
    {
      index = i;
      break;
    }
  }
  tc_audiobuffer *audioBuffer = &audio.multiChannel.buffer[index];
  ma_decoder_config decoderConfig = ma_decoder_config_init(AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, AUDIO_DEVICE_SAMPLE_RATE);
  ma_result result = 0;
  audioBuffer->currentReadPos = 0;

  size_t size;
  char *data = tc_read_file(filename, &size);
  audioBuffer->data = data;
  result = ma_decoder_init_memory(audioBuffer->data, size, &decoderConfig, &audioBuffer->decoder);
//   if (usage == AUDIO_STATIC)
//   {
//     FILE *file;
//     ma_fopen(&file, filename, "rb");
//     fseek(file, 0, SEEK_END);
//     size_t size = ftell(file);
//     fseek(file, 0, SEEK_SET);
//     char *data = malloc(size);
//     size_t r = fread(data, 1, size, (FILE *)file);
//     audioBuffer->data = data;
//     fclose((FILE *)file);

//     result = ma_decoder_init_memory(audioBuffer->data, size, &decoderConfig, &audioBuffer->decoder);
//   }
//   else
//   {
//     result = ma_decoder_init_file(filename, &decoderConfig, &audioBuffer->decoder);
//   }

  if (result != MA_SUCCESS)
  {
    ERROR("Failed to load sound '%s'", filename);
    if (audioBuffer->data)
      free(audioBuffer->data);
    return NULL;
  }
  audioBuffer->loaded = TC_TRUE;
  audioBuffer->playing = TC_FALSE;
  audioBuffer->paused = TC_TRUE;
  audioBuffer->usage = usage;
  audioBuffer->loop = TC_FALSE;

  return audioBuffer;
}

TCDEF void tc_play_buffer(tc_audiobuffer *audioBuffer)
{
  if (audioBuffer)
  {
    LOG("Playing sound %d\n", audioBuffer->id);
    audioBuffer->playing = TC_TRUE;
    audioBuffer->paused = TC_FALSE;
  }
}

TCDEF void tc_stop_buffer(tc_audiobuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->playing = TC_FALSE;
    ma_decoder_seek_to_pcm_frame(&audioBuffer->decoder, 0);
  }
}

TCDEF void tc_pause_buffer(tc_audiobuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->paused = TC_TRUE;
    audioBuffer->playing = TC_FALSE;
  }
}

TCDEF void tc_set_buffer_volume(tc_audiobuffer *audioBuffer, float volume)
{
  if (audioBuffer)
    audioBuffer->volume = volume;
}

TCDEF int tc_buffer_is_playing(tc_audiobuffer *audioBuffer)
{
  return audioBuffer->playing;
}

TCDEF int tc_buffer_is_paused(tc_audiobuffer *audioBuffer)
{
  return audioBuffer->paused;
}

TCDEF void tc_unload_buffer(tc_audiobuffer *audioBuffer)
{
  if (!audioBuffer)
  {
    audioBuffer->loaded = TC_FALSE;
    ma_decoder_uninit(&audioBuffer->decoder);
  }
}

TCDEF tc_sound tc_load_sound(const char *filename, TC_AUDIO_USAGE usage) {
  tc_sound sound = {0};
  sound.audioBuffer = tc_load_buffer(filename, usage);
  return sound;
}
TCDEF void tc_unload_sound(tc_sound sound) {
  tc_unload_buffer(sound.audioBuffer);
}

TCDEF void tc_play_sound(tc_sound sound) {
  tc_play_buffer(sound.audioBuffer);
}

TCDEF void tc_stop_sound(tc_sound sound) {
  tc_stop_buffer(sound.audioBuffer);
}

TCDEF void tc_pause_sound(tc_sound sound) {
  tc_pause_buffer(sound.audioBuffer);
}

TCDEF int tc_sound_is_playing(tc_sound sound) {
  return tc_buffer_is_playing(sound.audioBuffer);
}
TCDEF int tc_sound_is_paused(tc_sound sound) {
  return tc_buffer_is_paused(sound.audioBuffer);
}

TCDEF void tc_sound_set_volume(tc_sound sound, float volume) {
  tc_set_buffer_volume(sound.audioBuffer, volume);
}

#endif

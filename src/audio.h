#ifndef TICO_AUDIO_H
#define TICO_AUDIO_H

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
  TC_AUDIO_STREAM = 0,
  TC_AUDIO_STATIC
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
} tc_AudioBuffer;

typedef struct {
  tc_AudioBuffer *audioBuffer;
} tc_Sound;

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
    tc_AudioBuffer buffer[MAX_AUDIO_BUFFER_CHANNELS];
  } multiChannel;
} tc_Audio;

/* tc_Audio audio; */

/* Audio system */
TCDEF int tc_init_audio();
TCDEF int tc_start_device();
TCDEF int tc_stop_device();
TCDEF void tc_terminate_audio(void);

TCDEF void tc_set_master_volume(float volume);

TCDEF int tc_audio_get_id(tc_AudioBuffer *buffer);
TCDEF tc_AudioBuffer *tc_get_audio_from_id(unsigned int id);

/* Audio buffer */
TCDEF tc_AudioBuffer *tc_load_buffer(const char *filename, TC_AUDIO_USAGE usage);
TCDEF void tc_play_buffer(tc_AudioBuffer *audioBuffer);
TCDEF void tc_stop_buffer(tc_AudioBuffer *audioBuffer);
TCDEF void tc_pause_buffer(tc_AudioBuffer *audioBuffer);
TCDEF void tc_unload_buffer(tc_AudioBuffer *audioBuffer);

TCDEF int tc_buffer_is_playing(tc_AudioBuffer *audioBuffer);
TCDEF int tc_buffer_is_paused(tc_AudioBuffer *audioBuffer);

TCDEF void tc_set_buffer_volume(tc_AudioBuffer *audioBuffer, float volume);

/************
 * Sound
 ************/
TCDEF tc_Sound tc_load_sound(const char *filename, TC_AUDIO_USAGE usage);
TCDEF void tc_unload_sound(tc_Sound sound);

TCDEF void tc_play_sound(tc_Sound sound);
TCDEF void tc_stop_sound(tc_Sound sound);
TCDEF void tc_pause_sound(tc_Sound sound);

TCDEF int tc_sound_is_playing(tc_Sound sound);
TCDEF int tc_sound_is_paused(tc_Sound sound);
TCDEF void tc_sound_set_volume(tc_Sound sound, float volume);

#endif

#if defined(TICO_AUDIO_IMPLEMENTATION)

#define DR_WAV_IMPLEMENTATION
#include "external/dr_wav.h"

#define DR_FLAC_IMPLEMENTATION
#include "external/dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "external/dr_mp3.h"

#include "external/stb_vorbis.h"

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

static tc_Audio audio = {0};

static ma_uint32 tc_read_and_mix_pcm_frames(tc_AudioBuffer *audioBuffer, float *pOutputF32, ma_uint32 frameCount)
{

  float temp[4096];
  ma_uint32 tempCapInFrames = ma_countof(temp) / AUDIO_DEVICE_CHANNELS;
  ma_uint32 totalFramesRead = 0;
  ma_decoder *pDecoder = &audioBuffer->decoder;
  float volume = audioBuffer->volume;

  while (totalFramesRead < frameCount) {
    ma_uint32 iSample;
    ma_uint32 framesReadThisIteration;
    ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
    ma_uint32 framesToReadThisIteration = tempCapInFrames;
    if (framesToReadThisIteration > totalFramesRemaining) {
      framesToReadThisIteration = totalFramesRemaining;
    }

    if (audioBuffer->usage == TC_AUDIO_STREAM) {
      framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration);
    } else {
      framesReadThisIteration = framesToReadThisIteration;
      memcpy(temp, audioBuffer->data + audioBuffer->currentReadPos, framesToReadThisIteration);
      audioBuffer->currentReadPos += framesReadThisIteration;
    }


    if (framesReadThisIteration == 0) {
      break;
    }

    for (iSample = 0; iSample < framesReadThisIteration * AUDIO_DEVICE_CHANNELS; ++iSample) {
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
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++) {
    tc_AudioBuffer *audioBuffer = &audio.multiChannel.buffer[i];
    if (audioBuffer->playing && audioBuffer->loaded && !audioBuffer->paused) {
      ma_uint32 framesRead = tc_read_and_mix_pcm_frames(audioBuffer, fOutput, frameCount);
      if (framesRead < frameCount) {
        if (audioBuffer->loop) {
          ma_decoder_seek_to_pcm_frame(&audioBuffer->decoder, 0);
          audioBuffer->currentReadPos = 0;
        } else {
          audioBuffer->playing = tc_false;
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
    TRACEERR("Failed to init audio context");
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
    TRACEERR("Failed to init device");
    ma_context_uninit(&audio.system.ctx);
    return -1;
  }

  result = tc_start_device();
  if (result != MA_SUCCESS)
  {
    TRACEERR("Failed to start device");
    ma_context_uninit(&audio.system.ctx);
    ma_device_uninit(&audio.system.device);
    return -1;
  }

  if (ma_mutex_init(&audio.system.ctx, &audio.system.lock) != MA_SUCCESS)
  {
    TRACEERR("Failed to start mutex");
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.ctx);
    return -1;
  }

  int i;
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++)
  {
    tc_AudioBuffer *buffer = &audio.multiChannel.buffer[i];
    buffer->playing = tc_false;
    buffer->volume = 1.f;
    buffer->pitch = 1.f;
    buffer->loaded = tc_false;
    buffer->paused = tc_false;
    buffer->loop = tc_false;
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
    TRACEERR("Audio system could not be closed, not initialized");
  }
}

TCDEF void tc_set_master_volume(float volume)
{
  ma_device_set_master_volume(&audio.system.device, volume);
}

TCDEF int tc_sound_get_id(tc_AudioBuffer *buffer)
{
  return buffer->id;
}

TCDEF tc_AudioBuffer *tc_get_buffer_from_id(unsigned int id)
{
  return &audio.multiChannel.buffer[id];
}

TCDEF tc_AudioBuffer *tc_load_buffer(const char *filename, TC_AUDIO_USAGE usage)
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
  tc_AudioBuffer *audioBuffer = &audio.multiChannel.buffer[index];
  ma_decoder_config decoderConfig = ma_decoder_config_init(AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, AUDIO_DEVICE_SAMPLE_RATE);
  ma_result result = 0;
  audioBuffer->currentReadPos = 0;

  if (usage == TC_AUDIO_STREAM) {
    size_t size;
    char *data = tc_read_file(filename, &size);
    audioBuffer->data = data;
    result = ma_decoder_init_memory(audioBuffer->data, size, &decoderConfig, &audioBuffer->decoder);
  } else {
    size_t size;
    char *data = tc_read_file(filename, &size);
    ma_uint64 pFrameCountOut;
    void *ppData;
    result = ma_decode_memory(data, size, &decoderConfig, &pFrameCountOut, &ppData);
    audioBuffer->data = ppData;
    free(data);
  }

  if (result != MA_SUCCESS)
  {
    TRACEERR("Failed to load sound '%s'", filename);
    if (audioBuffer->data)
      free(audioBuffer->data);
    return NULL;
  }
  audioBuffer->loaded = tc_true;
  audioBuffer->playing = tc_false;
  audioBuffer->paused = tc_true;
  audioBuffer->usage = usage;
  audioBuffer->loop = tc_false;

  return audioBuffer;
}

TCDEF void tc_play_buffer(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    TRACELOG("Playing sound %d", audioBuffer->id);
    audioBuffer->playing = tc_true;
    audioBuffer->paused = tc_false;
  }
}

TCDEF void tc_stop_buffer(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->playing = tc_false;
    ma_decoder_seek_to_pcm_frame(&audioBuffer->decoder, 0);
  }
}

TCDEF void tc_pause_buffer(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->paused = tc_true;
    audioBuffer->playing = tc_false;
  }
}

TCDEF void tc_set_buffer_volume(tc_AudioBuffer *audioBuffer, float volume)
{
  if (audioBuffer)
    audioBuffer->volume = volume;
}

TCDEF int tc_buffer_is_playing(tc_AudioBuffer *audioBuffer)
{
  return audioBuffer->playing;
}

TCDEF int tc_buffer_is_paused(tc_AudioBuffer *audioBuffer)
{
  return audioBuffer->paused;
}

TCDEF void tc_unload_buffer(tc_AudioBuffer *audioBuffer)
{
  if (!audioBuffer)
  {
    audioBuffer->loaded = tc_false;
    ma_decoder_uninit(&audioBuffer->decoder);
  }
}

TCDEF tc_Sound tc_load_sound(const char *filename, TC_AUDIO_USAGE usage) {
  tc_Sound sound = {0};
  sound.audioBuffer = tc_load_buffer(filename, usage);
  return sound;
}
TCDEF void tc_unload_sound(tc_Sound sound) {
  tc_unload_buffer(sound.audioBuffer);
}

TCDEF void tc_play_sound(tc_Sound sound) {
  tc_play_buffer(sound.audioBuffer);
}

TCDEF void tc_stop_sound(tc_Sound sound) {
  tc_stop_buffer(sound.audioBuffer);
}

TCDEF void tc_pause_sound(tc_Sound sound) {
  tc_pause_buffer(sound.audioBuffer);
}

TCDEF int tc_sound_is_playing(tc_Sound sound) {
  return tc_buffer_is_playing(sound.audioBuffer);
}
TCDEF int tc_sound_is_paused(tc_Sound sound) {
  return tc_buffer_is_paused(sound.audioBuffer);
}

TCDEF void tc_sound_set_volume(tc_Sound sound, float volume) {
  tc_set_buffer_volume(sound.audioBuffer, volume);
}

#endif

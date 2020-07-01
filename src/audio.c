#include "tico.h"

#define DR_WAV_IMPLEMENTATION
#include "external/dr_wav.h"

#define DR_FLAC_IMPLEMENTATION
#include "external/dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "external/dr_mp3.h"

#include "external/stb_vorbis.h"

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

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

static tc_Audio audio = {0};

static ma_uint32 tic_read_and_mix_pcm_frames(tc_AudioBuffer *audioBuffer, float *pOutputF32, ma_uint32 frameCount)
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

    if (audioBuffer->data->usage == TIC_AUDIO_STREAM) {
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

static void tic_data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  float *fOutput = (float *)pOutput;

  ma_mutex_lock(&audio.system.lock);
  int i;
  for (i = 0; i < MAX_AUDIO_BUFFER_CHANNELS; i++) {
    tc_AudioBuffer *audioBuffer = &audio.multiChannel.buffer[i];
    if (audioBuffer->playing && audioBuffer->loaded && !audioBuffer->paused) {
      ma_uint32 framesRead = tic_read_and_mix_pcm_frames(audioBuffer, fOutput, frameCount);
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

int tic_audio_init()
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
  devConfig.dataCallback = tic_data_callback;

  result = ma_device_init(&audio.system.ctx, &devConfig, &audio.system.device);
  if (result != MA_SUCCESS)
  {
    TRACEERR("Failed to init device");
    ma_context_uninit(&audio.system.ctx);
    return -1;
  }

  result = tic_audio_start_device();
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

  audio.system.isReady = tc_true;

  TRACELOG("Audio system initiated");

  return 1;
}

int tic_audio_start_device()
{
  return ma_device_start(&audio.system.device);
}

int tic_audio_stop_device()
{
  ma_device_stop(&audio.system.device);
}

void tic_audio_terminate(void) {
  if (audio.system.isReady) {
    ma_mutex_uninit(&audio.system.lock);
    ma_device_uninit(&audio.system.device);
    ma_context_uninit(&audio.system.ctx);
  } else {
    TRACEERR("Audio system could not be closed, not initialized");
  }
  TRACELOG("Audio system finalized");
}

void tic_audio_set_master_volume(float volume)
{
  ma_device_set_master_volume(&audio.system.device, volume);
}

int tic_audio_get_id(tc_AudioBuffer *buffer)
{
  return buffer->id;
}

tc_AudioBuffer *tic_audio_get_from_id(unsigned int id)
{
  return &audio.multiChannel.buffer[id];
}

tc_AudioBuffer *tic_buffer_load(const char *filename, TIC_AUDIO_USAGE usage)
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

  if (usage == TIC_AUDIO_STREAM) {
    // tc_AudioData *aData = tic_resources_get_sound(filename);
  	tc_AudioData *aData = NULL;
    if (aData && aData->usage == usage) {
      audioBuffer->data = aData;
    } else {
      aData = malloc(sizeof(*aData));
      aData->data = tic_filesystem_read_file(filename, &aData->size);
      aData->usage = usage;
    }

    result = ma_decoder_init_memory(aData->data, aData->size, &decoderConfig, &audioBuffer->decoder);
    audioBuffer->data = aData;
    // tic_resources_add_sound(filename, aData);
    aData->refs++;
  } else {
    size_t size;
    char *data = tic_filesystem_read_file(filename, &size);
    ma_uint64 pFrameCountOut;
    void *ppData;
    result = ma_decode_memory(data, size, &decoderConfig, &pFrameCountOut, &ppData);
    audioBuffer->data = ppData;
    free(data);
  }

  if (result != MA_SUCCESS)
  {
    TRACEERR("Failed to load sound '%s'", filename);
    return NULL;
  }
  audioBuffer->loaded = tc_true;
  audioBuffer->playing = tc_false;
  audioBuffer->paused = tc_true;
//   audioBuffer->usage = usage;
  audioBuffer->loop = tc_false;

  return audioBuffer;
}

void tic_buffer_play(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    TRACELOG("Playing sound %d", audioBuffer->id);
    audioBuffer->playing = tc_true;
    audioBuffer->paused = tc_false;
  }
}

void tic_buffer_stop(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->playing = tc_false;
    ma_decoder_seek_to_pcm_frame(&audioBuffer->decoder, 0);
  }
}

void tic_buffer_pause(tc_AudioBuffer *audioBuffer)
{
  if (audioBuffer)
  {
    audioBuffer->paused = tc_true;
    audioBuffer->playing = tc_false;
  }
}

void tic_buffer_set_volume(tc_AudioBuffer *audioBuffer, float volume)
{
  if (audioBuffer)
    audioBuffer->volume = volume;
}

int tic_buffer_is_playing(tc_AudioBuffer *audioBuffer)
{
  return audioBuffer->playing;
}

int tic_buffer_is_paused(tc_AudioBuffer *audioBuffer)
{
  return audioBuffer->paused;
}

void tic_buffer_unload(tc_AudioBuffer *audioBuffer) {
  if (audioBuffer) {
    audioBuffer->loaded = tc_false;
    audioBuffer->data->refs--;
    audioBuffer->data = NULL;
    ma_decoder_uninit(&audioBuffer->decoder);
  }
}

tc_Sound tic_sound_load(const char *filename, TIC_AUDIO_USAGE usage) {
  tc_Sound sound = {0};
  sound.audioBuffer = tic_buffer_load(filename, usage);
  return sound;
}
void tic_sound_unload(tc_Sound sound) {
  tic_buffer_unload(sound.audioBuffer);
}

void tic_sound_play(tc_Sound sound) {
  tic_buffer_play(sound.audioBuffer);
}

void tic_sound_stop(tc_Sound sound) {
  tic_buffer_stop(sound.audioBuffer);
}

void tic_sound_pause(tc_Sound sound) {
  tic_buffer_pause(sound.audioBuffer);
}

int tic_sound_is_playing(tc_Sound sound) {
  return tic_buffer_is_playing(sound.audioBuffer);
}
int tic_sound_is_paused(tc_Sound sound) {
  return tic_buffer_is_paused(sound.audioBuffer);
}

void tic_sound_set_volume(tc_Sound sound, float volume) {
  tic_buffer_set_volume(sound.audioBuffer, volume);
}

/********************
 * Audio Data
 ********************/

void tic_audio_data_free(tc_AudioData *data) {
  free(data->data);
}

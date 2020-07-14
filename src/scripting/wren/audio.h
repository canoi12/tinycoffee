#include "../../tico.h"

/******************
 * Audio
 ******************/

static void tic_wren_audio_static(WrenVM *vm) {
  wrenSetSlotDouble(vm, 0, TIC_AUDIO_STATIC);
}

static void tic_wren_audio_stream(WrenVM *vm) {
  wrenSetSlotDouble(vm, 0, TIC_AUDIO_STREAM);
}

static void tic_wren_audio_set_master_volume(WrenVM *vm) {
  float volume = wrenGetSlotDouble(vm, 1);
  tic_audio_set_master_volume(volume);
}

static void tic_wren_audio_start_device(WrenVM *vm) {
  tic_audio_start_device();
}
static void tic_wren_audio_stop_device(WrenVM *vm) {
  tic_audio_stop_device();
}

static tc_WrenLib wrenAudioLib[] = {
  {"s_setMasterVolume(_)", tic_wren_audio_set_master_volume},
  {"s_startDevice()", tic_wren_audio_start_device},
  {"s_stopDevice()", tic_wren_audio_stop_device},
  {"s_STATIC", tic_wren_audio_static},
  {"s_STREAM", tic_wren_audio_stream}
};

/*****************
 * Sound
 *****************/

static void tic_wren_sound_allocate(WrenVM *vm) {
  const char *filename = wrenGetSlotString(vm, 1);
  int audioType = TIC_AUDIO_STREAM;
  if (wrenGetSlotCount(vm) > 2)
    audioType = wrenGetSlotDouble(vm, 2);

  tc_Sound *sound = (tc_Sound*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Sound));

  *sound = tic_sound_load(filename, audioType);
}

static void tic_wren_sound_finalize(void *data)
{
  tc_Sound *sound = (tc_Sound*)data;
  tic_sound_unload(*sound);
  TIC_FREE(sound);
}

static void tic_wren_sound_play(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tic_sound_play(*sound);
}
static void tic_wren_sound_stop(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tic_sound_stop(*sound);
}
static void tic_wren_sound_pause(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tic_sound_pause(*sound);
}

static void tic_wren_sound_is_playing(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotBool(vm, 0, tic_sound_is_playing(*sound));
}
static void tic_wren_sound_is_paused(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotBool(vm, 0, tic_sound_is_paused(*sound));
}

static tc_WrenLib wrenSoundLib[] = {
  {"play()", tic_wren_sound_play},
  {"stop()", tic_wren_sound_stop},
  {"pause()", tic_wren_sound_pause},
  {"isPlaying", tic_wren_sound_is_playing},
  {"isPaused", tic_wren_sound_is_paused}
};

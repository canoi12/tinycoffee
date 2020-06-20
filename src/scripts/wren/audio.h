#include "module.h"

static const char *tcWrenAudioModule =
"foreign class Sound {\n"
" construct load(filename) {}\n"
" construct load(filename, type) {}\n"
" foreign play()\n"
" foreign stop()\n"
" foreign pause()\n"
" foreign isPlaying\n"
" foreign isPaused\n"
"}\n"
"foreign class Audio {\n"
" foreign static setMasterVolume(volume)\n"
" foreign static startDevice()\n"
" foreign static stopDevice()\n"
" foreign static STATIC\n"
" foreign static STREAM\n"
"}\n";

/*******************
 * Audio Module
 *******************/

static void wren_audio_static(WrenVM *vm)
{
  wrenSetSlotDouble(vm, 0, TC_AUDIO_STATIC);
}
static void wren_audio_stream(WrenVM *vm)
{
  wrenSetSlotDouble(vm, 0, TC_AUDIO_STREAM);
}

static void wren_audio_set_master_volume(WrenVM *vm) {
  float volume = wrenGetSlotDouble(vm, 1);
  tc_set_master_volume(volume);
}

static void wren_audio_start_device(WrenVM *vm) {
  tc_start_device();
}
static void wren_audio_stop_device(WrenVM *vm) {
  tc_stop_device();
}

static tc_WrenLib wrenAudioLib[] = {
  {"s_setMasterVolume(_)", wren_audio_set_master_volume},
  {"s_startDevice()", wren_audio_start_device},
  {"s_stopDevice()", wren_audio_stop_device},
  {"s_STATIC", wren_audio_static},
  {"s_STREAM", wren_audio_stream}
};

/*******************
 * Sound class
 *******************/

static void wren_sound_allocate(WrenVM *vm)
{
  const char *filename = wrenGetSlotString(vm, 1);
  int audioType = TC_AUDIO_STREAM;
  if (wrenGetSlotCount(vm) > 2)
    audioType = wrenGetSlotDouble(vm, 2);

  tc_Sound *sound = (tc_Sound*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(tc_Sound));

  *sound = tc_load_sound(filename, audioType);
}
static void wren_sound_finalize(void *data)
{
  tc_Sound *sound = (tc_Sound*)data;
  tc_unload_sound(*sound);
  free(sound);
}

static void wren_sound_play(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tc_play_sound(*sound);
}
static void wren_sound_stop(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tc_stop_sound(*sound);
}
static void wren_sound_pause(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  tc_pause_sound(*sound);
}

static void wren_sound_is_playing(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotBool(vm, 0, tc_sound_is_playing(*sound));
}
static void wren_sound_is_paused(WrenVM *vm)
{
  tc_Sound *sound = (tc_Sound*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotBool(vm, 0, tc_sound_is_paused(*sound));
}

static tc_WrenLib wrenSoundLib[] = {
  {"play()", wren_sound_play},
  {"stop()", wren_sound_stop},
  {"pause()", wren_sound_pause},
  {"isPlaying", wren_sound_is_playing},
  {"isPaused", wren_sound_is_paused}
};

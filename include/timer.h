#ifndef TICO_TIMER_H
#define TICO_TIMER_H

#include "definitions.h"

typedef struct tc_Timer {
	double currentTime;
  double lastTime;
  double fpsLastTime;
  float delta;
  int fps;
  int frames;
} tc_Timer;

TIC_API int tico_timer_init(tc_Timer *timer);
TIC_API void tico_timer_terminate(tc_Timer *timer);

TIC_API void INTERNAL_API(tico_timer_update, tc_Timer *timer);
TIC_API float INTERNAL_API(tico_timer_get_time, tc_Timer *timer);
TIC_API float INTERNAL_API(tico_timer_get_delta, tc_Timer *timer);
TIC_API int INTERNAL_API(tico_timer_get_fps, tc_Timer *timer);
TIC_API void INTERNAL_API(tico_timer_wait, tc_Timer *timer, float ms);

#endif // TICO_TIMER_H
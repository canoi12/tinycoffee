#include "tico.h"

int tico_timer_init(tc_Timer *timer) {
	return 1;
}
void tico_timer_terminate(tc_Timer *timer) {

}

void INTERNAL_API(tico_timer_update, tc_Timer *timer) {
	ASSERT(timer != NULL);
  timer->currentTime = glfwGetTime();
  timer->delta = timer->currentTime - timer->lastTime;
  timer->lastTime = timer->currentTime;

  float delta = timer->currentTime - timer->fpsLastTime;
  timer->frames++;
  if (delta >= 1) {
    timer->fps = timer->frames;
    timer->frames = 0;
    timer->fpsLastTime = timer->currentTime;
    // tico_timer_wait(delta);
  }
}

float INTERNAL_API(tico_timer_get_time, tc_Timer *timer) {
	ASSERT(timer != NULL);
  return timer->currentTime;
}

float INTERNAL_API(tico_timer_get_delta, tc_Timer *timer) {
	ASSERT(timer != NULL);
  return timer->delta;
}

int INTERNAL_API(tico_timer_get_fps, tc_Timer *timer) {
	ASSERT(timer != NULL);
  return timer->fps;
}

void INTERNAL_API(tico_timer_wait, tc_Timer *timer, float ms) {
	// ASSERT(timer != NULL);
	#if defined(_WIN32)
		Sleep((unsigned int) ms);
	#elif defined(__linux__)
		struct timespec req = {0};
		time_t sec = (int)(ms/1000.f);
		ms -= (sec * 1000);
		req.tv_sec = sec;
		req.tv_nsec = ms * 1000000L;

		while(nanosleep(&req, &req) == -1) continue;
	#elif defined(__APPLE__)
			usleep(ms * 1000.f);
	#endif
}
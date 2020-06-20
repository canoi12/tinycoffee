#ifndef TICO_DEBUG_H
#define TICO_DEBUG_H

#include "tinycoffee.h"

#include <time.h>
#include <stdarg.h>

TCDEF void tc_log(int type, const tc_uint8 *fmt, ...);
TCDEF void tc_tracelog(int type, const tc_uint8 *file, const tc_uint8 *function, int line, const tc_uint8 *fmt, ...);

#endif /* TICO_DEBUG_H */

#if defined(TICO_DEBUG_IMPLEMENTATION)

TCDEF void tc_log(int type, const tc_uint8 *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  tc_uint8 err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR: ");
  tc_uint8 buffer[1024];
  tc_uint8 bufmsg[512];

  tm_now = localtime(&t);
  tc_uint8 buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s", buf, err);
  va_start(args, fmt);
  vfprintf(stderr, (const char*)fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

TCDEF void tc_tracelog(int type, const tc_uint8 *file, const tc_uint8 *function, int line, const tc_uint8 *fmt, ...) {
  time_t t = time(NULL);
  struct tm *tm_now;

  va_list args;

  tc_uint8 err[15] = "";
  if (type == 1) sprintf((char*)err, "ERROR in ");
  tc_uint8 buffer[1024];
  tc_uint8 bufmsg[512];

  tm_now = localtime(&t);
  tc_uint8 buf[10];
  strftime((char*)buf, sizeof(buf), "%H:%M:%S", tm_now);
  fprintf(stderr, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  // sprintf(buffer, "%s %s:%d %s%s(): ", buf, file, line, err, function);
  va_start(args, fmt);
  // vsprintf(bufmsg, (const char*)fmt, args);
  vfprintf(stderr, (const char*)fmt, args);
  // fprintf(stderr, "%s", bufmsg);
  va_end(args);
  fprintf(stderr, "\n");
  // strcat(buffer, bufmsg);
  // tc_editor_write_log(buffer);
}


#endif /* TICO_DEBUG_IMPLEMENTATION */

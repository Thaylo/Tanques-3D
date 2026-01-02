/**
 * Timer.cpp - Time utilities implementation
 */

#include "core/Timer.h"
#include <sys/time.h>

long getCurrentTime() {
  timeval tp;
  gettimeofday(&tp, nullptr);
  return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

/**
 * test_timer.cpp - Unit tests for Timer utility
 */

#include "core/Timer.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

// Test that getCurrentTime returns a reasonable value
TEST(TimerTest, ReturnsPositiveValue) {
  long time = getCurrentTime();
  EXPECT_GT(time, 0);
}

// Test that getCurrentTime is monotonically increasing
TEST(TimerTest, MonotonicallyIncreasing) {
  long time1 = getCurrentTime();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  long time2 = getCurrentTime();
  EXPECT_GT(time2, time1);
}

// Test time difference is approximately correct
TEST(TimerTest, MeasuresElapsedTime) {
  long start = getCurrentTime();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  long end = getCurrentTime();

  long elapsed = end - start;
  // Should be at least 40ms (allowing for timing variance)
  EXPECT_GE(elapsed, 40);
  // Should be less than 150ms (reasonable upper bound)
  EXPECT_LE(elapsed, 150);
}

// Test that time is in milliseconds (reasonable magnitude)
TEST(TimerTest, ValueInMilliseconds) {
  long time = getCurrentTime();
  // Current time in ms from Unix epoch should be > 1 billion (around year
  // 2001+)
  EXPECT_GT(time, 1000000000L);
}

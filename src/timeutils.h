#pragma once
// std
#include <stdint.h>
// libraries
#include <TimeLib.h>

// Copied from timelib.cpp because it is not exposed via the header file
#define LEAP_YEAR(Y)                                                           \
  (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) &&                                \
   (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))

static const uint8_t monthDays[] = {0,  31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};

// Create a time_t at the start of the given day
inline time_t createTime(uint8_t year, uint8_t month, uint8_t day) {
  tmElements_t tmElements = {
      0,
      0,
      0,
      0,
      day,
      month,
      static_cast<uint8_t>(CalendarYrToTm(static_cast<int>(year)))};
  return makeTime(tmElements);
}

time_t getSameDayPreviousMonth();

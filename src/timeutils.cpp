// std
#include <algorithm>
#include <stdint.h>
// libraries
#include <TimeLib.h>
// project
#include "./timeutils.h"


time_t getSameDayPreviousMonth() {
  uint8_t currentYear = year();
  uint8_t prevMonth{};
  if (month() == 1) {
    prevMonth = 12;
    currentYear -= 1;
  } else {
      prevMonth = month() - 1;
  }

  // Leap year exception
  uint8_t daysInMonth = monthDays[prevMonth];
  if (month() == 2 && LEAP_YEAR(year())) {
    daysInMonth += 1;
  }

  uint8_t currentday = std::min(static_cast<uint8_t>(day()), daysInMonth);

  time_t to = createTime(currentYear, prevMonth, currentday);
  return to;
}

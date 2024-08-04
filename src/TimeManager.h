#pragma once
// std
#include <coredecls.h> // TrivialCB
#include <time.h>

// Syncs the time via the NTP service
// Glue to combine std time.cpp, the TimeLib and MyAlarm libraries.
class TimeManager {
public:
  TimeManager();

  void syncWithNTP(const char *tz, const TrivialCB &cb);
  void printTimeStatus() const;
  void printTime(const time_t tNow) const;
  void createHour(int8_t _minute, int8_t _second, const TrivialCB &_callback);
  void createDay(int8_t _hour, int8_t _minute, int8_t _second, const TrivialCB &_callback);
  void update();

private:
  void onTimeSynced();
  void configureTimeLib();
  void configureMyAlarm();
};

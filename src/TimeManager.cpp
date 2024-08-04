// std
#include <coredecls.h> // TrivialCB
#include <ctime>
#include <time.h>
// libraries
#include <ArduinoLog.h>
// MyAlarm creates global variables within the header file.
// Therefore the header file can only be included once in the whole project
// By wrapping MyAlarm within TimeManager, it can be used in multiple locations.
#include <MyAlarm.h>
#include <TimeLib.h>
// project
#include "./TimeManager.h"
#include "./format.h"

TimeManager::TimeManager() {
  // From the old code, not sure why its needed
  MyAlarm::stopService();
}

void TimeManager::syncWithNTP(const char *tz, const TrivialCB &cb) {
  // settimeofday_cb from time.h called after
  // successful configTime with NTP server
  settimeofday_cb([this, cb]() -> void {
    Log.infoln("Time is set.");
    this->onTimeSynced();
    cb();
  });

  // configTime from time.h
  // TODO: There seems to be no retry mechanism
  //  therefore when the first NTP request failes
  //  the time dependent code is never run
  configTime(tz, "pool.ntp.org");
}

void TimeManager::printTimeStatus() const {
  // timeStatus from TimeLib.h
  timeStatus_t ts = timeStatus();
  if (ts == timeSet) {
    Log.infoln("Time is set");
  } else if (ts == timeNeedsSync) {
    Log.infoln("Time need sync");
  } else if (ts == timeNotSet) {
    Log.warningln("Time not set");
  }

  Log.verboseln("TimeManager:now %d", now());
  Log.verboseln("TimeManager:millis %d", millis());
  this->printTime(now());
}

void TimeManager::printTime(const time_t tNow) const {
  std::string gmtTime = fmt::format("{:%Y-%m-%d %H:%M:%S}", *gmtime(&tNow));
  std::string localTime =
      fmt::format("{:%Y-%m-%d %H:%M:%S}", *localtime(&tNow));
  Log.verboseln("TimeManager:GMT %s", gmtTime.c_str());
  Log.verboseln("TimeManager:LocalTime %s", localTime.c_str());
}

void TimeManager::createHour(int8_t _minute, int8_t _second,
                             const TrivialCB &_callback) {
  Log.traceln("TimeManager::createHour");
  MyAlarm::createHour(_minute, _second, _callback);
}
void TimeManager::createDay(int8_t _hour, int8_t _minute, int8_t _second,
                            const TrivialCB &_callback) {
  Log.traceln("TimeManager::createDay");
  MyAlarm::createDay(_hour, _minute, _second, _callback);
}

void TimeManager::update() { MyAlarm::update(); }

void TimeManager::onTimeSynced() {
  Log.traceln("TimeManager::onTimeSynced");
  this->configureTimeLib();
  this->configureMyAlarm();
}

void TimeManager::configureTimeLib() {
  Log.traceln("TimeManager::configureTimeLib");
  // setSyncProvider of TimeLib.h configured with time from time.h
  setSyncProvider([]() -> time_t { return time(nullptr); });
}

void TimeManager::configureMyAlarm() {
  Log.traceln("TimeManager::configureMyAlarm");
  // Configure timerAlarm to work with the local times instead of GMT
  MyAlarm::setTimeProvider([]() -> time_t {
    time_t tNow = time(nullptr);
    // TODO: How do we handle Summer/Winter time changes?
    return mktime(localtime(&tNow));
  });
  MyAlarm::startService();
}

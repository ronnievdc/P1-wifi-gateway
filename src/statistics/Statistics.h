#pragma once
// std
#include <stdint.h>
#include <string>
#include <time.h>
// arduino
#include <FS.h>
#include <HardwareSerial.h>
// libraries
#include <ArduinoLog.h>
#include <TimeLib.h>
// project
#include "./DSMRData.h"
#include "./format.h"
#include "./statistics/StatisticsStream.h"
#include "./statistics/StatisticsLine.h"

typedef std::function<bool(const StatisticsLine &line)> LineFilter;

const LineFilter NO_FILTER = [](const StatisticsLine &line) -> bool {
  return true;
};

class Statistics {
public:
  explicit Statistics(FS &fileSystem) : mFileSystem(fileSystem) {}

  // rename today.log -> yesterday.log
  bool rotateDay();

  // saves values in today.log
  inline void saveHour(const DSMRData &data) {
    Log.verboseln("Statistics::saveHour");
    saveToFile(data, mFilenameToday);
  }

  // saves values in <year>.log
  inline void saveday(const DSMRData &data) {
    int currentYear = year();
    saveToFile(data, getYearLog(currentYear));
  }

  // streams values of yesterday.log + today.log
  void sendHours(const StatisticsStream &stream);

  // streams values of <year>.log in between from timestamp and to timestamp
  void sendDays(const StatisticsStream &stream, const time_t from,
                const time_t to, LineFilter filter);

  void sendDays(const StatisticsStream &stream, const time_t from,
                const time_t to) {
    sendDays(stream, from, to, NO_FILTER);
  }

  void sendWeeks(const StatisticsStream &stream, const time_t from,
                 const time_t to, const uint8_t weekday);

  void sendMonths(const StatisticsStream &stream, const time_t from,
                  const time_t to);

  bool getFirstOfToday(StatisticsLine &line);
  bool getFirstOfMonth(StatisticsLine &line);

private:
  FS &mFileSystem;
  const String mFilenameToday = "today.log";
  const String mFilenameYesterday = "yesterday.log";

  static inline const String getYearLog(const int year) {
    const std::string filename = fmt::format("{}.log", year);
    const String filenameYear(filename.c_str());
    return filenameYear;
  }

  void saveToFile(const DSMRData &data, const String filename);
  void streamFile(const StatisticsStream &stream, const String filename,
                  const time_t from, const time_t to, LineFilter filter);
};

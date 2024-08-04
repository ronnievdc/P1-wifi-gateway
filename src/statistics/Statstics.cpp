// std
#include <stdint.h>
#include <time.h>
// arduino
#include <FS.h>
#include <WString.h>
// libraries
#include <ArduinoLog.h>
// project
#include "./DSMRData.h"
#include "./Statistics.h"
#include "./StatisticsLine.h"
#include "./StatisticsStream.h"
#include "./timeutils.h"

bool Statistics::rotateDay() {
  Log.traceln("Statistics::rotateDay");

  mFileSystem.remove(mFilenameYesterday);
  const bool success = mFileSystem.rename(mFilenameToday, mFilenameYesterday);

  if (!success) {
    Log.warningln("Statistics: Failed to rename %s to %s.",
                  mFilenameToday.c_str(), mFilenameYesterday.c_str());
  }
  return success;
}

void Statistics::saveToFile(const DSMRData &data, const String filename) {
  Log.traceln("Statistics::saveToFile");

  // Convert DSMRData to StatisticsLine
  StatisticsLine line = {
      1,
      now(),
      data.energy_delivered_tariff1.int_val(),
      data.energy_delivered_tariff2.int_val(),
      data.energy_returned_tariff1.int_val(),
      data.energy_returned_tariff2.int_val(),
      static_cast<uint32_t>(data.gas_delivered.toFloat() * 1000)};

  line.printLine();

  File file = mFileSystem.open(filename, "a");
  line.saveToFile(file);
  file.write('\n');
  file.close();
}

void Statistics::sendHours(const StatisticsStream &stream) {
  Log.traceln("Statistics::sendHours");
  streamFile(stream, mFilenameYesterday, 0, 0, NO_FILTER);
  streamFile(stream, mFilenameToday, 0, 0, NO_FILTER);
}

void Statistics::sendDays(const StatisticsStream &stream, const time_t from,
                          const time_t to, LineFilter filter) {
  Log.traceln("Statistics::sendDays");
  const int fromYear = year(from);
  const int toYear = year(to);
  for (int targetYear = fromYear; targetYear <= toYear; targetYear++) {
    String filenameYear = getYearLog(targetYear);
    streamFile(stream, filenameYear, from, to, filter);
  }
}

void Statistics::sendWeeks(const StatisticsStream &stream, const time_t from,
                           const time_t to, const uint8_t measureWeekDay) {
  sendDays(stream, from, to, [measureWeekDay](const StatisticsLine &line) -> bool {
    return weekday() == measureWeekDay;
  });
}

void Statistics::sendMonths(const StatisticsStream &stream, const time_t from,
                            const time_t to) {
  sendDays(stream, from, to, [](const StatisticsLine &line) -> bool {
    return day() == 1;
  });
}

void Statistics::streamFile(const StatisticsStream &stream,
                            const String filename, const time_t from,
                            const time_t to, LineFilter filter) {
  Log.traceln("Statistics::streamFile %s", filename.c_str());
  File file = mFileSystem.open(filename, "r");
  if (!file) {
    Log.warningln("Statistics: Failed to open file %s.", filename.c_str());
    return;
  }

  StatisticsLine line;
  while (file.available()) {
    const uint8_t version = file.peek();

    // For now only version 1 is used
    // Version can be increased
    // if the dataformat would change in the future
    if (version == 1) {
      line.loadFromFile(file);
      const char newLine = file.read(); // Read the '\n'
      if (newLine != '\n') {
        Log.warningln("Statistics: Line does not end with newline");
        file.readStringUntil('\n');
      } else {
        // TODO: This is not the best solution
        // the Statistics class should have no knowledge
        // of the StatisticsLine's timestamp value
        if ((from == 0 || line.timestamp >= from) &&
            (to == 0 || line.timestamp <= to)) {
          // Used by sendWeeks and sendMonths to only return the right lines
          if (filter(line)) {
            // Send down the line for further processing
            stream.handleLine(line);
          }
        }
      }
    } else {
      Log.warningln("Statistics: Line version %d not supported.", version);
      file.readStringUntil('\n');
    }
  }

  Log.verboseln("Statistics: Done streaming, closing file");
  file.close();
}

bool Statistics::getFirstOfToday(StatisticsLine &targetLine) {
  Log.traceln("Statistics::getFirstOfToday");

  bool success{false};
  CallbackStream stream([&targetLine, &success](const StatisticsLine &line) -> void {
    targetLine = line;
    success = true;
  });

  time_t from = createTime(year(), month(), day());
  time_t to = now();

  sendDays(stream, from, to);

  return success;
}

bool Statistics::getFirstOfMonth(StatisticsLine &targetLine) {
  Log.traceln("Statistics::getFirstOfToday");

  bool success{false};
  CallbackStream stream([&targetLine, &success](const StatisticsLine &line) -> void {
    targetLine = line;
    success = true;
  });

  time_t from = createTime(year(), month(), 1);
  time_t to = now();

  sendMonths(stream, from, to);

  return success;
}

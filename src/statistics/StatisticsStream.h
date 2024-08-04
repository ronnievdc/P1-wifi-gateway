#pragma once
// std
#include <stdint.h>
#include <string>
// arduino
#include <ESP8266WebServer.h>
#include <WString.h>
// project
#include "./format.h"
#include "./statistics/Statistics.h"
#include "./statistics/StatisticsLine.h"

class StatisticsStream {
public:
  StatisticsStream() {}
  virtual void handleLine(const StatisticsLine &line) const = 0;
};

typedef std::function<void (const StatisticsLine &line)> LineCallback;

class CallbackStream : public StatisticsStream {
public:
  CallbackStream(LineCallback callback) : mCallback(callback) {}
  inline void handleLine(const StatisticsLine &line) const override {
    mCallback(line);
  };

private:
  LineCallback mCallback;
};

class StatisticsStreamServer : public StatisticsStream {
public:
  StatisticsStreamServer(ESP8266WebServer &server, const String &prop,
                         const String &period)
      : mServer(server), mProp(prop), mPeriod(period) {}

  void handleLine(const StatisticsLine &line) const override {
    uint32_t value2{0};
    if (mProp == String("E1")) {
      value2 = line.energy_delivered_tariff1;
    } else if (mProp == String("E2")) {
      value2 = line.energy_delivered_tariff2;
    } else if (mProp == String("R1")) {
      value2 = line.energy_returned_tariff1;
    } else if (mProp == String("R2")) {
      value2 = line.energy_returned_tariff2;
    } else if (mProp == String("TE")) {
      value2 = line.energy_delivered_tariff1 + line.energy_delivered_tariff2;
    } else if (mProp == String("TR")) {
      value2 = line.energy_returned_tariff1 + line.energy_returned_tariff2;
    } else if (mProp == String("G") || mProp == String("Gc")) {
      value2 = line.gas_delivered;
    }

    const std::string formattedString =
        fmt::format("[{},{:.3f}],\n", line.timestamp, (float)value2 / 1000);

    // Send the line to the WiFiClient
    mServer.sendContent(formattedString.c_str(), formattedString.length());
  }

private:
  ESP8266WebServer &mServer;
  String mProp;
  String mPeriod;
};

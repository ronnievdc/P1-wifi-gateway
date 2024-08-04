#pragma once
// std
#include <stdint.h>
#include <time.h>

struct StatisticsLine {
  uint8_t version{1};  // starting with 1
  time_t timestamp{0}; // unix timestamp as returned by the Time now() method
  uint32_t energy_delivered_tariff1{0}; // total value in Watt (1000 x P1-value)
  uint32_t energy_delivered_tariff2{0};
  uint32_t energy_returned_tariff1{0};
  uint32_t energy_returned_tariff2{0};
  uint32_t gas_delivered{0}; // total value in dm3 (1000 x P1-value)

  void loadFromFile(File &file);

  void saveToFile(File &file);

  void printLine() const;
};

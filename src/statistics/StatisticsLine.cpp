// std
#include <stdint.h>
#include <time.h>
// arduino
#include <FS.h>
// libraries
#include <ArduinoLog.h>
// project
#include "./statistics/StatisticsLine.h"


void StatisticsLine::loadFromFile(File &file) {
  // Compilers pad structs to bytes that are multiple of 2 or 4
  // to make the access to them easier to implement in machine code.
  // Therefore instead of casting the whole struct as bytes, cast the struct field by field.
  file.read(reinterpret_cast<uint8_t *>(&version), sizeof(uint8_t));
  file.read(reinterpret_cast<uint8_t *>(&timestamp), sizeof(time_t));
  file.read(reinterpret_cast<uint8_t *>(&energy_delivered_tariff1), sizeof(uint32_t));
  file.read(reinterpret_cast<uint8_t *>(&energy_delivered_tariff2), sizeof(uint32_t));
  file.read(reinterpret_cast<uint8_t *>(&energy_returned_tariff1), sizeof(uint32_t));
  file.read(reinterpret_cast<uint8_t *>(&energy_returned_tariff2), sizeof(uint32_t));
  file.read(reinterpret_cast<uint8_t *>(&gas_delivered), sizeof(uint32_t));
}

void StatisticsLine::saveToFile(File &file) {
  // Compilers pad structs to bytes that are multiple of 2 or 4
  // to make the access to them easier to implement in machine code.
  // Therefore instead of casting the whole struct as bytes, cast the struct field by field.
  Log.traceln("StatisticsLine::saveToFile");
  file.write(reinterpret_cast<uint8_t *>(&version), sizeof(uint8_t));
  file.write(reinterpret_cast<uint8_t *>(&timestamp), sizeof(time_t)); // uint64_t
  file.write(reinterpret_cast<uint8_t *>(&energy_delivered_tariff1), sizeof(uint32_t));
  file.write(reinterpret_cast<uint8_t *>(&energy_delivered_tariff2), sizeof(uint32_t));
  file.write(reinterpret_cast<uint8_t *>(&energy_returned_tariff1), sizeof(uint32_t));
  file.write(reinterpret_cast<uint8_t *>(&energy_returned_tariff2), sizeof(uint32_t));
  file.write(reinterpret_cast<uint8_t *>(&gas_delivered), sizeof(uint32_t));
}

void StatisticsLine::printLine() const {
  Log.verboseln("version: %d", version);
  Log.verboseln("timestamp: %d", timestamp);
  Log.verboseln("energy_delivered_tariff1: %d", energy_delivered_tariff1);
  Log.verboseln("energy_delivered_tariff2: %d", energy_delivered_tariff2);
  Log.verboseln("energy_returned_tariff1: %d", energy_returned_tariff1);
  Log.verboseln("energy_returned_tariff2: %d", energy_returned_tariff2);
  Log.verboseln("gas_delivered: %d", gas_delivered);
}

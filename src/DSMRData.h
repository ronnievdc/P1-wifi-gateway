#pragma once
// std
#include <stdint.h>
// arduino
#include <WString.h>

// Value that is parsed as a three-decimal float, but stored as an
// integer (by multiplying by 1000). Supports val() (or implicit cast to
// float) to get the original value, and int_val() to get the more
// efficient integer value. The unit() and int_unit() methods on
// FixedField return the corresponding units for these values.
struct FixedValue {
  FixedValue() {
    _value = 0;
  }
  explicit FixedValue(String value) {
    _value = value.toFloat() * 1000;
  }
  operator float() { return val(); }
  float val() const { return _value / 1000.0; }
  uint32_t int_val() const { return _value; }

  uint32_t _value{0};
};

// Vars to store meter readings
// we capture all data in char arrays or Strings for longer hard to delineate
// data
struct DSMRData {
  // String identification;
  String p1_version;                   // P1version
  String timestamp;                    // P1timestamp
  String equipment_id;                 // equipmentId
  FixedValue energy_delivered_tariff1; // electricityUsedTariff1
  FixedValue energy_delivered_tariff2; // electricityUsedTariff2
  FixedValue energy_returned_tariff1;  // electricityReturnedTariff1
  FixedValue energy_returned_tariff2;  // electricityReturnedTariff2
  String electricity_tariff;           // tariffIndicatorElectricity
  FixedValue power_delivered;          // actualElectricityPowerDeli
  FixedValue power_returned;           // actualElectricityPowerRet
  // FixedValue electricity_threshold;
  // uint8_t electricity_switch_position;
  uint32_t electricity_failures;      // numberPowerFailuresAny
  uint32_t electricity_long_failures; // numberLongPowerFailuresAny
  String electricity_failure_log;     // longPowerFailuresLog
  uint32_t electricity_sags_l1;       // numberVoltageSagsL1
  uint32_t electricity_sags_l2;       // numberVoltageSagsL2
  uint32_t electricity_sags_l3;       // numberVoltageSagsL3
  uint32_t electricity_swells_l1;     // numberVoltageSwellsL1
  uint32_t electricity_swells_l2;     // numberVoltageSwellsL2
  uint32_t electricity_swells_l3;     // numberVoltageSwellsL3
  // String message_short;
  // String message_long;
  FixedValue voltage_l1;         // instantaneousVoltageL1
  FixedValue voltage_l2;         // instantaneousVoltageL2
  FixedValue voltage_l3;         // instantaneousVoltageL3
  FixedValue current_l1;         // instantaneousCurrentL1
  FixedValue current_l2;         // instantaneousCurrentL2
  FixedValue current_l3;         // instantaneousCurrentL3
  FixedValue power_delivered_l1; // activePowerL1P
  FixedValue power_delivered_l2; // activePowerL2P
  FixedValue power_delivered_l3; // activePowerL3P
  FixedValue power_returned_l1;  // activePowerL1NP
  FixedValue power_returned_l2;  // activePowerL2NP
  FixedValue power_returned_l3;  // activePowerL3NP

  // uint16_t gas_device_type;
  String gas_equipment_id; // equipmentId2
  // uint8_t gas_valve_position;
  String gas_delivered; // gasReceived5min

  // uint16_t thermal_device_type;
  // String thermal_equipment_id;
  // uint8_t thermal_valve_position;
  // String thermal_delivered;

  // uint16_t water_device_type;
  // String water_equipment_id;
  // uint8_t water_valve_position;
  // String water_delivered;
  // uint16_t sub_device_type;
  // String sub_equipment_id;
  // uint8_t sub_valve_position;
  // String sub_delivered;

  uint8_t P1prot() { return p1_version[0] == '4' ? 4 : 5; }
};
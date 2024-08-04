#pragma once
// std
#include <string>
// libraries
#include <ArduinoLog.h>
// project
#include "format.h"
#include "SmartMeterSerialMock.h"

class DSMR5With3LinesMock : public SmartMeterSerialMock {
public:
  DSMR5With3LinesMock(uint8_t reqPin) : SmartMeterSerialMock(reqPin) {}

protected:
  std::string getDatagramBody() {
    Log.traceln("DSMR5With3LinesMock::getDatagramBody");
    return fmt::format("/ISK5\\2M550T-1012\r\n"
                       "\r\n"
                       "1-3:0.2.8(50)\r\n"         // p1_version
                       "0-0:1.0.0({}W)\r\n"        // timestamp YYMMDDHHMMSS
                       "0-0:96.1.1({})\r\n"        // equipment_id
                       "1-0:1.8.1({:.3f}*kWh)\r\n"   // energy_delivered_tariff1
                       "1-0:1.8.2({:.3f}*kWh)\r\n"   // energy_delivered_tariff2
                       "1-0:2.8.1({:.3f}*kWh)\r\n"   // energy_returned_tariff1
                       "1-0:2.8.2({:.3f}*kWh)\r\n"   // energy_returned_tariff2
                       "0-0:96.14.0(0002)\r\n"     // electricity_tariff
                       "1-0:1.7.0(00.172*kW)\r\n"  // power_delivered
                       "1-0:2.7.0(00.000*kW)\r\n"  // power_returned
                       "0-0:96.7.21(00304)\r\n"    // electricity_failures
                       "0-0:96.7.9(00003)\r\n"     // electricity_long_failures
                       "1-0:99.97.0()\r\n"         // electricity_failure_log
                       "1-0:32.32.0(00018)\r\n"    // electricity_sags_l1
                       "1-0:52.32.0(00017)\r\n"    // electricity_sags_l2
                       "1-0:72.32.0(00016)\r\n"    // electricity_sags_l3
                       "1-0:32.36.0(00001)\r\n"    // electricity_swells_l1
                       "1-0:52.36.0(00001)\r\n"    // electricity_swells_l2
                       "1-0:72.36.0(00001)\r\n"    // electricity_swells_l3
                       "0-0:96.13.0()\r\n"         // message_short
                       "1-0:32.7.0(236.5*V)\r\n"   // voltage_l1
                       "1-0:52.7.0(233.9*V)\r\n"   // voltage_l2
                       "1-0:72.7.0(235.1*V)\r\n"   // voltage_l3
                       "1-0:31.7.0(011*A)\r\n"     // current_l1
                       "1-0:51.7.0(001*A)\r\n"     // current_l2
                       "1-0:71.7.0(000*A)\r\n"     // current_l3
                       "1-0:21.7.0(00.000*kW)\r\n" // power_delivered_l1
                       "1-0:41.7.0(00.113*kW)\r\n" // power_delivered_l2
                       "1-0:61.7.0(00.016*kW)\r\n" // power_delivered_l3
                       "1-0:22.7.0(02.588*kW)\r\n" // power_returned_l1
                       "1-0:42.7.0(00.000*kW)\r\n" // power_returned_l2
                       "1-0:62.7.0(00.000*kW)\r\n" // power_returned_l3
                       "0-1:24.1.0(003)\r\n"       // gas_device_type
                       "0-1:96.1.0({})\r\n"        // gas_equipment_id
                       "0-1:24.2.1({}W)({:.3f}*m3)\r\n", // gas_delivered
                       getTimeStamp().c_str(), mEquipmentId,
                       getEnergyDeliveredHigh() / float(1000),
                       getEnergyDeliveredLow() / float(1000),
                       getEnergyReturnedHigh() / float(1000),
                       getEnergyReturnedLow() / float(1000), mGasEquipmentId,
                       getTimeStamp().c_str(), getGasDelivered() / float(1000));
  }
};

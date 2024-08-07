/*
 * Copyright (c) 2022 Ronald Leenes
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file loggingoder.ino
 * @author Ronald Leenes
 * @date 19.1.2023
 *
 *
 * @brief This file contains the logging functions for the graphs
 *
 * @see http://esp8266thingies.nl
 */

/*
 * files:
 *    HourE1.log    DayE1.log   WeekE1.log  MonthE1.log YearE1.log
 *    HourE2.log    DayE2.log   WeekE2.log  MonthE2.log YearE2.log
 *    HourR1.log    DayR1.log   WeekR1.log  MonthR1.log YearR1.log
 *    HourR2.log    DayR2.log   WeekR2.log  MonthR2.log YearR2.log
 *    HourG.log     DayG.log    WeekG.log   MonthG.log  YearG.log
 *    HourTE.log    DayTE.log   WeekTE.log  MonthTE.log YearTE.log
 *    Hour.TR.log   DayTR.log   WeekTR.log  MonthTR.log YearTR.log
 *
 *    Hour logs keep data of current 24 hours -> rotates out via hourX-1
 *    day
 *    week keeps track of daily readings for current week -> rotates out via
 * weekX-1 month keeps track of daily readings for current month -> rotates out
 * via monthX-1 year keeps track of daily readings for current year E D/W/ logs
 * > ['%s', %s],\n", (String)hour(), value E M logs >
 *
 *    DayG.log > [new Date(%s, %s, %s), %s],\n", year(), month(), day(), value);
 */

void doInitLogVars() {
  char value[12];
  // init all vars on current (first) reading
  Log.verbose("Initialising log vars ... ");
  strcpy(log_data.hourE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.dayE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.weekE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.monthE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.yearE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());

  strcpy(log_data.hourE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.dayE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.weekE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.monthE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.yearE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());

  strcpy(log_data.hourR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.dayR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.weekR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.monthR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.yearR1, String(dsmrData.energy_returned_tariff1.val()).c_str());

  strcpy(log_data.hourR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
  strcpy(log_data.dayR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
  strcpy(log_data.weekR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
  strcpy(log_data.monthR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
  strcpy(log_data.yearR2, String(dsmrData.energy_returned_tariff2.val()).c_str());

  dtostrf((atof(log_data.hourE1) + atof(log_data.hourE2)), 6, 2, value);
  strcpy(log_data.hourTE, value);
  strcpy(log_data.dayTE, value);
  strcpy(log_data.weekTE, value);
  strcpy(log_data.monthTE, value);
  strcpy(log_data.yearTE, value);

  dtostrf((atof(log_data.hourR1) + atof(log_data.hourR2)), 6, 2, value);
  strcpy(log_data.hourTR, value);
  strcpy(log_data.dayTR, value);
  strcpy(log_data.weekTR, value);
  strcpy(log_data.monthTR, value);
  strcpy(log_data.yearTR, value);

  needToInitLogVars = false;
  Log.verboseln("done.");
}

void doInitLogVarsGas() {
  Log.verboseln("Initialising log GAS vars ... ");

  strcpy(log_data.hourG, dsmrData.gas_delivered.c_str());
  strcpy(log_data.dayG, dsmrData.gas_delivered.c_str());
  strcpy(log_data.weekG, dsmrData.gas_delivered.c_str());
  strcpy(log_data.monthG, dsmrData.gas_delivered.c_str());
  strcpy(log_data.yearG, dsmrData.gas_delivered.c_str());
  needToInitLogVarsGas = false;
  Log.verboseln("done.");
}

void doMinutelyLog() {
  //  if (minFlag) return;
  FST.begin();
  char buffer[60];
  char value[13];
  dtostrf((dsmrData.gas_delivered.toFloat() - atof(minG)), 6, 2, value);
  sprintf(buffer, "['%s:%s:%s',%s],\n", (char*)hour(), (char*)minute(),
          (char*)second(), value);
  appendFile("/MinG.log", buffer);
  strcpy(minG, dsmrData.gas_delivered.c_str());
  FST.end();
  MountFS();
}
void doHourlyLog() {
  /*
   * cur - hour
   * append
   * hour = cur
   * set flag
   */
  // if (hourFlag) return;
  FST.begin();

  Log.verbose("Hourly log started at %s ... ", timestampkaal());
  char buffer[60];
  char value[13];

  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.hourE1)), 6, 2, value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourE1.log", buffer);
  strcpy(log_data.hourE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());

  dtostrf((dsmrData.energy_delivered_tariff2.val() - atof(log_data.hourE2)), 6, 2, value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourE2.log", buffer);
  strcpy(log_data.hourE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());

  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.hourR1)), 6, 2,
          value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourR1.log", buffer);
  strcpy(log_data.hourR1, String(dsmrData.energy_returned_tariff1.val()).c_str());

  dtostrf((dsmrData.energy_returned_tariff2.val() - atof(log_data.hourR2)), 6, 2,
          value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourR2.log", buffer);
  strcpy(log_data.hourR2, String(dsmrData.energy_returned_tariff2.val()).c_str());

  dtostrf((dsmrData.gas_delivered.toFloat() - atof(log_data.hourG)), 6, 2, value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourG.log", buffer);
  strcpy(log_data.hourG, dsmrData.gas_delivered.c_str());

  dtostrf((atof(log_data.hourE1) + atof(log_data.hourE2)), 6, 2, value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourTE.log", buffer);
  strcpy(log_data.hourTE, value);

  dtostrf((atof(log_data.hourR1) + atof(log_data.hourR2)), 6, 2, value);
  sprintf(buffer, "['%d',%s],\n", hour(), value);
  appendFile("/HourTR.log", buffer);
  strcpy(log_data.hourTR, value);

  // save state to file
  deleteFile("/logData1.txt");
  renameFile("/logData.txt", "/logData1.txt");
  File file = FST.open("/logData.txt", "w");
  file.write((byte *)&log_data, sizeof(log_data));
  file.close();

  FST.end();
  hourFlag = true;
  Log.verbose("completed at %s.", timestampkaal());
  Log.verboseln("");
  checkMinute = minute();
  if (MountFS()) {
    char payload[50];
    sprintf(payload, "Remounted FS at %s", string2char(timestampkaal()));
    if (MQTT_debug)
      send_mqtt_message("p1wifi/logging", payload);
  }
}

void doDailyLog() {
  /*
   * today = cur - day
   * append
   * week = week + today
   * month = month + today
   * year = year + today
   * day = cur
   * set flag
   */
  monitoring = false;
  Log.verbose("Dayly log started ... ");

  String str = "";
  char buffer[60];
  char value[12];

  if (month() < 10)
    str += "0";
  str += month();
  if ((day()) < 10)
    str += "0";
  str += day();
  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.dayE1)), 6, 2, value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayE1.log", buffer);
  appendFile("/WeekE1.log", buffer);
  appendFile("/MonthE1.log", buffer);
  appendFile("/YearE1.log", buffer);
  strcpy(log_data.dayE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());

  dtostrf((dsmrData.energy_delivered_tariff2.val() - atof(log_data.dayE2)), 6, 2, value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayE2.log", buffer);
  appendFile("/WeekE2.log", buffer);
  appendFile("/MonthE2.log", buffer);
  appendFile("/YearE2.log", buffer);
  strcpy(log_data.dayE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());

  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.dayR1)), 6, 2,
          value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("/DayR1.log", buffer);
  appendFile("/WeekR1.log", buffer);
  appendFile("/MonthR1.log", buffer);
  appendFile("/YearR1.log", buffer);
  strcpy(log_data.dayR1, String(dsmrData.energy_returned_tariff1.val()).c_str());

  dtostrf((dsmrData.energy_returned_tariff2.val() - atof(log_data.dayR2)), 6, 2,
          value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayR2.log", buffer);
  appendFile("/WeekR2.log", buffer);
  appendFile("/MonthR2.log", buffer);
  appendFile("/YearR2.log", buffer);
  strcpy(log_data.dayR2, String(dsmrData.energy_returned_tariff2.val()).c_str());

  dtostrf((dsmrData.gas_delivered.toFloat() - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayG.log", buffer);
  appendFile("/WeekG.log", buffer);
  appendFile("/MonthG.log", buffer);
  appendFile("/YearG.log", buffer);
  sprintf_P(buffer, PSTR("(%d,%d,%d), %s],\n"), year(), month() - 1, day(),
            value);
  // sprintf(buffer, "(%s , %s , %s), %s],\n", (char*)year(), (char*)0, (char*)day(), value); //
  // Google graph format uses month 0 for Jan!!
  appendFile("/YearGc.log", buffer);
  strcpy(log_data.dayG, dsmrData.gas_delivered.c_str());

  dtostrf((atof(log_data.dayE1) + atof(log_data.dayE2)), 6, 2, value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayTE.log", buffer);
  appendFile("/WeekTE.log", buffer);
  appendFile("/MonthTE.log", buffer);
  appendFile("/YearTE.log", buffer);
  strcpy(log_data.dayTE, value);

  dtostrf((atof(log_data.dayR1) + atof(log_data.dayR2)), 6, 2, value);
  sprintf(buffer, "['%s',%s],\n", string2char(str), value);
  //  appendFile("DayTR.log", buffer);
  appendFile("/WeekTR.log", buffer);
  appendFile("/MonthTR.log", buffer);
  appendFile("/YearTR.log", buffer);
  strcpy(log_data.dayTR, value);

  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");

  dayFlag = true;
  Log.verboseln("completed.");
  monitoring = true;
}

void doWeeklyLog() {
  /*
   *   append week
   *   week cur
   *   set flag
   */
  Log.verbose("Weekly log started. It was a Sunday evening ... ");
  String str = "";
  char buffer[55];
  char value[12];

  if (month() < 10)
    str += "0";
  str += month();
  if ((day()) < 10)
    str += "0";
  str += day();

  deleteFile("/WeekE1-1.log");
  renameFile("/WeekE1.log", "/WeekE1-1.log");
  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.weekE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksE1.log", buffer);
  strcpy(log_data.weekE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());

  deleteFile("/WeekE2-1.log");
  renameFile("/WeekE2.log", "/WeekE2-1.log");
  dtostrf((dsmrData.energy_delivered_tariff2.val() - atof(log_data.weekE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksE2.log", buffer);
  strcpy(log_data.weekE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());

  deleteFile("/WeekR1-1.log");
  renameFile("/WeekR1.log", "/WeekR1-1.log");
  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.weekR1)), 6, 2,
          value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksR1.log", buffer);
  strcpy(log_data.weekR1, String(dsmrData.energy_returned_tariff1.val()).c_str());

  deleteFile("/WeekR2-1.log");
  renameFile("/WeekR2.log", "/WeekR2-1.log");
  dtostrf((dsmrData.energy_returned_tariff2.val() - atof(log_data.weekR2)), 6, 2,
          value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksR2.log", buffer);
  strcpy(log_data.weekR2, String(dsmrData.energy_returned_tariff2.val()).c_str());

  deleteFile("/WeekTE-1.log");
  renameFile("/WeekTE.log", "/WeekTE-1.log");
  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.weekE1)) +
              (dsmrData.energy_delivered_tariff2.val() - atof(log_data.weekE2)),
          6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksTE.log", buffer);
  strcpy(log_data.weekTE, value);

  deleteFile("/WeekTR-1.log");
  renameFile("/WeekTR.log", "/WeekTR-1.log");
  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.weekR1)) +
              (dsmrData.energy_returned_tariff2.val() - atof(log_data.weekR2)),
          6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksTR.log", buffer);
  strcpy(log_data.weekTR, value);

  deleteFile("/WeekG-1.log");
  renameFile("/WeekG.log", "/WeekG-1.log");
  dtostrf((dsmrData.gas_delivered.toFloat() - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/WeeksG.log", buffer);
  strcpy(log_data.weekG, value);

  weekFlag = true;
  Log.verboseln("completed.");
}

void doMonthlyLog() {
  /*
   * append month
   * month = cur
   * set flag
   */

  Log.verbose("Monthlyly log started ... ");
  String str = "";
  char buffer[55];
  char value[12];

  if (month() < 10)
    str += "0";
  str += month();
  if ((day()) < 10)
    str += "0";
  str += day();

  deleteFile("/MonthE1-1.log");
  renameFile("/MonthE1.log", "/MonthE1-1.log");
  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.monthE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsE1.log", buffer);
  strcpy(log_data.monthE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());

  deleteFile("/MonthE2-1.log");
  renameFile("/MonthE2.log", "/MonthE2-1.log");
  dtostrf((dsmrData.energy_delivered_tariff2.val() - atof(log_data.monthE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsE2.log", buffer);
  strcpy(log_data.monthE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());

  deleteFile("/MonthR1-1.log");
  renameFile("/MonthR1.log", "/MonthR1-1.log");
  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.monthR1)), 6, 2,
          value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsR1.log", buffer);
  strcpy(log_data.monthR1, String(dsmrData.energy_returned_tariff1.val()).c_str());

  deleteFile("/MonthR2-1.log");
  renameFile("/MonthR2.log", "/MonthR2-1.log");
  dtostrf((dsmrData.energy_returned_tariff2.val() - atof(log_data.monthR2)), 6, 2,
          value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsR2.log", buffer);
  strcpy(log_data.monthR2, String(dsmrData.energy_returned_tariff2.val()).c_str());

  deleteFile("/MonthTE-1.log");
  renameFile("/MonthTE.log", "/MonthTE-1.log");
  dtostrf((dsmrData.energy_delivered_tariff1.val() - atof(log_data.monthE1)) +
              (dsmrData.energy_delivered_tariff2.val() - atof(log_data.monthE2)),
          6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsTE.log", buffer);
  strcpy(log_data.monthTE, value);

  deleteFile("/MonthTR-1.log");
  renameFile("/MonthTR.log", "/MonthTR-1.log");
  dtostrf((dsmrData.energy_returned_tariff1.val() - atof(log_data.monthR1)) +
              (dsmrData.energy_returned_tariff2.val() - atof(log_data.monthR2)),
          6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsTR.log", buffer);
  strcpy(log_data.monthTR, value);

  deleteFile("/MonthG-1.log");
  renameFile("/MonthG.log", "/MonthG-1.log");
  dtostrf((dsmrData.gas_delivered.toFloat() - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", string2char(str), value);
  appendFile("/MonthsG.log", buffer);
  strcpy(log_data.monthG, value);

  monthFlag = true;
  Log.verboseln("completed.");
}

void doYearlyLog() {
  /*
   * append year
   * year = cur
   * set flag
   */
}

void resetEnergyDaycount() {
#ifdef SWEDISH
  strcpy(log_data.dayE1, cumulativeActiveImport);
  strcpy(log_data.dayE2, cumulativeReactiveImport);
  strcpy(log_data.dayR1, cumulativeActiveExport);
  strcpy(log_data.dayR2, cumulativeReactiveExport);
#else
  strcpy(log_data.dayE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.dayE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.dayR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.dayR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
#endif
  coldbootE = false;
}

void resetEnergyMonthcount() {
#ifdef SWEDISH
  strcpy(log_data.monthE1, cumulativeActiveImport);
  strcpy(log_data.monthE2, cumulativeReactiveImport);
  strcpy(log_data.monthR1, cumulativeActiveExport);
  strcpy(log_data.monthR2, cumulativeReactiveExport);
#else
  strcpy(log_data.monthE1, String(dsmrData.energy_delivered_tariff1.val()).c_str());
  strcpy(log_data.monthE2, String(dsmrData.energy_delivered_tariff2.val()).c_str());
  strcpy(log_data.monthR1, String(dsmrData.energy_returned_tariff1.val()).c_str());
  strcpy(log_data.monthR2, String(dsmrData.energy_returned_tariff2.val()).c_str());
#endif
}

void resetGasCount() {
  strcpy(log_data.dayG, dsmrData.gas_delivered.c_str());
  coldbootG = false;
}

void DirListing() {
  String str;
  str += F("<html>\n<head>\n");
  str += F("<title>Slimme meter</title>");
  str += F("</head><body>\n");

  Dir root = FST.openDir("/");
  while (root.next()) {
    File file = root.openFile("r");
    str += ("  FILE: ");
    str += ("<a href='");
    str += (root.fileName());
    str += ("'>");
    str += (root.fileName());
    str += ("</a>");
    str += ("  SIZE: ");
    str += ((String)file.size());
    str += ("<br>");

    // time_t cr = file.getCreationTime();  
    // time_t lw = file.getLastWrite();
    file.close();
    // struct tm * tmstruct = localtime(&cr);
    // sprintf(buffer, "    CREATION: %d-%02d-%02d %02d:%02d:%02d\n",
    // (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday,
    // tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    // server.sendContent(buffer);
    //  tmstruct = localtime(&lw);
    // sprintf(buffer, "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
    // (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday,
    // tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    //   server.sendContent(buffer);
  }
  str += F("</body></html>\n");
  server.send(200, "text/html", str);
}

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

#include <utility> // for std::as_const

void doHourlyLog() {
  Log.verboseln("Hourly log started");
  Statistics statistics(FST);
  statistics.saveHour(dsmrData);
  Log.verboseln("Hourly log completed");
}

void doDailyLog() {
  Log.verboseln("Daily log started");
  Statistics statistics(FST);
  statistics.saveday(dsmrData);
  // today.log -> yesterday.log
  statistics.rotateDay();
  Log.verboseln("Daily log completed");
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

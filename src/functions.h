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
 * @file functions.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains useful functions
 *
 * @see http://esp8266thingies.nl
 */

void alignToTelegram() {
  // make sure we don't drop into the middle of a telegram on boot. Read
  // whatever is in the stream until we find the end char ! then read until EOL
  // and flsuh serial, return to loop to pick up the first complete telegram.

  int inByte = 0; // incoming serial byte
  char buf[10];
  if (DataSerial.available() > 0) {
    while (DataSerial.available()) {
      inByte = DataSerial.read();
      if (inByte == '!')
        break;
    }
    DataSerial.readBytesUntil('\n', buf, 9);
    DataSerial.flush();
  }
}

void RTS_on() {           // switch on Data Request
  digitalWrite(OE, LOW);  // enable buffer
  digitalWrite(DR, HIGH); // turn on Data Request
  OEstate = true;
  Log.verbose("Data request on at ");
  Log.verbose("%d\n", millis());
}

void RTS_off() {          // switch off Data Request
  digitalWrite(DR, LOW);  // turn off Data Request
  digitalWrite(OE, HIGH); // put buffer in Tristate mode
  OEstate = false;
  nextUpdateTime = millis() + interval;
  Log.verbose("Data request off at: ");
  Log.verbose("%d", millis());
  Log.verbose(" nextUpdateTime: ");
  Log.verbose("%d\n", nextUpdateTime);
}

int FindCharInArray(char array[], char c, int len) {
  for (int i = 0; i < len; i++) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

String timestampkaal() {
  return (String)hour() + ":" + minute() + ":" + second();
}

void timeIsSet_cb() {
  if (!timeIsSet) {
    timeIsSet = true;
    initTimers();
  }
}

void createToken() {
  char HexList[] = "1234567890ABCDEF";

  for (int i = 0; i < 16; i++) {
    setupToken[i] = HexList[random(0, 16)];
  }
  setupToken[16] = '\0';
  Log.verboseln("Token: %s", setupToken);
}

void listDir(const char *dirname) {
  Log.verbose("Listing directory: %s\n", dirname);

  Dir root = FST.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Log.verbose("  FILE: ");
    Log.verbose("%s", root.fileName());
    Log.verbose("  SIZE: ");
    Log.verbose("%d", file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm *tmstruct = localtime(&cr);
    Log.verbose("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n",
                (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Log.verbose("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
                (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                tmstruct->tm_sec);
  }
}

void readFile(const char *path) {
  Log.verbose("Reading file: %s\n", path);
  Log.verboseln("");
  File file = FST.open(path, "r");
  if (!file) {
    Log.verboseln("Failed to open file for reading");
    return;
  }

  Log.verboseln("Read from file: ");
  while (file.available()) {
    Log.verbose("%s", String((char)file.read()));
  }
  Log.verboseln("");
  file.close();
}

void writeFile(const char *path, const char *message) {
  Log.verbose("Writing file: %s\n", path);

  File file = FST.open(path, "w");
  if (!file) {
    Log.verboseln("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Log.verboseln("File written");
  } else {
    Log.verboseln("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char *path, const char *message) {

  Log.verboseln("Appending to file: %s time: %d", path, millis());
  char payload[50];

  File file = FST.open(path, "a");
  if (!file) {
    Log.verboseln("Failed to open file for appending");
    sprintf(payload, "can't open file %s", path);
    send_mqtt_message("p1wifi/logging", payload);
    return;
  }
  if (file.print(message)) {
    Log.verbose("Message appended: ");
    Log.verboseln(message);
    sprintf(payload, "Append to %s with %s succeeded: %s", path, message,
            string2char(timestampkaal()));
    send_mqtt_message("p1wifi/logging", payload);
  } else {
    Log.verboseln("Append failed");
    sprintf(payload, "Append to %s with %s failed: %s", path, message,
            string2char(timestampkaal()));
    send_mqtt_message("p1wifi/logging", payload);
  }
  file.flush();
  delay(3000);
  file.close();
}

void renameFile(const char *path1, const char *path2) {
  Log.verbose("Renaming file %s ", path1);
  Log.verbose("to %s\n", path2);
  if (FST.rename(path1, path2)) {
    Log.verboseln("File renamed");
  } else {
    Log.verboseln("Rename failed");
  }
}

void deleteFile(const char *path) {
  Log.verbose("Deleting file: %s\n", path);
  if (FST.remove(path)) {
    Log.verboseln("File deleted");
  } else {
    Log.verboseln("Delete failed");
  }
}

boolean MountFS() {
  Log.verboseln("Mount LittleFS");
  if (!FST.begin()) {
    Log.verboseln("FST mount failed");
    return false;
  }
  return true;
}

static void handleNotFound() {
  String path = server.uri();
  if (!FST.exists(path)) {
    server.send(404, "text/plain",
                "Path " + path + " not found. Please double-check the URL");
    return;
  }
  String contentType = "text/plain";
  if (path.endsWith(".css")) {
    contentType = "text/css";
  } else if (path.endsWith(".html")) {
    contentType = "text/html";
  } else if (path.endsWith(".js")) {
    contentType = "application/javascript";
  } else if (path.endsWith(".log")) {
    contentType = "text/plain";
  }
  File file = FST.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}

void zapFiles() {
  Log.verbose("Cleaning out logfiles ... ");

  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");

  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");

  deleteFile("/DayE1.log");
  deleteFile("/DayE2.log");
  deleteFile("/DayR1.log");
  deleteFile("/DayR2.log");
  deleteFile("/DayTE.log");
  deleteFile("/DayTR.log");
  deleteFile("/DayG.log");

  deleteFile("/WeekE1.log");
  deleteFile("/WeekE2.log");
  deleteFile("/WeekR1.log");
  deleteFile("/WeekR2.log");
  deleteFile("/WeekTE.log");
  deleteFile("/WeekTR.log");
  deleteFile("/WeekG.log");

  deleteFile("/MonthE1.log");
  deleteFile("/MonthE2.log");
  deleteFile("/MonthR1.log");
  deleteFile("/MonthR2.log");
  deleteFile("/MonthTE.log");
  deleteFile("/MonthTR.log");
  deleteFile("/MonthG.log");

  deleteFile("/YearE1.log");
  deleteFile("/YearE2.log");
  deleteFile("/YearR1.log");
  deleteFile("/YearR2.log");
  deleteFile("/YearTE.log");
  deleteFile("/YearTR.log");
  deleteFile("/YearG.log");
  deleteFile("/YearGc.log");

  deleteFile("/today.log");
  deleteFile("/yesterday.log");
  deleteFile("/2024.log");
  Log.verboseln("done.");
}

void zapConfig() {
  Log.verbose("Cleaning out logData files ... ");
  deleteFile("/logData.txt");
  deleteFile("/logData-1.txt");
  Log.verboseln("done.");
}

void formatFS() {
  char payload[50];
  sprintf(payload, "Formatting filesystem at %s", string2char(timestampkaal()));
  if (MQTT_debug)
    send_mqtt_message("p1wifi/logging", payload);

  FST.format();

  if (!FST.begin()) {
    Log.verboseln("FST mount failed AGAIN");
  } else {
    sprintf(payload, "Filesystem formatted at %s",
            string2char(timestampkaal()));
    if (MQTT_debug)
      send_mqtt_message("p1wifi/logging", payload);
  }
}

void identifyMeter(){
  if (datagram.indexOf("ISK5\\2M550E-1011") != -1) meterName = "ISKRA AM550e-1011";
  if (datagram.indexOf("KFM5KAIFA-METER") != -1) meterName = "Kaifa  (MA105 of MA304)";
  if (datagram.indexOf("XMX5LGBBFG10") != -1) meterName = "Landis + Gyr E350";
  if (datagram.indexOf("XMX5LG") != -1) meterName = "Landis + Gyr";
  if (datagram.indexOf("Ene5\\T210-D") != -1) meterName = "Sagemcom T210-D";
  if (datagram.indexOf("FLU5\\") !=-1) {
    meterName = "Siconia";
  }

  Log.verboseln(meterName.c_str());

  if (meterName != "") meternameSet = true;
}

void initTimers() {
  Log.traceln("initTimers");
#if DEBUG == 2
  for (int i = 0; i < 60; i += 60) {
    Log.verboseln("MyAlarm createHour XX:%d:00 local time", i);
    timeManager.createHour(i, 0, doHourlyLog);
  }
#else
  Log.verboseln("MyAlarm createHour XX:00:00 local time");
  timeManager.createHour(0, 0, doHourlyLog);
#endif
  Log.verboseln("MyAlarm createDay 00:00:00 local time");
  timeManager.createDay(0, 0, 0, doDailyLog);
}

void doWatchDogs() {
  char payload[60];
  if (ESP.getFreeHeap() < 2000) {
    Log.errorln("Rebooting because of memory leak. Free heap: %d", ESP.getFreeHeap());
    ESP.reset(); // watchdog, in case we still have a memery leak
  }
  if (millis() - LastSample > 300000) {
    DataSerial.flush();
    sprintf(payload, "No data in 300 sec, restarting monitoring: %s",
            string2char(timestampkaal()));
    if (MQTT_debug)
      send_mqtt_message("p1wifi/logging", payload);
    nextUpdateTime = millis() + 10000;
    OEstate = false;
    state = WAITING;
    monitoring = true;
  }
}

char *string2char(String str) {
  if (str.length() != 0) {
    char *p = const_cast<char *>(str.c_str());
    return p;
  }
  return (char *)"";
}

 /*
 * Copyright (c) 2023 Ronald Leenes
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
 * @file P1WG2022current.ino
 * @author Ronald Leenes
 * @date 22.09.2023
 * @version 1.1c 
 *
 * @brief This file contains the main file for the P1 wifi gatewway
 *
 * @see http://esp8266thingies.nl
 */
 
/*
 * P1 wifi gateway 2022
 * 
 * Deze software brengt verschillende componenten bij elkaar in een handzaam pakket.
 * 
 * De software draait op een ESP8285 of ESP8266 en heeft een bescheiden voetafdruk.
 * 
 * De data processing is gebaseerd op: http://domoticx.com/p1-poort-slimme-meter-data-naar-domoticz-esp8266/
 * De captive portal is gebaseerd op een van de ESP8266 voorbeelden, de OTA eenheid eveneens.
 * 
 * De module levert data aan een host via JSON of MQTT messages, maar kan ook via de webinterface van de module of via telnet worden uitgelezen
 * 
 * De module zal bij opstarten eerst de buffercondensator laden. 
 * Vervolgens knippert hij 2x en zal eerst proberen te koppelen aan je wifi netwerk. Gedurende dat proces zal de LED knipperen
 * met een periode van 0.5 Hz.
 * Wanneer geen verbinding kan worden gemaakt start de modiule een Access point onder de naam P1_Setup
 * Verbind met dit netwerk en surf naar 192.168.4.1
 * 
 * Het menuscherm van de module verschijnt dan. Vul daar SSID en wachtwoord van je wifi netwerk in. 
 * Daaronder vul je gegevens van de ontvangede server in (IP en poortnummer). Dit kan je DOmotociz server zijn
 * In dat geval moet je ook het IDx dat Domoticz maakt voor een Gas device en een Energy device invoeren.
 * Tenslotte geef je aan per hoeveel seconde je data naar de server wilt laten sturen.
 * DSMR4 meters hanteren een meetinterval van 10 sec. SMR5 meters leveren iedere seconde data. Dat lijkt me wat veel en 
 * kost de module ook veel stroom waardoor het maar de vraag is of hij parasitair (door de meter gevoed) kan werken.
 * Ik raad een interval van 15 sec of hoger aan. Het interval dat je hier invoert zal niet altijd synchroon lopen met
 * het data interval van de meter, ga er dus niet van uit dat je exact iedere x seconden een meetwaarde ziet in DOmoticz. 
 * Tussen metingen 'slaapt' de module (de modem wordt afgeschakeld waardoor het stroomverbruik van zo'n 70mA terugvalt naar 15 mA). 
 * Dit geeft de bufferelco tijd omm op te laden voor de stroompiekjes die de wifi zender van de module produceert 
 * (en het bespaart hoe dan ook wat stroom (die weliswaar door je energieleverancier wordt betaald, maar toch). Alle kleine 
 * beetjes helpen..
 *
 *  informatie, vragen, suggesties ed richten aan romix@macuser.nl 
 *  
 *  
 *    
 *  versie: 1.1c 
 *  datum:  22 Sep 2023
 *  auteur: Ronald Leenes
 *  
 *  1.1c: clean-up. Telnet can now accept 10 sessions at the same time. Restart when max is reached.
 *  1.1bf: kludge to fix empty ret value. Somehow the value is overwritten by something. Relocating defs for actualElectricityPowerDeli and Ret seems to fix it ofr now
 *  1.1bea: extended field length for actualElectricityPowerDeli[12] and actualElectricityPowerRet[12];
 *  1.1be: added support for ISKRA /ISK5\2M550E-1011 (which terminates 1-0.2.7.0 and 1-0:22.7.0 without *, using kW instead) 
 *  1.1bd: added 3 phase consumption in webdashboard
 *          fixe 3 phase voltage in webdashboard
 *  1.1b    cleaning up, bug fixes, cosmetic changes
 *  1.1adc
 *  1.1ad: bug fixes and graph improvements
 *  1.1aa: bug fixes
 *  1.1: implemented graphs
 *  
 *  
 *  ud: fixed refreshheader
 *  ua: fixed setup field issue
 *  u: 
 *    password on Setup and Firmware Update
 *    made mqtt re-connect non-blocking
 *    incorporated "DSMR Reader" mqtt topics 
 *    fixed hardboot daycounters reset
 *    fixed sending empty MQTT messages
 *  
 *  t: improvements on powermanagement, overall (minor) cleanup
 *  ta: fix for Telnet reporting
 *  
 *  s: added German localisation
 *        Added mqtt output for Swedish specific OBIS codes
 *        
 *  r: speed improvements and streamlining of the parser
 *      localisations for: NL, SE
 *      
 *  q: added daily values
 *  p: incorporated equipmentID in mqtt set
 *  o: fixed gas output, fixed mqtt reconnect
 *  n: empty call to SetupSave now redirects to main menu instead of resetting settings ;-)
 *      fixed kWh/Wh inversion
 *  m: setupsave fix, relocate to p1wifi.local na 60 sec 
 *      mqtt - kw/W fix
 *  l: wifireconnect after wifi loss
 *  k: fixed big BUG, softAP would not produce accessible webserver.
 *  j: raw data on port 23
 *      major code rewrite
 *      implemented data reporting methods: 
 *        parsed data: json, mqtt, p1wifi.local/P1
 *        raw data: telnet on port 23, p1wifi.local/Data
 *        
 *  i:  extended max line length for TD210-D meters, which have a really loong 1-0:99.97.0 line
 *  h:  extended mqtt set with instant powers, voltages, actual tarif group. power outages, power drops
 *  g: fixed mqtt
 *  
 *  Generic ESP8285 module 
*   Flash Size: 2mb (FS: 128Kb, OTA: –960Kb) 
*   
*   needed files: 
*   this one (obv), 
*   vars.h            in the process of moving all vars here
*   lang.h
*   CRC16.h
*   JSON.ino
*   MQTT.ino
*   TELNET.ino
*   debug.ino
*   decoder.ino
*   functions.ino
*   graph.ino
*   webserver.ino
*   webserverDE.ino 
*   webserverSE.ino
*   webserverNL.ino
*   wifi.ino
*   
*/

// to do: implement reboot when 5 mins in setup-window


bool zapfiles = false; 

String version = "1.1c – NL";
#define   NEDERLANDS //  SWEDISH //      GERMAN//   FRENCH // 

#define HOSTNAME "p1meter"
#define FSystem 1 // 0= LittleFS 1 = SPIFFS  

#define GRAPH 1
#define V3
#define DEBUG 0//1//0//3//2//1 is on serial only, 2 is serial + telnet, 
#define DEBUG2 0
#define DEBUGT 1

#define ESMR5 1
//#define SLEEP_ENABLED 


const uint32_t  wakeTime = 90000; // stay awake wakeTime millisecs 
const uint32_t  sleepTime = 15000; //sleep sleepTime millisecs

#if DEBUG2 == 1
#define debug2(x) Serial.print(x)
#define debug2ln(x) Serial.println(x)
#else
#define debug2(x)
#define debug2ln(x)
#endif

#if DEBUG == 1
const char* host = "P1test";
#define BLED LED_BUILTIN
#define debug(x) Serial.print(x)
#define debugf(x) Serial.printf(x)
#define debugff(x,y) Serial.printf(x,y)
#define debugfff(x,y,z) Serial.printf(x,y,z)
#define debugln(x) Serial.println(x)
#elif DEBUG == 2
#define BLED LED_BUILTIN
#define debug(x) Serial.print(x);if(telnetConnected)telnet.print(x)
#define debugf(x) Serial.printf(x)
#define debugff(x,y) Serial.printf(x,y)
#define debugfff(x,y,z) Serial.printf(x,y,z)
#define debugln(x) Serial.println(x);if(telnetConnected)telnet.println(x)
#elif DEBUG == 3
const char* host = "P1home";
#define BLED LED_BUILTIN
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugff(x,y)
#define debugfff(x,y,z)
#else
const char* host = "P1wifi";
#define BLED LED_BUILTIN
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugff(x,y)
#define debugfff(x,y,z)
#endif

#define errorHalt(msg) {debugln(F(msg)); return;}

#define NUM(off, mult) ((P1timestamp[(off)] - '0') * (mult) )   // macro for getting time out of timestamp, see decoder

#define ToggleLED  digitalWrite(BLED, !digitalRead(BLED));
#define LEDoff  digitalWrite(BLED, HIGH);
#define LEDon   digitalWrite(BLED, LOW);
#define OE  16 //IO16 OE on the 74AHCT1G125 
#define DR  4  //IO4 is Data Request

#include "vars.h"
#include "lang.h"

#include <TZ.h>
#define MYTZ TZ_Europe_Amsterdam
#include <TimeLib.h>
#include <coredecls.h> // settimeofday_cb()
#include <MyAlarm.h>


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "CRC16.h"

#if GRAPH == 1
//#include "lfs.h"
#if FSystem == 0
#include <LittleFS.h>
#define FST LittleFS
#elif FSystem == 1
#include "FS.h" //SPIFFS
#define FST SPIFFS
#endif
File file;
#endif


// van ESP8266WiFi/examples/WiFiShutdown/WiFiShutdown.ino
#ifndef RTC_config_data_SLOT_WIFI_STATE
#define RTC_config_data_SLOT_WIFI_STATE 33u
#endif
#include <include/WiFiState.h>  // WiFiState structure details
WiFiState WiFistate;



//van captive portal
/*  ============================================================================================================

    Captive Portal based on:
    https://gitlab.com/MrDIYca/code-samples/-/raw/master/esp8266_setup_portal.ino

    For more info, please watch my video at https://youtu.be/1VW-z4ibMXI
    MrDIY.ca
  ============================================================================================================== */
#include <EEPROM.h>
#include <ESP8266WebServer.h>
ESP8266WebServer    server(80);

#include <WiFiClient.h>
#include <ESP8266mDNS.h>

// mqtt stuff . https://github.com/ict-one-nl/P1-Meter-ESP8266-MQTT/blob/master/P1Meter/P1Meter.ino
#include <PubSubClient.h>
WiFiClient espClient;                   // * Initiate WIFI client
PubSubClient mqtt_client(espClient);    // * Initiate MQTT client
bool MQTT_Server_Fail = false;
long nextMQTTreconnectAttempt; 
// end mqtt stuff

//// Raw data server
WiFiServer telnet(port);
WiFiClient telnetClients[MAX_SRV_CLIENTS];

ADC_MODE(ADC_VCC);  // allows you to monitor the internal VCC level;
  



void setup() {
  WiFi.forceSleepBegin(sleepTime * 1000000L); //In uS. Must be same length as your delay
  delay(10); // it doesn't always go to sleep unless you delay(10); yield() wasn't reliable
  delay(sleepTime); //Hang out at 15mA for (sleeptime) seconds
  WiFi.forceSleepWake(); // Wifi on

  pinMode(BLED, OUTPUT);
  Serial.begin(115200);
/* swap serial ports to gpio15(tx) and gpio13(rx)
  delay(500);
  Serial.swap();
  delay(500);
end swap  */ 
  debugln("Serial.begin(115200);");
    
  pinMode(OE, OUTPUT);      //IO16 OE on the 74AHCT1G125
  digitalWrite(OE, HIGH);   //  Put(Keep) OE in Tristate mode
  pinMode(DR, OUTPUT);      //IO4 Data Request
  digitalWrite(DR, LOW);    //  DR low (only goes high when we want to receive data)

  blink(2);
    debugln("Booting");
    debugln("Done with Cap charging … ");
    debugln("Let's go …");
    
  EEPROM.begin(sizeof(struct settings) );
  EEPROM.get( 0, config_data );
  
  if (config_data.dataSet[0] !='j') {
      config_data = (settings) {"n", "orbi","Moesmate","192.168.1.12","8080","1234","1235","sensors/power/p1meter","192.168.1.12","1883", "mqtt_user", "mqtt_passwd", "30", "n","n","n","n","n","adminpwd"};
  }
 
  (config_data.watt[0] =='j') ? reportInDecimals = false : reportInDecimals = true;
  (config_data.domo[0] =='j') ? Json = true : Json = false;
  (config_data.mqtt[0] =='j') ? Mqtt = true : Mqtt = false;
  (config_data.telnet[0] =='j') ? Telnet = true : Telnet = false;
  (config_data.debug[0] =='j') ? MQTT_debug = true : MQTT_debug = false;
  if (strcmp(config_data.mqttTopic, "dsmr") == 0) { // auto detext need to report in 'dsmr reader' mqtt format
    mqtt_dsmr = true;
   // reportInDecimals = true;
  } else {
    mqtt_dsmr = false; 
   // reportInDecimals = false;
  }
   
  debugln("EEprom read: done");
  PrintConfigData();  

  interval = atoi( config_data.interval) * 1000; 
  debug("interval: ");
  debugln(interval);
  
  debugln("Trying to connect to your wifi network:");
  WiFi.mode(WIFI_STA);
  WiFi.begin(config_data.ssid, config_data.password);    
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
      ToggleLED
        delay(300);
        debug("o");
      if (tries++ > 30) {
        debugln("");
        debugln("Setting up Captive Portal by the name 'P1_setup'");
        LEDon
        WiFi.mode(WIFI_AP);
        softAp = WiFi.softAP("P1_Setup", "");
        APtimer = millis();
        breaking = true;
        break;
      }
     }    
      debugln("");
      debugln("Set up wifi, either in STA or AP mode");
    if (softAp){
        debugln("running in AP mode, all handles will be initiated");
      start_webservices();
    }

  if (WiFi.status() == WL_CONNECTED){ 
     WiFi.setAutoReconnect(true);
       debugln("HTTP server running.");
      debug("IP address: ");
       debugln(WiFi.localIP());
      setRFPower();
      wifiSta = true;
        debugln("wifi running in Sta (normal) mode");
      LEDoff
      ESP.rtcUserMemoryWrite(RTC_config_data_SLOT_WIFI_STATE, reinterpret_cast<uint32_t *>(&WiFistate), sizeof(WiFistate));
#ifdef SLEEP_ENABLED
      modemSleep();
      modemWake();
#else
      start_services();
#endif
     
      
 
 // handle Files
 debug("Mounting file system ... ");
      if (!FST.begin()) {
          debugln("FST mount failed");
          debug("Formatting file system ... "); 
          FST.format();
          if (!FST.begin()) {
            debugln("FST mount failed AGAIN");
          } else 
          { 
            debugln("done.");
            if (zapfiles) zapFiles();
          }
      } else debugln("done.");

     debugln("Reading logdata:");
      File logData = FST.open("/logData.txt", "r");
        if (logData) {
         logData.read((byte *)&log_data, sizeof(log_data)/sizeof(byte));
        logData.close();
        } else {
          debugln("Failed to open logData.txt for reading");
          needToInitLogVars = true;
          needToInitLogVarsGas = true;
        }
        if (zapfiles) zapFiles();
      
  } // WL connected
 //debugln("Something is terribly wrong, no network connection");
  timerAlarm.stopService();
  settimeofday_cb(timeIsSet_cb);
  configTime(MYTZ, "pool.ntp.org");

 debugln("All systems are go...");
  alignToTelegram();
  state = WAITING;    // signal that we are waiting for a valid start char (aka /)
  devicestate = CONFIG;
  nextUpdateTime = nextMQTTreconnectAttempt = EverSoOften = TenSecBeat = millis();
   
  monitoring = true; // start monitoring data
  time_to_sleep = millis() + wakeTime;  // we go to sleep wakeTime seconds from now  
  datagram.reserve(1500);
}

void readTelegram() {
  if (Serial.available()) {
    memset(telegram, 0, sizeof(telegram));
    while (Serial.available()) {
      int len = Serial.readBytesUntil('\n', telegram, MAXLINELENGTH);
      telegram[len] = '\n';
      telegram[len+1] = 0;
      yield();
      ToggleLED      
      if(decodeTelegram(len+1)){     // test for valid CRC
        debug2ln(datagram);
          break;
      } else { // we don't have valid data, but still may need to report to Domo
        if (dataEnd && !CRCcheckEnabled) { //this is used for dsmr 2.2 meters
         // yield(); //state = WAITING; 
        }
      }
    }
    LEDoff
  }
}

void loop() {  
  if ((millis() > nextUpdateTime)){ // && monitoring){
    if (!OEstate) { // OE is disabled  == false
       RTS_on();
       Serial.flush();
    }
  } // update window  
  if (OEstate) readTelegram();

#ifdef SLEEP_ENABLED
  if ((millis() > time_to_sleep) && !atsleep && wifiSta){  // not currently sleeping and sleeptime
    modemSleep();
  }
  if (wifiSta && (millis() > time_to_wake) && (WiFi.status() != WL_CONNECTED)) { // time to wake, if we're not already awake
     modemWake();
  }
#endif
  
  if (datagramValid && (state == DONE) && (WiFi.status() == WL_CONNECTED)){ 
      if (Mqtt) {
        doMQTT();
        if (MqttDelivered) {
           MqttDelivered = false;  // reset
        }
      }
      if (Json)       doJSON();
      if (Telnet)     TelnetReporter();
      if (MQTT_debug) MQTT_Debug();
      
      datagramValid = false;  // reset
      state = WAITING;
     }


  if (softAp || (WiFi.status() == WL_CONNECTED)) {
        server.handleClient(); //handle web requests
        MDNS.update();
  //      if (Telnet) telnetloop();// telnet.loop();
  }

if (millis() > TenSecBeat){
  if (Telnet && wifiSta) telnetloop();
  TenSecBeat = millis() + 10000;
}

if (millis() > EverSoOften){
    checkCounters();
    resetFlags();
    doWatchDogs();
    EverSoOften = millis() + 22000;
    
  }
  timerAlarm.update();
}   

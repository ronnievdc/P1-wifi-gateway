#include <WString.h>

/*
 * vars.h
 */
struct settings {
  char dataSet[4];
  char ssid[32];
  char password[64];
  char domoticzIP[30] = "IPaddress";
  char domoticzPort[10] = "Port";
  char domoticzEnergyIdx[10] = "EnergyIdx";
  char domoticzGasIdx[10] = "GasIdx";
  char mqttTopic[50] = "sensors/power/p1meter";
  char mqttIP[30] = "";
  char mqttPort[10] = "";
  char mqttUser[32] = "";
  char mqttPass[64] = "";
  char interval[3] = "20";
  char domo[4] = "j";
  char mqtt[4] = "n";
  char watt[4] = "n";
  char telnet[4] = "n";
  char debug[4] = "n";
  char adminPassword[32];
} config_data = {};

// admin related
bool bootSetup = false; // flag for adminpassword bypass
bool AdminAuthenticated = false;
char setupToken[18];

String prevGAS; // not an P1 protocol var, but holds gas value

// logging vars
bool timeIsSet = false;
bool TimeTriggersSet = false;

unsigned long EverSoOften = 0;
unsigned long TenSecBeat = 0;
unsigned long FiveSecBeat = 0;
unsigned long SecBeat = 0;

// energy management vars
int interval;
unsigned long nextUpdateTime = 0;
unsigned long LastReportinMillis = 0;
unsigned long LastSample = 0;
time_t LastReportinSecs = 0;

bool monitoring = false; // are we permitted to collect P1 data? Not if in setup
                         // or firmware update

// datagram stuff
#define MAXLINELENGTH                                                          \
  2048 // 0-0:96.13.0 has a maximum lenght of 1024 chars + 11 of its identifier
char telegram[MAXLINELENGTH]; // holds a single line of the datagram
String datagram;              // holds entire datagram for raw output
String meterId = "";
String meterName = "";
bool meternameSet = false; // do we have a metername already?

bool datagramValid = false; //
int dataFailureCount = 0; //counter for CRC failures
bool dataEnd = false; // signals that we have found the end char in the data (!)
unsigned int currentCRC = 0; // the CRC v alue of the datagram
bool reportInDecimals = true;
String mtopic = "Een lange MQTT boodschap als placeholder";

// process stuff
bool OEstate =
    false; // 74125 OE output enable is off by default (EO signal high)

#define DISABLED 0
#define WAITING 1
#define READING 2
#define CHECKSUM 3
#define DONE 4
#define FAILURE 5
#define FAULT 6
int state = DISABLED;

// JSON
bool Json = false;
String LastJReport = ""; // timestamp of last telegram reported via JSON

// Telnet
bool Telnet = false;
int currentTelnetSession;
#define MAX_SRV_CLIENTS 10
#define STACK_PROTECTOR 1512 // bytes
bool telnetConnected = false;
bool telnetDebugConnected = false;
String LastTReport = ""; // timestamp of last telegram reported on Telnet
const int port = 23;
int activetelnets = 0;
int telnetDebugClient = 0;

// MQTT
bool Mqtt = false;
bool MqttConnected = false;
bool MqttDelivered = false;
String LastMReport = ""; // timestamp of last telegram reported on MQTT
bool mqtt_dsmr = false;  // deliver mqtt data in 'dsmr reader" format
bool MQTT_debug = false;
bool MQTT_Server_Fail = false;
long nextMQTTreconnectAttempt;

// debug related
float volts;
String statusMsg;
uint32_t FlashSize;

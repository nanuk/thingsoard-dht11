// This sketch demonstrates connecting and sending telemetry
// using ThingsBoard SDK and GSM modem, such as SIM808
//
// Hardware:
//  - Arduino Uno
//  - SIM808 Arduino shield connected to Arduino Uno

// Select your modem:
//#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
 #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
//#include <SparkFunHTU21D.h>
#include <DHT.h>
#include "ThingsBoard.h"


const bool eco = true ;      
// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "wap.tmovil.cl";
const char user[] = "wap";
const char pass[] = "wap";

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "pMBHiJU3IPZvVFwugfY9"
#define THINGSBOARD_SERVER  "165.22.154.109"
#define THINGSBOARD_PORT    8080

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   9600

// Serial port for GSM shield
SoftwareSerial serialGsm(10, 11); // RX, TX pins for communicating with modem

//DHT11
#define DHTPIN 2 //pin
#define DHTTYPE DHT11 //tipo sensor
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

// Uncomment to see StreamDebugger output in serial monitor
//#define DUMP_AT_COMMANDS 1

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(serialGsm, Serial);
  TinyGsm modem(debugger);
#else
  // Initialize GSM modem
  TinyGsm modem(serialGsm);
#endif

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoardHttp tb(client, TOKEN, THINGSBOARD_SERVER, THINGSBOARD_PORT);

// Set to true, if modem is connected
bool modemConnected = false;

//HTU21D sensor;

void setup() {

  //encendido SIM900
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(5000);
  
  // Set console baud rate

  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println("-------SETUP------");

  // Set GSM module baud rate
  serialGsm.begin(9600);
  delay(3000);

  // Lower baud rate of the modem.
  // This is highly practical for Uno board, since SoftwareSerial there
  // works too slow to receive a modem data.

  serialGsm.write("AT+IPR=9600\r\n");
  serialGsm.end();
  serialGsm.begin(9600);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);

  //sensor.begin();
  // Inicializamos comunicación serie
  Serial.begin(9600);
  // Comenzamos el sensor DHT
  dht.begin();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
 
  
}

void loop() {
  delay(60000);

  if (!modemConnected) {
    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" OK");

    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, user, pass)) {
        Serial.println(" fail");
        delay(10000);
        return;
    }

    modemConnected = true;
    Serial.println(" OK");
  }

  // Uploads new telemetry to ThingsBoard using HTTP.
  // See https://thingsboard.io/docs/reference/http-api/#telemetry-upload-api
  // for more details

  Serial.println("Sending temperature data...");
  //tb.sendTelemetryFloat("temperature", sensor.readTemperature());
  if (!tb.sendTelemetryFloat("temperature", dht.readTemperature())){
        Serial.println(" fail send data, reconect");
        modemConnected = false;
    }
  //tb.sendTelemetryFloat("temperature", dht.readTemperature());
  Serial.println("Sending humidity data...");
  //tb.sendTelemetryFloat("humidity", sensor.readHumidity());
  if (!tb.sendTelemetryFloat("humidity", dht.readHumidity())){
        Serial.println(" fail send data, reconect");
        modemConnected = false;
    }
  //tb.sendTelemetryFloat("humidity", dht.readHumidity());

 
}

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include "Adafruit_SHT31.h"

//const char* ssid = "TP-Link_4346";
//const char* password = "84850034";

const char* ssid = "inHouseTest1";
const char* password = "nasturtium";
 
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// The pins which will be connected to the 2 relay
int humidPin = 2;
int tempPin = 14;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 0;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// the temperature (Cecius) and humidity (%) threshholds to trigger relay
//float tempLimit = 22.78;
float tempLimit = 30.0;
float humidLimit = 97.0;
 
void setup() {
  Serial.begin(115200);

  // Enables DS18B20
  sensors.begin();

  // Sets the digital pins as output
  pinMode(humidPin, OUTPUT);
  pinMode(tempPin, OUTPUT);

  OTAinit();
}

// Initializes OTA functionality for the 8266
void OTAinit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Outputs measured temperature and humidity to serial monitor
void printShtMeasurments(float temp, float hum) {
   if (! isnan(temp)) {
    Serial.print("Temp *C = "); Serial.println(temp);
  } else { 
    Serial.println("Failed to read temperature");
  }
 
  if (! isnan(hum)) {
    Serial.print("Hum. % = "); Serial.println(hum);
  } else { 
    Serial.println("Failed to read humidity");
  }
}

// Sets relay pins to high or low depending on measurements
void relayPower(int pin,float current, float limit) {
  if(current < limit) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }
}


void shtAlgorithm() {
  // Reads temperature and humidity from sht31 and stores it to appropriate variables
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  
  // Displays current readings
  printShtMeasurments(t, h);
  
  // Sets relay relevent pins to appropriate voltage levels
  relayPower(humidPin, h, humidLimit);
  relayPower(tempPin, t, tempLimit);
}

void DS18B20Algorithm() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);

  relayPower(tempPin, temperatureC, tempLimit);
  
  Serial.print(temperatureC);
  Serial.println("ºC");
}
 
void loop() {
  if(sht31.begin(0x44)) {
    shtAlgorithm();
  }else {
    DS18B20Algorithm();
  }

  // Checks for FOTA update
  ArduinoOTA.handle();
  
  Serial.println();
  
  delay(1000);
}

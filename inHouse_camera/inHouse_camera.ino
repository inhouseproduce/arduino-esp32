#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_NO_PSRAM

#include "esp_camera.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//
#include "Arduino.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
//#include "dl_lib.h"
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#define EEPROM_SIZE 1
int pictureNumber = 0;

#include <iostream>
//#include <math.h>
#include <vector>
//#include <base64.h>
#include <string>
#include "base64.hpp"

//

#include <HTTPClient.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        // Time ESP32 will go to sleep (in seconds) */

String macA;

// esp32 number denoting position
const int number = 1;

// The length of the complete cycle.
const int cycleLength = 60;
// How long the camera remains on in each cycle
const int intervalOn = 30;

// How long the camera sleeps for each cycle
int intervalOff = cycleLength - intervalOn;
// how long after the start of the next cycle it will take to turn on given id Number
int stagger = (number - 1) * intervalOn;
// When the next cycle starts
int start;
// Time device turned on epoch time
int awoke;
// When the 32 will turn on next
int sleepAlarm;

const char* ssid = "inHouseTest1";
const char* password = "nasturtium";

const String postAddress = "http://10.39.242.79:3000/camera/";
//const String postAddress = "/camera/";

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP);

int currentTime = 0; // What the current time is
int wokeUpAt = 0; // When the 32 last woke up

void startCameraServer();

IPAddress ip;

struct photo {
  uint8_t* buff;
  String mac;
  photo(uint8_t* b, String m) {
    buff = b;
    mac = m;
  }
};

void OTA() {
  ArduinoOTA.handle();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connection strong");
  } else {
    Serial.println("connection lost");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(WiFi.status());
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(ssid, password);
      delay(2000);
    }
    Serial.println("reconnected!");
  }
  if (ip != WiFi.localIP()){
    ip = WiFi.localIP();
    Serial.println("new ip address");
    Serial.print(ip);
  }
  delay(500);
}

void postProtocol() {
  Serial.println("Sending");
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin(postAddress);      //Specify request destination
   http.addHeader("Content-Type", "application/json");  //Specify content-type header

   //Serial.println(macA + " " +WiFi.localIP());

   int httpCode = http.POST("{\"id\": \"" + macA + "\", \"address\": \"" + WiFi.localIP() + "\"}");   //Send the request
   //int httpCode = http.POST("{\"address\": \"" + ip +"\"}");   //Send the request
   String payload = http.getString();                  //Get the response payload

   //Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 }else{
    Serial.println("Error in WiFi connection"); 
 }
}

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();

  //pinMode(sleepPin, INPUT);
  //esp_sleep_enable_ext0_wakeup(sleepGpioPin, 1);
  Serial.println("Morning!");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //config.pixel_format = PIXFORMAT_RAW;
  //init with high specs to pre-allocate larger buffers
  
  Serial.printf("psramFound(): ",psramFound());
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    Serial.printf("framesize UXGA");
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    Serial.printf("framesize SVGA");
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_XGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  macA = WiFi.macAddress();

  ///////////////////////////////////////////////
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  timeClient.begin();
  timeClient.update();
  awoke = timeClient.getEpochTime();
  // Gets start of next cycle
  if(awoke % cycleLength != 0) {
    // start of next cycle equals time awoken + length of cycle minus time elapsed since start of last cycle
    start = awoke + cycleLength - (awoke % cycleLength);
  } else {
    // In case the system woke up exactly on it's previous starting time (Should only be possible for camera 1)
    start = awoke + cycleLength;
  }
  sleepAlarm = start + stagger;

  capture();
}

void loop() {
  OTA();
  timeClient.update();
  currentTime = timeClient.getEpochTime();
  //Serial.println(currentTime);
//  if(currentTime > awoke + intervalOn) {
//    Serial.println("Good Night!");
//    esp_sleep_enable_timer_wakeup((sleepAlarm - currentTime) * uS_TO_S_FACTOR);
//    esp_deep_sleep_start();
//  }
  //postProtocol();
  //Serial.println(number);
  delay(500);
}


// Code used to capture the immage will throw an error
// if no sd card is detected. If sd card is present but
// errors persist reformat the card. If images are
// unnesesary, comment out relative code.
void capture() {
  //Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
    
  camera_fb_t * fb = NULL;
  
  // Take Picture with Camera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  // Path where new picture will be saved in SD Card
  String path = "/picture" + String(pictureNumber) +".jpg";

  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }

  file.close();
  
  postPic(fb);
  
  esp_camera_fb_return(fb); 
  
  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
  
  delay(2000);
  Serial.println("Going to sleep now");
  delay(2000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

// Code to perform http post on the immage.
// Currently attempting to post as both
// a String and Byte array.
void postPic(camera_fb_t * fb) {
  //Serial.println(fb->len);
  //Serial.println(encode64(fb->buf, fb->len - 1).c_str());
  Serial.println("break");
  //encode64(fb->buf, fb->len);
  //std::cout << encode64(fb->buf, fb->len) << std::endl;
  Serial.println("break");
  t(fb);
  //Serial.println(encode64(fb->buf, fb->len));
  Serial.println("break");
  //Serial.println(encode64(fb->buf, fb->len + 1).c_str());
  Serial.println("Sending");
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin(postAddress);      //Specify request destination
   //http.addHeader("Content-Type", "image/jpeg");  //Specify content-type header
   http.addHeader("Content-Type", "application/json");  //Specify content-type header

   ////photo p = photo(fb->buf, macA);
  
   //int httpCode = http.POST(reinterpret_cast <const char*> (fb->buf));   // payload (image), payload length
   int httpCode = http.POST("{\"id\": \"" + macA + "\", \"image\": \"" + reinterpret_cast <const char*> (fb->buf) + "\"}");
   String payload = http.getString();                  //Get the response payload

   Serial.println(httpCode);   //Print HTTP return code
   //Serial.println(fb->buf);
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 }else{
    Serial.println("Error in WiFi connection"); 
 }
}

//////////////////////////////////
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

// Code to encode image data as base64 code.
// Confirm functionality by printing the coded string
// and translating it back to an immage with:
// https://base64.guru/converter/decode/image
// So far String outputed does convert image completly.
// Unsure if error lies in encode64 or elsewhere.

String encode64(unsigned char const* bytes_to_encode, unsigned int in_len) {
//  String encoded_png;
//  //Mat img; // Load an image here
//  
//  std::vector<unsigned char> buf;
//  //cv::imencode(".png", fb, buf);
//  auto base64_png = reinterpret_cast<const unsigned char*>(buf.data());
//  encoded_png = "data:image/jpeg;base64," + base64_encode(base64_png, buf.size());
  
//  String toEncode = "Test encoding";
// 
//  String encoded = base64::encode(fb->buf);
// 
//  Serial.println(encoded);

  String ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
        //Serial.print("" + base64_chars[char_array_4[i]]);
        //std::cout << base64_chars[char_array_4[i]]);
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];
      //Serial.print("" + base64_chars[char_array_4[j]]);
      //std::cout << base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
      //Serial.print("" + '=');
      //std::cout << '=';

  }
  //Serial.println("out");
  return ret;
}


//Attemp to get base64 string from image data by using recasting the data bytes beforehand.
#include <inttypes.h>
void t(camera_fb_t * fb) {
  //std::vector<uchar> buf;
  //cv::imencode(".jpg", img, buf);
  auto *enc_msg = reinterpret_cast<unsigned char*>(fb->buf);
  Serial.println(encode64(enc_msg, fb->len));
}

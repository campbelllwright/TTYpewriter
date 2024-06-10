/*
  --------------
  WebSerial Demo
  --------------
  
  Skill Level: Beginner

  This example provides with a bare minimal app with WebSerial functionality.

  Github: https://github.com/ayushsharma82/WebSerial
  Wiki: https://docs.webserial.pro

  Works with following hardware:
  - ESP8266
  - ESP32

  WebSerial terminal will be accessible at your microcontroller's <IPAddress>/webserial URL.

  Checkout WebSerial Pro: https://webserial.pro
*/
#include "secrets.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>



AsyncWebServer server(80);

const char* ssid = MY_SSID; // Your WiFi SSID
const char* password = MY_PASS; // Your WiFi Password

unsigned long last_print_time = 0;// = millis();
unsigned int counter = 0;
char* buffer[256] = {0};

void setup() {
  
  Serial.begin(9600);
  //Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }
  
  // Once connected, print IP
  Serial.print("IP Address: ");
  //Serial.println(WiFi.localIP());
  last_print_time = millis();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is WebSerial demo. You can access webserial interface at http://" + WiFi.localIP().toString() + "/webserial");
  });

  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);

  /* Attach Message Callback */
  WebSerial.onMessage([&](uint8_t *data, size_t len) {
    Serial.printf("Received %u bytes from WebSerial: ", len-1);
    Serial.write(data, len-1);
    Serial.println();
    WebSerial.println("Received Data...");
    String d = "";
    for(size_t i=0; i < len; i++){
      d += char(data[i]);
    }
    WebSerial.println(d);
  });

  // Start server
  server.begin();
}

void loop() {
  
  // Print every 2 seconds (non-blocking)
  if ((unsigned long)(millis() - last_print_time) > 2000) {
    //WebSerial.print(F("IP address: "));
    //WebSerial.println(WiFi.localIP());
    //WebSerial.printf("Uptime: %lums\n", millis());
    //WebSerial.printf("Free heap: %u\n", ESP.getFreeHeap());
    //Serial.println(WiFi.localIP());
    last_print_time = millis();
    if(Serial.available() > 0){
        WebSerial.println(Serial.readString());
    }
  }
  

  WebSerial.loop();
  /*Serial.print("test! ");
  Serial.print(counter);
  Serial.print('\n');
  counter++;
  delay(1000);*/
}
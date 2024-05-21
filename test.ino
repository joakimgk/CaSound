#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#include <ArduinoOTA.h>

#define DATABASE_URL "./log.cgi"
#define STATUS_LED 2
#define DEBUG false

const char* ssid     = "SSID";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "password";     // The password of the Wi-Fi network

void createLog(String event) {
  if (DEBUG) {
    Serial.println(event);
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = String(DATABASE_URL) + "?log=" + urlEncode(event);
    //Serial.println(url);

    if (http.begin(client, url.c_str())) {
            // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (DEBUG) {
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      }
      // Free resources
      http.end();
    }
  }
}

void setup() {
  Serial.begin(9600, SERIAL_8N1);  // Initialize serial communication

  pinMode(STATUS_LED, OUTPUT);
  int blink = LOW;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    digitalWrite(STATUS_LED, blink);  // toggle LED
    blink = (blink == LOW ? HIGH : LOW);
    delay(1000);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(STATUS_LED, LOW);  // turn on LED
  } else {
    digitalWrite(STATUS_LED, HIGH);  // turn off LED
  }

  
  ArduinoOTA.onStart([]() {
    createLog("Start");
  });
  ArduinoOTA.onEnd([]() {
    createLog("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    createLog("Progress: " + (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    createLog("Error[" + String(error) + "]: ");
    if (error == OTA_AUTH_ERROR) createLog("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) createLog("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) createLog("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) createLog("Receive Failed");
    else if (error == OTA_END_ERROR) createLog("End Failed");
  });
  ArduinoOTA.begin();

  createLog("\nHeisan!! @ " + WiFi.localIP().toString());
  createLog(String(DATABASE_URL));
}


void printByteBits(byte b) {
  for (int i = 7; i >= 0; i--) { // Iterate from bit 7 to bit 0
    Serial.print((b >> i) & 1); // Shift right and mask with 1 to get the bit value
  }
  Serial.println(); // Move to the next line after printing all bits
}

unsigned char hello;
unsigned char _hello = 0x13;

unsigned char val[20];
int len1, len2, len3;
unsigned char _val = 0x06;

unsigned char val2[20];
unsigned char _val2 = 0x06;

unsigned char val3[20];
int len = 0;
int MAXLEN = 20;
String msg = "";

void loop() {
  ArduinoOTA.handle();

  while (!Serial.available()) {
  }
  unsigned char hello = Serial.read();  // 0x15

  delay(20);
  Serial.write(0x13);

  while (!Serial.available()) {
  }
  len = 0;
  while (Serial.available() && len < MAXLEN) {
    val[len++] = Serial.read();
  }
  val[len] = '\0';
  len1 = len;

  delay(20);
  Serial.write(0x06);

  while (!Serial.available()) {
  }
  len = 0;
  while (Serial.available() && len < MAXLEN) {
    val2[len++] = Serial.read();
  }
  val2[len] = '\0';
  len2 = len;

  delay(20);
  Serial.write(0x06);

  while (!Serial.available()) {
  }
  len = 0;
  while (Serial.available() && len < MAXLEN) {
    val3[len++] = Serial.read();
  }
  val3[len] = '\0';
  len3 = len;

  msg = "Read 0x" + String(hello, HEX);
  createLog(msg);

  msg = "Sent 0x" + String(0x13, HEX);
  createLog(msg);

  msg = "Read " + String(len1) + " bytes: ";
  for (int i = 0; i < len1; i++) {
    msg += " 0x" + String(val[i], HEX);
  }
  createLog(msg);

  msg = "Sent 0x" + String(0x06, HEX);
  createLog(msg);

  msg = "Read " + String(len2) + " bytes: ";
  for (int i = 0; i < len2; i++) {
    msg += " 0x" + String(val2[i], HEX);
  }
  createLog(msg);

  msg = "Sent 0x" + String(0x06, HEX);
  createLog(msg);

  msg = "Read " + String(len3) + " bytes: ";
  for (int i = 0; i < len3; i++) {
    msg += " 0x" + String(val3[i], HEX);
  }
  createLog(msg);

  //printByteBits(receivedChar);
}

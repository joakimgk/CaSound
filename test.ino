#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

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

  createLog("\nReady @ 9600");
  createLog(String(DATABASE_URL));
}


void printByteBits(byte b) {
  for (int i = 7; i >= 0; i--) { // Iterate from bit 7 to bit 0
    Serial.print((b >> i) & 1); // Shift right and mask with 1 to get the bit value
  }
  Serial.println(); // Move to the next line after printing all bits
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()) {
    unsigned char receivedChar = Serial.read();

    String msg = "Read data: 0x" + String(receivedChar, HEX);
    createLog(msg);
    //printByteBits(receivedChar);

    if (receivedChar == 0x15) {
      createLog("Received hello");
    } else {
      createLog("Received something else");
    }
    delay(100);
    Serial.write(0x13);

    while (Serial.available()) {
      char receivedChar2 = Serial.read();
      String msg2 = "Read 2 data: 0x" + String(receivedChar2, HEX);
      createLog(msg2);
      //printByteBits(receivedChar2);
    }

  }
}


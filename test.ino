#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#include <ArduinoOTA.h>

#define DATABASE_URL "./log.cgi"
#define STATUS_LED 2
#define TIMEOUT 100 // Timeout in milliseconds
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

  /*
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
  */

  createLog("\nHeisan!! @ " + WiFi.localIP().toString());
  createLog(String(DATABASE_URL));
}


void printByteBits(byte b) {
  for (int i = 7; i >= 0; i--) { // Iterate from bit 7 to bit 0
    Serial.print((b >> i) & 1); // Shift right and mask with 1 to get the bit value
  }
  Serial.println(); // Move to the next line after printing all bits
}

unsigned char buf[64];
int l = 0;
unsigned long lastByteTime = 0;
unsigned char variable;
double value;

const unsigned char varHeader[4] = {0x3a, 0x56, 0x41, 0x4c};  // :VAL
const unsigned char valHeader[5] = {0x3a, 0x00, 0x01, 0x00, 0x01 };  // :0101

double convert(char bcd) {
    // Each byte contains two BCD digits
    unsigned char highNibble = (bcd >> 4) & 0x0F; // High nibble
    unsigned char lowNibble = bcd & 0x0F;          // Low nibble
    
    return 10.0 * (double)highNibble + (double)lowNibble;
}

double parseBCD(unsigned char* bcdArray, int startIndex, int endIndex) {
  unsigned long long number = 0;
  
  for (int i = startIndex; i < endIndex; i++) {
    number = number * 100 + convert(bcdArray[i]);
  }

  double exps = (bcdArray[13] == 1) ? 
    endIndex - convert(bcdArray[14]) 
    : 100 - convert(bcdArray[14]) + endIndex;
  
  double scale = pow(10, exps);

  return (double)number/scale;
}

void loop() {
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      buf[l++] = Serial.read();
      lastByteTime = millis(); // Record the time when the last byte was received
    }
  } else if (l > 0 && (millis() - lastByteTime) > TIMEOUT) {
    // Process the buffer as no more bytes have been received within the timeout period
    String msg = "Read " + String(l) + " bytes: ";
    for (int i = 0; i < l; i++) {
      msg += " " + String(buf[i], HEX);
    }
    createLog(msg);

    if (l == 1 && buf[0] == 0x15) {
      Serial.write(0x13); // ACK byte for 0x15
    } else {
      if (memcmp(buf, varHeader, 4) == 0) {
        variable = buf[11];
        createLog(String((char)variable));
      }
      if (memcmp(buf, valHeader, 5) == 0) {
        value = parseBCD(buf, 5, 12);
        createLog(String(value));
      }
      Serial.write(0x06); // ACK byte for other cases
    }
    l = 0; // Reset buffer index for the next message
  }
}

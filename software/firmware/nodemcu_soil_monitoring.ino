#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Soil moisture sensor pin
const int soilPin = A0;
const int relayPin = D3; // Relay control pin

const int moistureThreshold = 500; // Threshold value

// WiFi credentials
const char* ssid = "admin";
const char* password = "45464545";

// ThingSpeak
const char* serverUrl = "http://api.thingspeak.com/update";
const char* writeAPIKey = "H86OVC0NP8C6WSFN";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Pump OFF (active LOW relay)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  int moistureValue = analogRead(soilPin);
  int pumpStatus;

  if (moistureValue < moistureThreshold) {
    digitalWrite(relayPin, LOW);  // Pump ON
    pumpStatus = 1;
    Serial.println("Pump ON");
  } else {
    digitalWrite(relayPin, HIGH); // Pump OFF
    pumpStatus = 0;
    Serial.println("Pump OFF");
  }

  Serial.print("Moisture: ");
  Serial.println(moistureValue);

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + "?api_key=" + writeAPIKey +
                 "&field1=" + String(moistureValue) +
                 "&field2=" + String(pumpStatus);

    http.begin(client, url);
    int httpResponse = http.GET();
    if (httpResponse > 0) {
      Serial.println("Data sent to ThingSpeak");
    } else {
      Serial.print("HTTP Error: ");
      Serial.println(httpResponse);
    }
    http.end();
  }

  delay(15000); // 15s delay (ThingSpeak rate limit)
}


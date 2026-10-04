/*
 * Smart Watering System for Home Gardens
 * ---------------------------------------
 * ESP32 + YL-69 Soil Moisture Sensor + RTC + Relay + Pump
 * + Wi-Fi + MQTT/ThingsBoard
 *
 * Based on the implementation documented in the project report.
 *
 * IMPORTANT:
 * - Wi-Fi and ThingsBoard credentials are loaded from config.h.
 * - Copy config.h.example to config.h and enter your local credentials.
 * - config.h is intentionally ignored by Git.
 */

#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include <RTClib.h>

#include "config.h"

// ============================================================
// PROJECT CONFIGURATION
// ============================================================

constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;

// Relay and sensor pins from the documented implementation.
constexpr uint8_t RELAY1_PIN = 26;
constexpr uint8_t RELAY2_PIN = 27;
constexpr uint8_t SOIL_MOISTURE_PIN = 34;

// ============================================================
// GLOBAL OBJECTS
// ============================================================

RTC_DS3231 rtc;

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// ============================================================
// GLOBAL STATE
// ============================================================

bool pumpState = false;
bool subscribed = false;

unsigned long pumpOnTime = 0;
unsigned long lastUpdateTime = 0;

// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

void initWiFi();
bool reconnect();

RPC_Response processSetpumpStatus(const RPC_Data &data);
void processTime(const JsonVariantConst &data);

void sendSensorData();
void sendPumpOnTime();

// ============================================================
// RPC CALLBACKS
// ============================================================

const std::array<RPC_Callback, 1U> callbacks = {
    RPC_Callback{"setpumpStatus", processSetpumpStatus}
};

// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);

  // Configure pump relay pins.
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  // Relay HIGH = pump OFF for the relay configuration
  // documented in the original implementation.
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);

  delay(100);

  // Initialize RTC.
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (true) {
      delay(1000);
    }
  }

  // If the RTC lost power, initialize it using the
  // compilation date/time.
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  initWiFi();
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {
  // Small delay to avoid excessive loop execution.
  delay(10);

  DateTime now = rtc.now();

  // Reconnect to Wi-Fi if necessary.
  if (!reconnect()) {
    return;
  }

  // Connect to ThingsBoard when disconnected.
  if (!tb.connected()) {
    Serial.print("Connecting to: ");
    Serial.println(THINGSBOARD_SERVER);
    Serial.print(" with token ");

    if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect");
      return;
    }

    // Subscribe to RPC callbacks.
    Serial.println("Subscribing for RPC...");

    if (!tb.RPC_Subscribe(callbacks.begin(), callbacks.end())) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }
  }

  // ----------------------------------------------------------
  // PUMP CONTROL
  // ----------------------------------------------------------

  digitalWrite(RELAY1_PIN, pumpState ? LOW : HIGH);

  // Track total pump ON time.
  // The original implementation increments this counter
  // by 10 ms because loop() contains delay(10).
  if (pumpState) {
    pumpOnTime += 10;
  }

  // Send accumulated pump ON time every 5 seconds.
  if (millis() - lastUpdateTime >= 5000) {
    sendPumpOnTime();
    lastUpdateTime = millis();
  }

  // Display RTC time on Serial Monitor.
  Serial.print("Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.println(now.second(), DEC);

  // ----------------------------------------------------------
  // SCHEDULED WATERING
  // ----------------------------------------------------------
  //
  // The documented implementation checks for second 59 and
  // briefly activates the pump for scheduled watering.
  //
  if (now.second() % 59 == 0) {
    digitalWrite(RELAY1_PIN, LOW);
    delay(5000);

    sendSensorData();

    digitalWrite(RELAY1_PIN, HIGH);
  }

  // ----------------------------------------------------------
  // REQUEST CURRENT TIME FROM THINGSBOARD
  // ----------------------------------------------------------

  if (!subscribed) {
    Serial.println("Requesting RPC...");

    RPC_Request_Callback requestCallback(
        "getCurrentTime",
        processTime
    );

    if (!tb.RPC_Request(requestCallback)) {
      Serial.println("Failed to request for RPC");
      return;
    }

    Serial.println("Request done");
    subscribed = true;
  }

  // Send current soil-moisture data.
  sendSensorData();

  // Process incoming ThingsBoard messages.
  tb.loop();
}

// ============================================================
// WI-FI
// ============================================================

void initWiFi() {
  Serial.println("Connecting to AP...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to AP");
}

bool reconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }

  return WiFi.status() == WL_CONNECTED;
}

// ============================================================
// RPC: PUMP CONTROL
// ============================================================

RPC_Response processSetpumpStatus(const RPC_Data &data) {
  // Process the RPC request to change the pump state.
  int dataInt = data;

  pumpState = (dataInt == 1);

  Serial.println(
      pumpState ? "pump ON" : "pump OFF"
  );

  // Keep the accumulated pump-on time.
  return RPC_Response("newStatus", dataInt);
}

// ============================================================
// RPC: TIME RESPONSE
// ============================================================

void processTime(const JsonVariantConst &data) {
  Serial.print("Received time from ThingsBoard: ");
  Serial.println(data["time"].as<String>());
}

// ============================================================
// TELEMETRY: PUMP ON TIME
// ============================================================

void sendPumpOnTime() {
  // Send total pump ON time in seconds.
  tb.sendTelemetryData(
      "pumpOnTime",
      pumpOnTime / 1000
  );

  Serial.print("Sending pump ON time: ");
  Serial.print(pumpOnTime / 1000);
  Serial.println(" seconds");
}

// ============================================================
// TELEMETRY: SOIL MOISTURE
// ============================================================

void sendSensorData() {
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  Serial.print("Raw Soil Moisture Value: ");
  Serial.println(soilMoistureValue);

  // ESP32 ADC range used in the documented implementation.
  float moisturePercentage =
      map(soilMoistureValue, 0, 4095, 100, 0);

  moisturePercentage =
      constrain(moisturePercentage, 0, 100);

  Serial.printf(
      "Sending Soil Moisture: %.1f%%\n",
      moisturePercentage
  );

  tb.sendTelemetryData(
      "soilMoisture",
      moisturePercentage
  );
}

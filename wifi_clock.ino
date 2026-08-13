/*
 * =====================================================================
 * wifi_clock.ino
 *
 * A low-cost digital clock using an ESP32 + TM1637 4-digit 7-segment
 * display. Time is fetched over WiFi from an NTP (Network Time
 * Protocol) server, so no separate RTC module/battery is needed.
 *
 * Hardware:
 *   - ESP32 dev board
 *   - TM1637 4-digit 7-segment display module
 *
 * Wiring (see docs/wiring.md for a diagram):
 *   TM1637 CLK -> ESP32 GPIO 22
 *   TM1637 DIO -> ESP32 GPIO 21
 *   TM1637 VCC -> 3.3V (or 5V, check your module)
 *   TM1637 GND -> GND
 *
 * Library required (install via Arduino Library Manager):
 *   "TM1637Display" by Avishay Orpaz
 *
 * WiFi credentials:
 *   Copy config/secrets_example.h to config/secrets.h and fill in your
 *   own WiFi SSID/password. secrets.h is gitignored so you never
 *   accidentally commit your credentials.
 * =====================================================================
 */

#include <WiFi.h>
#include <TM1637Display.h>
#include "time.h"
#include "../config/secrets.h"   // defines WIFI_SSID and WIFI_PASSWORD

// ---------------- Pin configuration ----------------
#define CLK_PIN 22
#define DIO_PIN 21

TM1637Display display(CLK_PIN, DIO_PIN);

// ---------------- Time configuration ----------------
// Set gmtOffset_sec for your timezone, e.g. IST (UTC+5:30) = 19800
const long  gmtOffset_sec      = 19800;
const int   daylightOffset_sec = 0;
const char* ntpServer          = "pool.ntp.org";

// Re-sync with the NTP server this often (in milliseconds)
const unsigned long RESYNC_INTERVAL_MS = 60UL * 60UL * 1000UL; // 1 hour

unsigned long lastResync = 0;
bool colonOn = true;

// ---------------- WiFi connection ----------------
void connectWiFi() {
  display.setSegments((const uint8_t[]){0, 0, 0, 0}); // blank display while connecting

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    // After 15s, show a "connecting" pattern on the display instead of hanging silently
    if (millis() - start > 15000) {
      Serial.println("\nStill trying to connect...");
      start = millis();
    }
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ---------------- NTP time sync ----------------
void syncTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  Serial.print("Waiting for NTP time sync...");
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 20) {
    Serial.print(".");
    delay(500);
    attempts++;
  }

  if (attempts >= 20) {
    Serial.println("\nFailed to sync time. Will retry later.");
  } else {
    Serial.println("\nTime synced.");
    lastResync = millis();
  }
}

// ---------------- Display update ----------------
void updateDisplay() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return; // no valid time yet, skip this update
  }

  int hours   = timeinfo.tm_hour;
  int minutes = timeinfo.tm_min;

  int displayValue = hours * 100 + minutes;

  uint8_t colonMask = colonOn ? 0b01000000 : 0; // TM1637 colon bit on digit 2
  display.showNumberDecEx(displayValue, colonMask, true);

  colonOn = !colonOn; // blink the colon once per second
}

// ---------------- Setup / loop ----------------
void setup() {
  Serial.begin(115200);
  display.setBrightness(5); // 0 (dim) - 7 (brightest)

  connectWiFi();
  syncTime();
}

void loop() {
  // Reconnect WiFi if it drops
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectWiFi();
    syncTime();
  }

  // Periodically re-sync with the NTP server to correct for drift
  if (millis() - lastResync > RESYNC_INTERVAL_MS) {
    syncTime();
  }

  updateDisplay();
  delay(500); // blink colon at ~1Hz while keeping the display responsive
}

#define GLOBALS_IMPL

#include "config.h"
#include "globals.h"
#include "storage.h"
#include "shake.h"
#include "modes/modes.h"

#if DATA_LINE_TEST

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println(F("DATA LINE TEST"));
  Serial.println(F("D6 changes between 0V and 5V every 2 seconds"));
}

void loop() {
  static bool dataHigh = false;
  static unsigned long lastToggleMs = 0;

  if (static_cast<unsigned long>(millis() - lastToggleMs) >= 2000UL) {
    lastToggleMs = millis();
    dataHigh = !dataHigh;
    digitalWrite(LED_PIN, dataHigh ? HIGH : LOW);
    Serial.println(dataHigh ? F("D6 = HIGH (~5V)") : F("D6 = LOW (~0V)"));
  }
}

#else

void setup() {
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

#if SERIAL_DEBUG
  Serial.begin(115200);
#endif

  loadSettings();

#if SERIAL_DEBUG
  Serial.println("Crystal Lamp");
  Serial.print("LED pin: ");
  Serial.println(LED_PIN);
  Serial.print("LED count: ");
  Serial.println(numLeds);
  Serial.print(F("Shake pin: D"));
  Serial.println(SHAKE_PIN);
  Serial.print(F("Mode: "));
  Serial.println(modeNames[lampMode]);
  Serial.print(F("Lamp: "));
  Serial.println(lampEnabled ? F("ON") : F("OFF"));
#endif

  FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, numLeds).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(maxBrightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, POWER_LIMIT_MA);

  modeStartedAtMs = millis();
  updateMode();
  lastUpdate = millis();
  setupShakeSensor();
}

void loop() {
  handleShakeSensor();

#if !SENSOR_RAW_DEBUG
  if (millis() - lastUpdate >= UPDATE_TICK_MS) {
    lastUpdate = millis();
    updateMode();
  }
#endif
}

#endif

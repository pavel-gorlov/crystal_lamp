#pragma once

#include "config.h"
#include "globals.h"
#include "storage.h"

volatile bool shakePulsePending = false;
volatile bool shakeMotionStartPending = false;
volatile unsigned long lastShakePulseMicros = 0;
volatile byte shakeEdgeCount = 0;
unsigned long shakeSensorArmedAtMs = 0;

#if SENSOR_RAW_DEBUG
const byte RAW_EVENT_BUFFER_SIZE = 64;
volatile unsigned long rawEventMicros[RAW_EVENT_BUFFER_SIZE];
volatile byte rawEventState[RAW_EVENT_BUFFER_SIZE];
volatile byte rawEventHead = 0;
volatile byte rawEventTail = 0;
volatile byte rawEventDropped = 0;
#endif

void onShakePulse() {
  const unsigned long pulseMicros = micros();

#if SENSOR_RAW_DEBUG
  const byte nextHead = (rawEventHead + 1) % RAW_EVENT_BUFFER_SIZE;
  if (nextHead != rawEventTail) {
    rawEventMicros[rawEventHead] = pulseMicros;
    rawEventState[rawEventHead] = digitalRead(SHAKE_PIN);
    rawEventHead = nextHead;
  } else if (rawEventDropped < 255) {
    rawEventDropped++;
  }
#else
  if (!shakePulsePending) {
    shakeMotionStartPending = true;
  }
  shakePulsePending = true;
  lastShakePulseMicros = pulseMicros;
  if (shakeEdgeCount < 255) {
    shakeEdgeCount++;
  }
#endif
}

#if SENSOR_RAW_DEBUG
inline void printRawSensorEvents() {
  while (true) {
    unsigned long eventMicros = 0;
    byte eventState = LOW;
    byte droppedEvents = 0;
    bool hasEvent = false;

    noInterrupts();
    if (rawEventTail != rawEventHead) {
      eventMicros = rawEventMicros[rawEventTail];
      eventState = rawEventState[rawEventTail];
      rawEventTail = (rawEventTail + 1) % RAW_EVENT_BUFFER_SIZE;
      hasEvent = true;
    } else {
      droppedEvents = rawEventDropped;
      rawEventDropped = 0;
    }
    interrupts();

    if (!hasEvent) {
      if (droppedEvents > 0) {
        Serial.print(F("RAW dropped="));
        Serial.println(droppedEvents);
      }
      return;
    }

    Serial.print(F("RAW t_us="));
    Serial.print(eventMicros);
    Serial.print(F(" state="));
    Serial.println(eventState == HIGH ? 1 : 0);
  }
}
#endif

inline void setupShakeSensor() {
  pinMode(SHAKE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SHAKE_PIN), onShakePulse, CHANGE);

  noInterrupts();
  shakePulsePending = false;
  shakeMotionStartPending = false;
  shakeEdgeCount = 0;
#if SENSOR_RAW_DEBUG
  rawEventHead = 0;
  rawEventTail = 0;
  rawEventDropped = 0;
#endif
  interrupts();
  shakeSensorArmedAtMs = millis() + SHAKE_DEBOUNCE_MS;

#if SENSOR_DEBUG
  Serial.print(F("KY-020 initial S: "));
  Serial.println(digitalRead(SHAKE_PIN) == HIGH ? F("HIGH") : F("LOW"));
#endif

#if SENSOR_RAW_DEBUG
  Serial.print(F("RAW t_us="));
  Serial.print(micros());
  Serial.print(F(" state="));
  Serial.print(digitalRead(SHAKE_PIN) == HIGH ? 1 : 0);
  Serial.println(F(" initial"));
#endif
}

inline void selectNextMode() {
  lampMode = (lampMode + 1) % MODE_COUNT;
  modeStartedAtMs = millis();
  applyModePreset();
  FastLED.setBrightness(maxBrightness);
  saveSettings();

#if SERIAL_DEBUG
  Serial.print(F("Mode: "));
  Serial.println(modeNames[lampMode]);
#endif
}

inline void handleShakeSensor() {
#if SENSOR_RAW_DEBUG
  printRawSensorEvents();
#else
#if SENSOR_DEBUG && SENSOR_LIVE_INTERVAL_MS > 0
  static unsigned long lastDebugMs = 0;
#endif
  bool motionStarted = false;
  bool pulsePendingSnapshot = false;
  unsigned long lastPulseMicrosSnapshot = 0;

  noInterrupts();
  motionStarted = shakeMotionStartPending;
  shakeMotionStartPending = false;
  pulsePendingSnapshot = shakePulsePending;
  lastPulseMicrosSnapshot = lastShakePulseMicros;
  interrupts();

  if (motionStarted) {
    const bool sensorArmed =
        static_cast<long>(millis() - shakeSensorArmedAtMs) >= 0;

    if (sensorArmed) {
#if SENSOR_DEBUG
      Serial.println(F("KY-020 motion start"));
#endif
      selectNextMode();
    } else {
#if SENSOR_DEBUG
      Serial.println(F("KY-020 startup motion ignored"));
#endif
    }
  }

  const unsigned long nowMicros = micros();
  if (pulsePendingSnapshot &&
      static_cast<unsigned long>(nowMicros - lastPulseMicrosSnapshot) >=
          static_cast<unsigned long>(SHAKE_DEBOUNCE_MS) * 1000UL) {
#if SENSOR_DEBUG
    byte burstEdges = 0;
    bool rearmed = false;
#endif

    noInterrupts();
    if (shakePulsePending && lastShakePulseMicros == lastPulseMicrosSnapshot) {
      shakePulsePending = false;
#if SENSOR_DEBUG
      burstEdges = shakeEdgeCount;
      rearmed = true;
#endif
      shakeEdgeCount = 0;
    }
    interrupts();

#if SENSOR_DEBUG
    if (rearmed) {
      Serial.print(F("KY-020 rearmed after "));
      Serial.print(SHAKE_DEBOUNCE_MS);
      Serial.print(F(" ms, edges="));
      Serial.println(burstEdges);
    }
#endif
  }

#if SENSOR_DEBUG && SENSOR_LIVE_INTERVAL_MS > 0
  const unsigned long nowMs = millis();
  if (static_cast<unsigned long>(nowMs - lastDebugMs) >= SENSOR_LIVE_INTERVAL_MS) {
    lastDebugMs = nowMs;
    Serial.print(F("KY-020 S="));
    Serial.println(digitalRead(SHAKE_PIN) == HIGH ? F("HIGH") : F("LOW"));
  }
#endif
#endif
}

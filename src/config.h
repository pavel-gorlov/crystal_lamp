#pragma once

#include <Arduino.h>

#ifndef LED_PIN
  #define LED_PIN 6
#endif

#ifndef SHAKE_PIN
  #define SHAKE_PIN 2
#endif

#ifndef SHAKE_DEBOUNCE_MS
  #define SHAKE_DEBOUNCE_MS 500
#endif

#ifndef SENSOR_DEBUG
  #define SENSOR_DEBUG 0
#endif

#ifndef SENSOR_RAW_DEBUG
  #define SENSOR_RAW_DEBUG 0
#endif

#ifndef SENSOR_LIVE_INTERVAL_MS
  #define SENSOR_LIVE_INTERVAL_MS 2000
#endif

#ifndef SERIAL_DEBUG
  #define SERIAL_DEBUG 0
#endif

#ifndef DATA_LINE_TEST
  #define DATA_LINE_TEST 0
#endif

#ifndef DEFAULT_NUM_LEDS
  #define DEFAULT_NUM_LEDS 15
#endif

#ifndef MAX_LEDS
  #define MAX_LEDS 150
#endif

#ifndef LED_TYPE
  #define LED_TYPE WS2811
#endif

#ifndef LED_COLOR_ORDER
  #define LED_COLOR_ORDER GRB
#endif

#ifndef POWER_LIMIT_MA
  #define POWER_LIMIT_MA 400
#endif

#define UPDATE_TICK_MS 10
#define SETTINGS_MAGIC 0x43
#define SETTINGS_VERSION 12

enum Mode {
  MODE_CRYSTAL_BREATH = 0,
  MODE_AURORA,
  MODE_PRISM,
  MODE_GLITTER,
  MODE_CANDLE_CORE,
  MODE_METEOR,
  MODE_LIGHTNING,
  MODE_COUNT
};

const char* const modeNames[] = {
  "Crystal Breath",
  "Aurora",
  "Prism",
  "Glitter",
  "Candle Core",
  "Meteor",
  "Lightning"
};

const byte presetSpeed[] =      {35, 20, 18, 55, 28, 50, 50};
const byte presetBrightness[] = {255, 255, 255, 255, 255, 255, 255};

static_assert(sizeof(modeNames) / sizeof(modeNames[0]) == MODE_COUNT,
              "modeNames must match Mode");
static_assert(sizeof(presetSpeed) / sizeof(presetSpeed[0]) == MODE_COUNT,
              "presetSpeed must match Mode");
static_assert(sizeof(presetBrightness) / sizeof(presetBrightness[0]) == MODE_COUNT,
              "presetBrightness must match Mode");

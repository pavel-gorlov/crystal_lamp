#pragma once

#include "config.h"
#include <FastLED.h>

extern CRGB leds[MAX_LEDS];
extern int numLeds;
extern int lampMode;
extern bool lampEnabled;
extern int effectSpeed;
extern int maxBrightness;
extern byte baseHue;
extern unsigned long lastUpdate;
extern unsigned long modeStartedAtMs;

#ifdef GLOBALS_IMPL
CRGB leds[MAX_LEDS];
int numLeds = DEFAULT_NUM_LEDS;
int lampMode = MODE_CRYSTAL_BREATH;
bool lampEnabled = true;
int effectSpeed = 35;
int maxBrightness = 90;
byte baseHue = 145;
unsigned long lastUpdate = 0;
unsigned long modeStartedAtMs = 0;
#endif

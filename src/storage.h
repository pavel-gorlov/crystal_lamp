#pragma once

#include "config.h"
#include "globals.h"
#include <EEPROM.h>

struct StoredSettings {
  byte magic;
  byte version;
  byte mode;
  byte enabled;
  byte baseHue;
  uint16_t numLeds;
};

inline void applyModePreset() {
  effectSpeed = presetSpeed[lampMode];
  maxBrightness = presetBrightness[lampMode];
}

inline void loadSettings() {
  StoredSettings stored;
  EEPROM.get(0, stored);

  const bool valid =
      stored.magic == SETTINGS_MAGIC &&
      stored.version == SETTINGS_VERSION &&
      stored.mode < MODE_COUNT &&
      stored.enabled <= 1 &&
      stored.numLeds >= 1 &&
      stored.numLeds <= MAX_LEDS;

  if (!valid) {
    lampMode = MODE_CRYSTAL_BREATH;
    lampEnabled = true;
    baseHue = 145;
    numLeds = DEFAULT_NUM_LEDS;
    applyModePreset();
    return;
  }

  lampMode = stored.mode;
  lampEnabled = stored.enabled != 0;
  baseHue = stored.baseHue;
  numLeds = stored.numLeds;
  applyModePreset();
}

inline void saveSettings() {
  const StoredSettings stored = {
      SETTINGS_MAGIC,
      SETTINGS_VERSION,
      static_cast<byte>(lampMode),
      static_cast<byte>(lampEnabled),
      baseHue,
      static_cast<uint16_t>(numLeds)
  };
  EEPROM.put(0, stored);
}

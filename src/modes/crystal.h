#pragma once

#include "../config.h"
#include "../globals.h"

inline void updateOff() {
  fadeToBlackBy(leds, numLeds, 28);
  FastLED.show();
}

inline void updateCrystalBreath() {
  const uint8_t breath = beatsin8(map(effectSpeed, 5, 100, 18, 4), 35, maxBrightness);
  for (int i = 0; i < numLeds; i++) {
    uint8_t shimmer = inoise8(i * 42, millis() / 18);
    leds[i] = CHSV(baseHue + (i * 2), 55 + (shimmer / 5), scale8(breath, 170 + shimmer / 3));
  }
  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

inline void updateAurora() {
  for (int i = 0; i < numLeds; i++) {
    uint8_t n = inoise8(i * 32, millis() / map(effectSpeed, 5, 100, 3, 16));
    leds[i] = CHSV(baseHue + n / 3 + i, 180, n);
  }
  blur1d(leds, numLeds, 48);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

inline void updatePrism() {
  fill_rainbow(leds, numLeds, baseHue + millis() / map(effectSpeed, 5, 100, 5, 35), 255 / max(1, numLeds / 2));
  for (int i = 0; i < numLeds; i += 3) {
    leds[i].fadeLightBy(80);
  }
  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

inline void updateGlitter() {
  fadeToBlackBy(leds, numLeds, map(effectSpeed, 5, 100, 12, 38));
  if (random8() < 90) {
    leds[random16(numLeds)] += CHSV(baseHue + random8(70), 80, 255);
  }
  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

inline void updateCandleCore() {
  for (int i = 0; i < numLeds; i++) {
    uint8_t heat = qsub8(inoise8(i * 24, millis() / map(effectSpeed, 5, 100, 3, 18)), 35);
    leds[i] = HeatColor(heat);
    leds[i].nscale8_video(180);
  }
  blur1d(leds, numLeds, 64);
  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

struct MeteorState {
  bool initialized;
  bool flying;
  unsigned long activeModeStartedAtMs;
  unsigned long phaseStartedMs;
  uint16_t travelMs;
  uint16_t pauseMs;
  byte hue;
  byte saturation;
  byte startBrightness;
  byte endBrightness;
};

inline void startMeteorFlight(MeteorState& state, unsigned long nowMs) {
  state.flying = true;
  state.phaseStartedMs = nowMs;
  state.travelMs = random16(700, 1401);
  state.hue = random8();
  state.saturation = random8(180, 255);

  if (random8() < 128) {
    state.startBrightness = random8(180, 255);
    state.endBrightness = random8(30, 110);
  } else {
    state.startBrightness = random8(30, 110);
    state.endBrightness = random8(180, 255);
  }
}

inline void updateMeteor() {
  static MeteorState state = {
      false, false, 0, 0, 1000, 500, 0, 255, 255, 64
  };
  const unsigned long nowMs = millis();

  if (!state.initialized || state.activeModeStartedAtMs != modeStartedAtMs) {
    state.initialized = true;
    state.activeModeStartedAtMs = modeStartedAtMs;
    startMeteorFlight(state, nowMs);
  }

  fill_solid(leds, numLeds, CRGB::Black);

  if (state.flying) {
    const unsigned long elapsedMs =
        static_cast<unsigned long>(nowMs - state.phaseStartedMs);

    if (elapsedMs >= state.travelMs) {
      state.flying = false;
      state.phaseStartedMs = nowMs;
      state.pauseMs = random16(250, 901);
    } else {
      const uint16_t ledCount = static_cast<uint16_t>(numLeds);
      uint16_t step =
          static_cast<uint16_t>((elapsedMs * ledCount) / state.travelMs);
      if (step >= ledCount) {
        step = ledCount - 1;
      }

      const byte progress =
          static_cast<byte>((elapsedMs * 255UL) / state.travelMs);
      const int32_t brightnessDelta =
          static_cast<int32_t>(state.endBrightness) - state.startBrightness;
      const byte brightness = static_cast<byte>(
          state.startBrightness + (brightnessDelta * progress) / 255L);

      leds[static_cast<int>(ledCount - 1 - step)] =
          CHSV(state.hue, state.saturation, brightness);
    }
  } else if (
      static_cast<unsigned long>(nowMs - state.phaseStartedMs) >= state.pauseMs) {
    startMeteorFlight(state, nowMs);
  }

  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

inline void updateLightning() {
  static bool initialized = false;
  static bool flashOn = false;
  static byte flashesRemaining = 0;
  static byte flashPattern = 0;
  static byte flashHue = HUE_YELLOW;
  static byte flashBrightness = 255;
  static byte flashPointThreshold = 96;
  static unsigned long activeModeStartedAtMs = 0;
  static unsigned long nextTransitionMs = 0;
  const unsigned long nowMs = millis();

  if (!initialized || activeModeStartedAtMs != modeStartedAtMs) {
    initialized = true;
    flashOn = false;
    flashesRemaining = 0;
    activeModeStartedAtMs = modeStartedAtMs;
    nextTransitionMs = nowMs + random16(1200, 3201);
  }

  if (static_cast<long>(nowMs - nextTransitionMs) >= 0) {
    if (!flashOn) {
      if (flashesRemaining == 0) {
        flashesRemaining = random8(2, 5);
      }
      flashOn = true;
      flashPattern = random8();
      flashHue = random8(42, 75);
      flashBrightness = random8(190, 255);
      flashPointThreshold = random8(70, 145);
      nextTransitionMs = nowMs + random16(35, 81);
    } else {
      flashOn = false;
      flashesRemaining--;
      nextTransitionMs = flashesRemaining == 0
          ? nowMs + random16(1200, 3501)
          : nowMs + random16(45, 121);
    }
  }

  fill_solid(leds, numLeds, CRGB(0, 0, 48));

  if (flashOn) {
    for (int i = 0; i < numLeds; i++) {
      const byte pointHash = static_cast<byte>(
          (i * 73) ^ (flashPattern * 29 + i * 11));
      const bool guaranteedPoint = i == flashPattern % numLeds;

      if (guaranteedPoint || pointHash < flashPointThreshold) {
        const int8_t hueOffset = static_cast<int8_t>(pointHash % 11) - 5;
        const byte pointHue = static_cast<byte>(flashHue + hueOffset);
        const byte pointSaturation =
            static_cast<byte>(220 + pointHash % 35);
        const byte pointBrightness =
            qsub8(flashBrightness, pointHash % 48);
        leds[i] = CHSV(pointHue, pointSaturation, pointBrightness);
      } else {
        leds[i] = CRGB(0, 0, 20);
      }
    }
  }

  FastLED.setBrightness(maxBrightness);
  FastLED.show();
}

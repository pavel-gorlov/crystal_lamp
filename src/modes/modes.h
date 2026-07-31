#pragma once

#include "crystal.h"

inline void updateMode() {
  if (!lampEnabled) {
    updateOff();
    return;
  }

  switch (lampMode) {
    case MODE_CRYSTAL_BREATH:
      updateCrystalBreath();
      break;
    case MODE_AURORA:
      updateAurora();
      break;
    case MODE_PRISM:
      updatePrism();
      break;
    case MODE_GLITTER:
      updateGlitter();
      break;
    case MODE_CANDLE_CORE:
      updateCandleCore();
      break;
    case MODE_METEOR:
      updateMeteor();
      break;
    case MODE_LIGHTNING:
      updateLightning();
      break;
  }
}

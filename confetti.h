#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// https://gist.github.com/mneuschaefer/826fff551a72578c0871cc66dd4179d9
class ConfettiEffect : public Effect {
protected:
  uint8_t Hue = 0;
  unsigned long lastConfettiChange;
  unsigned long lastHueChange;

  virtual void confetti() {
    fadeToBlackBy( Leds, Size, 1 );
    int pos = random16(Size);
    Leds[pos] += CHSV(Hue + random8(64), 200, 255);
  }
  
  virtual void changeHue() {
    ++Hue;
  }
public:
  ConfettiEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property) 
    : Effect(pin, leds, palette, property)
  {
  }
  virtual void Loop() {
    if (millis() > lastConfettiChange + ChangeEveryMs) {
      lastConfettiChange = millis();
      confetti();
      FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
    }
    if (millis() > lastHueChange + 20) {
      lastHueChange = millis();
      changeHue();
    }
  }
  virtual void PropertyChanged() {
    if (IsOn()) {
      lastHueChange = lastConfettiChange = millis();
    }
  }
  virtual bool IsOn() {
    return ((CloudDimmedLight&)mProperty).getSwitch();
  }
  virtual void SetChangeEveryMs(uint16_t ms) {
    Effect::SetChangeEveryMs(ms);
    PropertyChanged();
  }
  virtual ~ConfettiEffect() {
  }
};
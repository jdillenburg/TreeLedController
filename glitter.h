#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// https://gist.github.com/mneuschaefer/826fff551a72578c0871cc66dd4179d9
class GlitterEffect : public Effect {
protected:
  int ChanceOfGlitter;
  unsigned long lastChanged;
  
  virtual void glitter() {
    Leds[ random16(Size) ] += CRGB::White;
  }
public:
  GlitterEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudLight& property) 
    : Effect(pin, leds, palette, property), 
      ChanceOfGlitter(80)
  {
  }
  virtual void Loop() {
    if (millis() > lastChanged + ChangeEveryMs) {
      lastChanged = millis();
      glitter();
    }
  }
  virtual void PropertyChanged() {
    lastChanged = millis();
  }
  virtual bool IsOn() {
    return ((CloudLight&)mProperty);
  }
  virtual void SetChangeEveryMs(uint16_t ms) {
    Effect::SetChangeEveryMs(ms);
    PropertyChanged();
  }
};
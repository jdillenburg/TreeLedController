#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// https://github.com/atuline/FastLED-Demos/blob/master/aanimations/aanimations.ino
class BlendEffect : public Effect {
protected:
  unsigned long lastUpdate;
  
  virtual void blendColors() {
    uint8_t ratio1 = beatsin8(2);
    uint8_t ratio2 = beatsin8(3);
    for (int i = 0;  i < Size;  i++) {
      uint8_t f1 = (millis() / 10) + (i * 12);
      if (f1 > 128) {
        f1 = 0;
      }
      uint8_t f2 = (millis() / 5) - (i * 12);
      if (f2 > 128) {
        f2 = 0;
      }
      uint8_t f3 = (millis() / 7) - (i * 6);
      if (f3 > 128) {
        f3 = 0;
      }
      CRGB c1 = CRGB(Palette[0]);
      CRGB c2 = CRGB(Palette[1]);
      CRGB c3 = CRGB(Palette[2]);
      CRGB b1 = blend(c1.nscale8_video(f1), c2.nscale8_video(f2), ratio1);
      Leds[i] = blend(b1, c3.nscale8_video(f3), ratio2);
    }
  }
public:
  BlendEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property) 
    : Effect(pin, leds, palette, property)
  {
  }
  virtual void Loop() {
    if (millis() > lastUpdate + ChangeEveryMs) {
      lastUpdate = millis();
      blendColors();
      FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
    }
  }
  virtual void PropertyChanged() {
    if (IsOn()) {
      lastUpdate = millis();
    }
  }
  virtual bool IsOn() {
    return ((CloudDimmedLight&)mProperty).getSwitch();
  }
  virtual void SetChangeEveryMs(uint16_t ms) {
    Effect::SetChangeEveryMs(ms);
    PropertyChanged();
  }
};
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// https://gist.github.com/mneuschaefer/826fff551a72578c0871cc66dd4179d9
class SweepEffect : public Effect {
protected:
  int& Bpm;
  unsigned long lastUpdate;
  
  virtual void sweep() {
    int head = beatsin88(Bpm, 0, Size);
    for (int i = 0;  i < 10;  i++) {
      int pos = head - i;
      if (pos < 0) {
        pos = 0;
      }
      Leds[pos] += ColorFromPalette(Palette, i, 255);
      fadeToBlackBy( Leds, Size, 2 );
    }
  }
public:
  SweepEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property, int& bpm) 
    : Effect(pin, leds, palette, property),
      Bpm(bpm)
  {
  }
  virtual void Loop() {
    if (millis() > lastUpdate + ChangeEveryMs) {
      lastUpdate = millis();
      sweep();
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
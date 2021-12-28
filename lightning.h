#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// https://github.com/atuline/FastLED-Demos/blob/master/lightnings/lightnings.ino
class LightningEffect : public Effect {
protected:
  uint8_t flashes = 8;
  unsigned long nextUpdate;
  uint16_t ledstart;
  uint16_t ledlen;
  int flashCounter;
  int flashSteps;
  
  virtual void startFlashes() {
    ledstart = random16(Size);                                   // Determine starting location of flash
    ledlen = random16(Size - ledstart);                          // Determine length of flash (not to go beyond SIZE-1)
    flashSteps = 2*random8(3, flashes);                          // num steps is twice num flashse since odd steps "clear" last flash
    flashCounter = 1;
    Leds(ledstart,ledstart+ledlen).fill_solid(CHSV(255, 0, 51)); // lead flash is 255/5 = 51 brightness
    nextUpdate = millis() + random8(4, 10);                      // flashes are 4-10 millis
  }
  
  virtual void clearLastFlash() {
    Leds(ledstart,ledstart+ledlen).fill_solid(CHSV(255, 0, 0));  // draw black over last flash to erase it
    if (flashCounter == 1) {
      nextUpdate = millis() + 150;                               // longer delay after first flash
    }
    else {
      nextUpdate = millis() + 50 + random8(100);
    }
    ++flashCounter;
    if (flashCounter >= flashSteps) {
      nextUpdate = millis() + random16(ChangeEveryMs/2, ChangeEveryMs*2);
      flashCounter = 0;
    }
  }
  
  virtual void nextFlash() {
    ledstart = random16(Size);                                   // Determine starting location of flash
    ledlen = random16(Size - ledstart);                          // Determine length of flash (not to go beyond SIZE-1)
    uint8_t brightness = 255/random8(1,3);                       // return strokes are brighter than the leader which is divided by 5
    Leds(ledstart,ledstart+ledlen).fill_solid(CHSV(255, 0, brightness));
    nextUpdate = millis() + random8(4, 10);
    ++flashCounter;
  }

public:
  LightningEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property) 
    : Effect(pin, leds, palette, property)
  {
  }
  virtual void Loop() {
    if (millis() > nextUpdate) {
      if (flashCounter == 0) {
        startFlashes();
      }
      else if (flashCounter % 2 == 1) {
        clearLastFlash();
      }
      else {
        nextFlash();
      }
      FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
    }
  }
  virtual void PropertyChanged() {
    if (IsOn()) {
      nextUpdate = millis();
      flashCounter = 0;
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
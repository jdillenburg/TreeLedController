#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

class ColoredEffect : public Effect {
public:
  ColoredEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudColoredLight& property) 
    : Effect(pin, leds, palette, property)
  {
  
  }
  virtual void Loop() {
    Color c = ((CloudColoredLight&)mProperty).getValue();
    CRGB crgb = colorToCRGB(c);
    Leds(0,Size).fill_solid(crgb);
    FastLED.setBrightness((uint8_t )(255 * ((CloudColoredLight& )mProperty).getBrightness() / 100));
  }
  virtual bool IsOn() {
    return ((CloudColoredLight&)mProperty).getSwitch();
  }
};
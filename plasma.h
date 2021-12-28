#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0

// https://github.com/atuline/FastLED-Demos/blob/master/plasma/plasma.ino
class PlasmaEffect : public Effect {
protected:
  CRGBPalette16 CurrentPalette;
  CRGBPalette16 TargetPalette;
  TBlendType Blending = LINEARBLEND;
  unsigned long lastPlasma, lastNewPalette, lastBlend;
  
  virtual void plasma() {
    int thisPhase = beatsin8(6,-64,64);                           // Setting phase change for a couple of waves.
    int thatPhase = beatsin8(7,-64,64);

    for (int k=0; k<Size; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:
      int colorIndex = cubicwave8((k*23)+thisPhase)/2 + cos8((k*15)+thatPhase)/2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
      int thisBright = qsuba(colorIndex, beatsin8(7,0,96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..
      Leds[k] = ColorFromPalette(CurrentPalette, colorIndex, thisBright, Blending);  // Let's now add the foreground colour.
    } 
  } 
  
  virtual void changePalette() {
    TargetPalette = GeneratePalette((CloudColoredLight&)GetProperty(), Palette);
  }

  virtual void blendPalette() {
    nblendPaletteTowardPalette(CurrentPalette, TargetPalette, 24);   // AWESOME palette blending capability.
  }
  
public:
  PlasmaEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudColoredLight& property) 
    : Effect(pin, leds, palette, property)
  {
    CurrentPalette = GeneratePalette((CloudColoredLight&)GetProperty(), palette);
  }
  virtual void Loop() {
    if (millis() > lastPlasma + ChangeEveryMs) {
      lastPlasma = millis();
      plasma();
    }
    if (millis() > lastBlend + ChangeEveryMs*2) {
      lastBlend = millis();
      blendPalette();
    }
    if (millis() > lastNewPalette + ChangeEveryMs*50) {
      lastNewPalette = millis();
      changePalette();
    }
    FastLED.setBrightness((uint8_t )(255 * ((CloudColoredLight& )mProperty).getBrightness() / 100));
  }
  virtual void PropertyChanged() {
    if (IsOn()) {
      lastPlasma = lastBlend = lastNewPalette = millis();
    }
  }
  virtual bool IsOn() {
    return ((CloudColoredLight&)mProperty).getSwitch();
  }
};
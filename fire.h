#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "effect.h"

// based on https://github.com/davepl/DavesGarageLEDSeries/blob/master/LED%20Episode%2010/include/fire.h    
class FireEffect : public Effect
{
protected:
  int Cooling;
  int Sparks;
  int SparkHeight;
  int Sparking;
  bool bReversed;
  bool bMirrored;
  byte* heat;
  unsigned long lastUpdate;

  // When diffusing the fire upwards, these control how much to blend in from the cells below (ie: downward neighbors)
  // You can tune these coefficients to control how quickly and smoothly the fire spreads.  

  static const byte BlendSelf = 2;
  static const byte BlendNeighbor1 = 3;
  static const byte BlendNeighbor2 = 2;
  static const byte BlendNeighbor3 = 1;
  static const byte BlendTotal = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);
  
  virtual void fire() {
    int n = bMirrored ? Size / 2 : Size;
    
    // First cool each cell by a little bit
    for (int i = 0; i < n; i++)
        heat[i] = max(0L, heat[i] - random(0, ((Cooling * 10) / n) + 2));

    // Next drift heat up and diffuse it a little but
    for (int i = 0; i < n; i++)
        heat[i] = (heat[i] * BlendSelf + 
                   heat[(i + 1) % n] * BlendNeighbor1 + 
                   heat[(i + 2) % n] * BlendNeighbor2 + 
                   heat[(i + 3) % n] * BlendNeighbor3) 
                  / BlendTotal;

    // Randomly ignite new sparks down in the flame kernel
    for (int i = 0; i < Sparks; i++)
    {
        if (random(255) < Sparking)
        {
            int y = n - 1 - random(SparkHeight);
            heat[y] = heat[y] + random(160, 255); // This randomly rolls over sometimes of course, and that's essential to the effect
        }
    }

    if (bMirrored) {
      for (int i = 0; i < n; i++) {
        CRGB color = HeatColor(heat[i]);
        int j = bReversed ? (n - 1 - i) : i;
        Leds[j] = color;
        int j2 = !bReversed ? (2 * n - 1 - i) : n + i;
        Leds[j2] = color;
      }      
    }
    else {
      for (int i = 0; i < Size; i++) {
        CRGB color = HeatColor(heat[i]);
        int j = bReversed ? (Size - 1 - i) : i;
        Leds[j] = color;
      }      
    }
  }
public:
    
  // Lower sparking -> more flicker.  Higher sparking -> more consistent flame

  FireEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property, int cooling = 70, int sparking = 60, int sparks = 3, int sparkHeight = 4, bool breversed = true, bool bmirrored = true) 
      : Effect(pin, leds, palette, property),
        Cooling(cooling),
        Sparks(sparks),
        SparkHeight(sparkHeight),
        Sparking(sparking),
        bReversed(breversed),
        bMirrored(bmirrored)
  {
    heat = new byte[Size] { 0 };
  }

  virtual void SetSize(int size) {
    Serial.print("FireEffect SetSize ");
    Serial.println(size);
    Effect::SetSize(size);
    delete [] heat;
    heat = new byte[Size];
    SparkHeight = Size / 2;
    Sparks = Size / 10;
  }
  
  virtual void SetChangeEveryMs(uint16_t ms) {
    Effect::SetChangeEveryMs(ms);
    PropertyChanged();
  }
  
  virtual void Loop()
  {
    if (millis() > lastUpdate + ChangeEveryMs) {
      lastUpdate = millis();
      fire();
      FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
    }
  }
  
  virtual void PropertyChanged() {
    lastUpdate = millis();
  }
  
  virtual bool IsOn() {
    return ((CloudDimmedLight& )mProperty).getSwitch();
  }
  
  virtual ~FireEffect()
  {
    delete [] heat;
  }
};
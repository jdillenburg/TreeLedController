#ifndef Effect_H
#define Effect_H

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <string>

#define MAX_LEDS    300
#define MAX_TASKS   10

CRGB colorToCRGB(Color color); // defined in effect.cpp file
CHSV colorToCHSV(Color color); // defined in effect.cpp file

/**
 * This is the base class for all effects.
 */
class Effect {
  protected:
    CRGBArray<MAX_LEDS>& Leds;
    CRGBPalette256& Palette;
    int Size;
    uint8_t Pin;
    Property& mProperty;
    uint16_t ChangeEveryMs = 50;  // milliseconds between changes
  
    //
    // Creates a random palette from a cloud light.
    //
    static CRGBPalette16 GeneratePaletteFromCloudLight(CloudColoredLight& cloudLight) {
      float h = cloudLight.getValue().hue; // 0.0 to 360.0
      uint8_t baseC = (uint8_t )(255 * h / 360);
      return CRGBPalette16(
               CHSV(baseC + random8(32), 192, random8(128, 255)),
               CHSV(baseC + random8(32), 255, random8(128, 255)),
               CHSV(baseC + random8(32), 192, random8(128, 255)),
               CHSV(baseC + random8(32), 255, random8(128, 255))
             );
    }

    //
    // Creates a random palette from a cloud light and another palette's first 3 colors
    //    Note that palette should be passed Effect.Palette since tha palette is set in the
    //    setPalette() method.
    //
    static CRGBPalette16 GeneratePalette(CloudColoredLight& cloudLight, CRGBPalette256& palette) {
      uint8_t baseH;
      uint8_t baseS;
      uint8_t baseV;
      CHSV hsv1 = rgb2hsv_approximate(palette[0]);
      CHSV hsv2 = rgb2hsv_approximate(palette[1]);
      CHSV hsv3 = rgb2hsv_approximate(palette[2]);
      switch (random8(4)) {
        case 0: 
          baseH = (uint8_t )(255 * cloudLight.getValue().hue / 360);
          baseS = (uint8_t )(255 * cloudLight.getValue().sat / 100);
          baseV = (uint8_t )(255 * cloudLight.getValue().bri / 100);
          break;
        case 1:
          baseH = hsv1.hue;
          baseS = hsv1.sat;
          baseV = hsv1.val;
          break;
        case 2:
          baseH = hsv2.hue;
          baseS = hsv2.sat;
          baseV = hsv2.val;
          break;
        case 3:        
          baseH = hsv3.hue;
          baseS = hsv3.sat;
          baseV = hsv3.val;
          break;
      }
      return CRGBPalette16(
               CHSV(baseH + random8(32), baseS, min(255, baseV + random8(32))),
               CHSV(baseH + random8(32), baseS, min(255, baseV + random8(32))),
               CHSV(baseH + random8(32), baseS, min(255, baseV + random8(32))),
               CHSV(baseH + random8(32), baseS, min(255, baseV + random8(32)))
             );
    }
  public:
    Effect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, Property& property)
      : Pin(pin),
        Leds(leds),
        Palette(palette),
        Size(leds.size()),
        mProperty(property)
    {
    }
    virtual void SetPalette(CRGBPalette256& palette) {
      Palette = palette;
    }
    virtual void SetSize(int size) {
      if (size > Leds.size()) {
        size = Leds.size();
      }
      Size = size;
    }
    virtual void Loop() {
    }
    virtual void PropertyChanged() {
    }
    virtual uint8_t GetPin() {
      return Pin;
    }
    virtual Property& GetProperty() {
      return mProperty;
    }
    virtual bool IsOn() = 0;
    virtual void SetChangeEveryMs(uint16_t ms) {
      ChangeEveryMs = ms;
    }
};

#endif
#ifndef E131Effect_H
#define E131Effect_H
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>
#include "effect.h"
#include "E131.h"

#define E131_DEFAULT_PORT 5568

//
// This effect operns up a UDP multicast port and listens for E13.1 streaming ACN (DMX) color data.
// Only 170 colors can be sent via sACN.
//
class E131Effect : public Effect {
protected:
  bool MulticastStarted;
  int& Universe;
  WiFiUDP Udp;
public:
  E131Effect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property, int& universe) 
    : Effect(pin, leds, palette, property), Universe(universe), MulticastStarted(false)
  {
    
  }
  virtual void Loop() {
    // start up multicast after we connect to Internet
    if (!MulticastStarted && ArduinoCloud.connected() == 1) {
      uint8_t c = ((Universe >> 8) & 0xff);
      uint8_t d = ((Universe >> 0) & 0xff);
      int status = Udp.beginMulticast(
        IPAddress(239, 255, c, d),
        E131_DEFAULT_PORT
      );
      if (status) {
        MulticastStarted = true;
      }
    }
    if (MulticastStarted) {      
      E131 e131(Udp);
      int e131count = e131.parsePacket();
      if (e131count > 0 && e131.universe == Universe) {
        int e131Pixels = e131count / 3;
        for (int i = 0;  i < min(Size, e131Pixels);  i++) {
          CRGB pixel(e131.data[i*3], e131.data[i*3+1], e131.data[i*3+2]);
          Leds[i] = pixel;
        }
        FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
        FastLED.show();
      }
    }
  }
  virtual bool IsOn() {
    return ((CloudDimmedLight&)mProperty).getSwitch();
  }
  void SetUniverse(int& universe) {
    Universe = universe;
    Udp.stop();
    MulticastStarted = false;
  }
};
#endif
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <ArduinoIoTCloud.h>

/*
  Utility functions to converts colors from ARduino cloud format to FastLED format.
  
  See https://github.com/arduino-libraries/ArduinoIoTCloud/blob/master/src/property/types/CloudColor.h
  See http://fastled.io/docs/3.1/struct_c_h_s_v.html
*/
CRGB colorToCRGB(Color color) {
  uint8_t R;
  uint8_t G;
  uint8_t B;
  color.getRGB(R, G, B);
  return CRGB(R, G, B);
}

CHSV colorToCHSV(Color color) {
  return CHSV(
    (uint8_t )((color.hue * 255) / 360), 
    (uint8_t )((color.sat * 255) / 100),
    (uint8_t )((color.bri * 255) / 100)
  );
}
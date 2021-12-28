class RainbowEffect : public Effect {
public:
  RainbowEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property)
    : Effect(pin, leds, palette, property)
  {
  }
  virtual void Loop() {
    // 0 == initial hue, 255 / NUM_LEDS = deltahue
    Leds(0, Size-1).fill_rainbow(0, 255 / Size);
    FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
  }
  virtual bool IsOn() {
    return ((CloudDimmedLight&)mProperty).getSwitch();
  }
};
class PatternEffect : public Effect {
public:
  PatternEffect(uint8_t pin, CRGBArray<MAX_LEDS>& leds, CRGBPalette256& palette, CloudDimmedLight& property) 
    : Effect(pin, leds, palette, property)
  {
  }
  virtual void Loop() {
    for ( int i = 0;  i < Size;  i++) {
      Leds[i] = ColorFromPalette(Palette, i, 255, NOBLEND);
    }
    FastLED.setBrightness((uint8_t )(255 * ((CloudDimmedLight& )mProperty).getBrightness() / 100));
  }
  virtual bool IsOn() {
    return ((CloudDimmedLight&)mProperty).getSwitch();
  }
};
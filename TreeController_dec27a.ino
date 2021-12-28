#include "arduino_secrets.h"

#include "thingProperties.h"
#include <vector>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>
// effect.h has MAX_LEDS and MAX_TASKS settings
#include "effect.h"
#include "rainbow.h"
#include "pattern.h"
#include "colored.h"
#include "fire.h"
#include "glitter.h"
#include "confetti.h"
#include "sweep.h"
#include "plasma.h"
#include "lightning.h"
#include "blend.h"
#include "e131effect.h"

// Setup the type of LEDs being controlled.  See FastLED for LED_TYPE and COLOR_ORDER.  Some supported types are:
//   TM1803, TM1804, TM1809, WS2811, WS2812, WS2812B, NEOPIXEL, UCS1903, UCS1903B, GW6205, GW6205_400, WS2801,
//   SM167816, and LPD8806.
#define LED_TYPE    WS2811
// Setup the order of sending colors to the strip.  You may need to experiment to find the right value as this
//   seems to be up to he manufacturer.
#define COLOR_ORDER BRG
// Colored and dimmed dashboard widget counts.
#define NUM_COLORED_LIGHTS 2
#define NUM_DIMMED_LIGHTS  8
// Arduino PIN used to light up an optionl status LED when IoT connection is made.
#define CLOUD_STATUS_PIN   12

// Define one CRGBArray per pin with light strip connected to it, initialized in setup() below.
CRGBArray<MAX_LEDS> leds2;
CRGBArray<MAX_LEDS> leds3;
CRGBArray<MAX_LEDS> leds4;
CRGBArray<MAX_LEDS> leds5;

// Should use fixed sized array but this vector makes setting up the effects easier.
std::vector<Effect*> effects;
// Keep a pointer to the E131Effect so we can change the Universe in onUniverseChange() below.
E131Effect *e131Effect;

// Too bad CloudColoredLight and CloudDimmedLight do not share a common base class with getSwitch() in it.
//   Need to treat them separately because of this oversight.
CloudColoredLight *coloredLights[NUM_COLORED_LIGHTS] = {
  &tree_plasma,
  &tree_color
};
CloudDimmedLight *dimmedLights[NUM_DIMMED_LIGHTS] = {
  &tree_sweep,
  &tree_fire,
  &tree_pattern,
  &tree_confetti,
  &tree_lightning,
  &tree_blend,
  &tree_rainbow,
  &tree_sacn
};
// Colors defined on dashboard get copied to this palette as palette[0], palette[1] and palette[2] (repeated 256/3 times)
CRGBPalette256 palette;
// Track last cloud connction status so we're not updating printing Serial debug statements every time loop() is called.
int lastCloudStatus = 1;


//
// setup() - called by Arduino when Nano is turned on or reset.
//
void setup() {
  Serial.begin(9600);
  delay(1500);
  // Set cloud status LED to OUTPUT, set to HIGH or LOW in loop() below.
  pinMode(CLOUD_STATUS_PIN, OUTPUT);
  // Defined in thingProperties.h
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  //
  // Setup the pins that are connected to the LEDs here.  We use pins 2 through 5.
  //
  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds2, MAX_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds3, MAX_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds4, MAX_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(leds5, MAX_LEDS).setCorrection( TypicalLEDStrip );
  //
  // Add the effects.  We create an instance for each PIN so random effects will be applied 
  //    to each pin separately and the LEDs will look different.  Each effect gets a
  //    cloud property that controls its color, dimming, and on/off.
  //
  effects.push_back(new FireEffect(2, leds2, palette, tree_fire));
  effects.push_back(new FireEffect(3, leds3, palette, tree_fire));
  effects.push_back(new FireEffect(4, leds4, palette, tree_fire));
  effects.push_back(new FireEffect(5, leds5, palette, tree_fire));
  effects.push_back(new RainbowEffect(2, leds2, palette, tree_rainbow));
  effects.push_back(new RainbowEffect(3, leds3, palette, tree_rainbow));
  effects.push_back(new RainbowEffect(4, leds4, palette, tree_rainbow));
  effects.push_back(new RainbowEffect(5, leds5, palette, tree_rainbow));
  effects.push_back(new PatternEffect(2, leds2, palette, tree_pattern));
  effects.push_back(new PatternEffect(3, leds3, palette, tree_pattern));
  effects.push_back(new PatternEffect(4, leds4, palette, tree_pattern));
  effects.push_back(new PatternEffect(5, leds5, palette, tree_pattern));
  effects.push_back(new ConfettiEffect(2, leds2, palette, tree_confetti));
  effects.push_back(new ConfettiEffect(3, leds3, palette, tree_confetti));
  effects.push_back(new ConfettiEffect(4, leds4, palette, tree_confetti));
  effects.push_back(new ConfettiEffect(5, leds5, palette, tree_confetti));
  effects.push_back(new SweepEffect(2, leds2, palette, tree_sweep, bpm));
  effects.push_back(new SweepEffect(3, leds3, palette, tree_sweep, bpm));
  effects.push_back(new SweepEffect(4, leds4, palette, tree_sweep, bpm));
  effects.push_back(new SweepEffect(5, leds5, palette, tree_sweep, bpm));
  effects.push_back(new PlasmaEffect(2, leds2, palette, tree_plasma));
  effects.push_back(new PlasmaEffect(3, leds3, palette, tree_plasma));
  effects.push_back(new PlasmaEffect(4, leds4, palette, tree_plasma));
  effects.push_back(new PlasmaEffect(5, leds5, palette, tree_plasma));
  effects.push_back(new ColoredEffect(2, leds2, palette, tree_color));
  effects.push_back(new ColoredEffect(3, leds3, palette, tree_color));
  effects.push_back(new ColoredEffect(4, leds4, palette, tree_color));
  effects.push_back(new ColoredEffect(5, leds5, palette, tree_color));
  effects.push_back(new LightningEffect(2, leds2, palette, tree_lightning));
  effects.push_back(new LightningEffect(3, leds3, palette, tree_lightning));
  effects.push_back(new LightningEffect(4, leds4, palette, tree_lightning));
  effects.push_back(new LightningEffect(5, leds5, palette, tree_lightning));
  effects.push_back(new BlendEffect(2, leds2, palette, tree_blend));
  effects.push_back(new BlendEffect(3, leds3, palette, tree_blend));
  effects.push_back(new BlendEffect(4, leds4, palette, tree_blend));
  effects.push_back(new BlendEffect(5, leds5, palette, tree_blend));
  // E13.1 sACN effect only applied to one pin because it opens a socket and only one socket is allowed on Nano 33 IoT
  e131Effect = new E131Effect(2, leds2, palette, tree_sacn, universe);
  effects.push_back(e131Effect);
  // overlay effects must go last
  effects.push_back(new GlitterEffect(2, leds2, palette, tree_glitter));
  effects.push_back(new GlitterEffect(3, leds3, palette, tree_glitter));
  effects.push_back(new GlitterEffect(4, leds4, palette, tree_glitter));
  effects.push_back(new GlitterEffect(5, leds5, palette, tree_glitter));
  Serial.print("setup complete with ");
  Serial.print(effects.size());
  Serial.println(" effects on pins 2 through 5");
  runningTime = 0;
}

//
// loop() - Arduino calls loop as often as possible.  We monitor our Internet connection and update the 
//    status LED once we connect to the Arduino IoT cloud.  We also loop through all the effects and
//    call their loop methods.
//
void loop() {
  EVERY_N_MILLIS(500) {
    ArduinoCloud.update();
    int newStatus = ArduinoCloud.connected();
    // every 1/2 second we check connection to cloud and turn LED ON if we're connected
    if (newStatus != lastCloudStatus) {
      if (newStatus == 1) {
        digitalWrite(CLOUD_STATUS_PIN, HIGH);    
        Serial.println("Set pin 12 high");
        Serial.println(WiFi.localIP());
      }
      else {
        digitalWrite(CLOUD_STATUS_PIN, LOW);
        Serial.println("Set pin 12 low");
      }
    }
    lastCloudStatus = newStatus;
  }
  // We keep track of whether all the effects are OFF.  If so, then we make sure FastLED.clear() is called to
  //   clear the LEDs from any animations left over frm the effects.
  bool oneOn = false;
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->IsOn()) {
      effects[i]->Loop();
      oneOn = true;
    }
  }
  if (!oneOn) {
    FastLED.clear();
  }
  FastLED.show();
  // Update the runningTime so we can tell via the Dashboard whether this device is connected to the dashboard or not.
  runningTime = millis()/1000;
}

void onColor1Change() {
  Serial.println("onColor1Change");
  updatePalette();
}

void onColor2Change() {
  Serial.println("onColor2Change");
  updatePalette();
}

void onColor3Change() {
  Serial.println("onColor3Change");
  updatePalette();
}

void updatePalette() {
  CRGB c1 = colorToCRGB(color1.getValue());
  CRGB c2 = colorToCRGB(color2.getValue());
  CRGB c3 = colorToCRGB(color3.getValue());
  for (int i = 0;  i < 256;  i++) {
    int pi = (i % 3);
    if (pi == 0) {
      palette[i] = c1;
    }
    else if (pi == 1) {
      palette[i] = c2;
    }
    else if (pi == 2) {
      palette[i] = c3;
    }
  }
  for (int i = 0;  i < effects.size();  i++) {
    effects[i]->SetPalette(palette);
  }  
}

void onRotateSpeedChange() {
  Serial.print("onRotateSpeedChange ");
  Serial.println(rotateSpeed);
  if (rotateSpeed < 1) {
    rotateSpeed = 1;
  }
  else if (rotateSpeed > 10000) {
    rotateSpeed = 10000;
  }
  for (int i = 0;  i < effects.size();  i++) {
    effects[i]->SetChangeEveryMs(10000/rotateSpeed);
  }
}

void onNumLedsOnPin2Change() {
  Serial.print("onNumLedsOnPin2Change ");
  Serial.println(numLedsOnPin2);
  if (numLedsOnPin2 < 1) {
    numLedsOnPin2 = 1;
  }
  if (numLedsOnPin2 > MAX_LEDS) {
    numLedsOnPin2 = MAX_LEDS;
  }
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->GetPin() == 2) {
      effects[i]->SetSize(numLedsOnPin2);
    }
  }
  FastLED.clear();
}

void onNumLedsOnPin3Change() {
  Serial.print("onNumLedsOnPin3Change ");
  Serial.println(numLedsOnPin3);
  if (numLedsOnPin3 < 1) {
    numLedsOnPin3 = 1;
  }
  if (numLedsOnPin3 > MAX_LEDS) {
    numLedsOnPin3 = MAX_LEDS;
  }
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->GetPin() == 3) {
      effects[i]->SetSize(numLedsOnPin3);
    }
  }
  FastLED.clear();
}

void turnOffAllExcept(Property* p) {
  for (int i = 0;  i < NUM_COLORED_LIGHTS; i++) {
    if (coloredLights[i] != p) {
      if (coloredLights[i]->getSwitch()) {
        coloredLights[i]->setSwitch(false);
        triggerPropertyChanged(coloredLights[i]);
      }
    }
  }
  for (int j = 0;  j < NUM_DIMMED_LIGHTS;  j++) {
    if (dimmedLights[j] != p) {
      if (dimmedLights[j]->getSwitch()) {
        dimmedLights[j]->setSwitch(false);
        triggerPropertyChanged(dimmedLights[j]);
      }
    }
  }
}

void triggerPropertyChanged(Property *p) {
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->GetProperty() == *p) {
      effects[i]->PropertyChanged();
    }
  }
}

void onTreeFireChange() {
  Serial.print("ontreeFireChange ");
  Serial.println(tree_fire.getSwitch());
  turnOffAllExcept(&tree_fire);
  triggerPropertyChanged(&tree_fire);
}

void onTreeRainbowChange() {
  Serial.print("ontreeRainbowChange ");
  Serial.println(tree_rainbow.getSwitch());
  turnOffAllExcept(&tree_rainbow);
  triggerPropertyChanged(&tree_rainbow);
}

void onTreePatternChange() {
  Serial.print("ontreePatternChange ");
  Serial.println(tree_pattern.getSwitch());
  turnOffAllExcept(&tree_pattern);
  triggerPropertyChanged(&tree_pattern);
}

void onTreeColorChange() {
  Serial.print("ontreeColorChange ");
  Serial.print(tree_color.getSwitch());
  Serial.print(" ");
  Serial.println(tree_color.getBrightness());
  turnOffAllExcept(&tree_color);
  triggerPropertyChanged(&tree_color);
}

void onTreeGlitterChange() {
  Serial.print("ontreeGlitterChange ");
  Serial.println(tree_glitter);
  // glitter works with all effects except fire, confetti, plasma, and sweep so we turn those off
  if (tree_glitter) {
    tree_fire.setSwitch(false);
    triggerPropertyChanged(&tree_fire);
    tree_confetti.setSwitch(false);
    triggerPropertyChanged(&tree_confetti);
    tree_sweep.setSwitch(false);
    triggerPropertyChanged(&tree_sweep);
    tree_plasma.setSwitch(false);
    triggerPropertyChanged(&tree_plasma);
    tree_lightning.setSwitch(false);
    triggerPropertyChanged(&tree_lightning);
  }
  triggerPropertyChanged(&tree_glitter);
}

void onTreeConfettiChange() {
  Serial.print("ontreeConfettiChange ");
  Serial.print(tree_confetti.getSwitch());
  Serial.print(" ");
  Serial.println(tree_confetti.getBrightness());
  turnOffAllExcept(&tree_confetti);
  triggerPropertyChanged(&tree_confetti);
}

void onTreeSweepChange() {
  Serial.print("ontreeSweepChange ");
  Serial.print(tree_sweep.getSwitch());
  Serial.print(" ");
  Serial.println(tree_sweep.getBrightness());
  turnOffAllExcept(&tree_sweep);
  triggerPropertyChanged(&tree_sweep);
}

void onBpmChange() {
  Serial.println("onBpmChange");
  if (bpm < 1) {
    bpm = 1;
  }
}

void onTreePlasmaChange() {
  Serial.print("ontreePlasmaChange ");
  Serial.print(tree_plasma.getSwitch());
  Serial.print(" ");
  Serial.println(tree_plasma.getBrightness());
  turnOffAllExcept(&tree_plasma);
  triggerPropertyChanged(&tree_plasma);
}


void onTreeLightningChange() {
  Serial.print("ontreeLightningChange ");
  Serial.print(tree_lightning.getSwitch());
  Serial.print(" ");
  Serial.println(tree_lightning.getBrightness());
  turnOffAllExcept(&tree_lightning);
  triggerPropertyChanged(&tree_lightning);
}


void onTreeBlendChange() {
  Serial.print("ontreeBlendChange ");
  Serial.print(tree_blend.getSwitch());
  Serial.print(" ");
  Serial.println(tree_blend.getBrightness());
  turnOffAllExcept(&tree_blend);
  triggerPropertyChanged(&tree_blend);
}

/*
  Since NumLedsOnPin4 is READ_WRITE variable, onNumLedsOnPin4Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNumLedsOnPin4Change()  {
  Serial.print("onNumLedsOnPin4Change ");
  Serial.println(numLedsOnPin4);
  if (numLedsOnPin4 < 1) {
    numLedsOnPin4 = 1;
  }
  if (numLedsOnPin4 > MAX_LEDS) {
    numLedsOnPin4 = MAX_LEDS;
  }
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->GetPin() == 4) {
      effects[i]->SetSize(numLedsOnPin4);
    }
  }
  FastLED.clear();
}

/*
  Since NumLedsOnPin5 is READ_WRITE variable, onNumLedsOnPin5Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onNumLedsOnPin5Change()  {
  Serial.print("onNumLedsOnPin5Change ");
  Serial.println(numLedsOnPin5);
  if (numLedsOnPin5 < 1) {
    numLedsOnPin5 = 1;
  }
  if (numLedsOnPin5 > MAX_LEDS) {
    numLedsOnPin5 = MAX_LEDS;
  }
  for (int i = 0;  i < effects.size();  i++) {
    if (effects[i]->GetPin() == 5) {
      effects[i]->SetSize(numLedsOnPin5);
    }
  }
  FastLED.clear();
}

/*
  Since TreeSacn is READ_WRITE variable, onTreeSacnChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTreeSacnChange()  {
  Serial.print("ontreeSacnChange ");
  Serial.print(tree_sacn.getSwitch());
  Serial.print(" ");
  Serial.println(tree_sacn.getBrightness());
  turnOffAllExcept(&tree_sacn);
  triggerPropertyChanged(&tree_sacn);
}

/*
  Since Universe is READ_WRITE variable, onUniverseChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onUniverseChange()  {
  e131Effect->SetUniverse(universe);
}
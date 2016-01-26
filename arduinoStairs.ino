//ArduinoStairs
//
//Written by: Chris Parent
//Date:October 1, 2014
//
//Uses an Arduino, 2 PIR sensors and set of LED lights.
//This code was written using WS2801 strand of 50 lights
//
//Uses FastLED library to talk to LEDS.
//https://github.com/FastLED/FastLED
#include "FastLED.h"

int PIR1InputPin = 2;
int PIR2InputPin = 3;
int val1 = 0;
int val2 = 0;

#define POTENTIOMETER_PIN A0;

//Tell it how many leds are in the strip.
#define TOTAL_LEDS 13
//if you want to turn them on and off as a unit.
#define LEDS_IN_A_GROUP 1
//color you want to use when the LED is on
//static const uint32_t onColor = 0xF7F76F;
static const uint32_t onColor = CRGB::AliceBlue;
static const uint32_t leadColor = CRGB::DeepPink;
//milliseconds led is off when dark led cycling
#define offChaseDelay 1
//milliseconds between turning next led on or off
#define onAndOffDelay 20
#define PULSEDELAY 1
#define FADEONDELAY 100
//number of times to cycle the entire set with a dark led cycle
#define cycleOffChaseTimes 60

#define MAX_BRIGHTNESS 255

CRGB leds[TOTAL_LEDS];
int colors[TOTAL_LEDS];
//Which pins in your arduino to talk to the LED strip
#define DATA_PIN 4  //white
#define DATA_PIN2 5  //white
//#define CLOCK_PIN 8 //green
//blue ground
//red 5v
int downUp = 0;              // variable to rememer the direction of travel up or down the stairs
unsigned long timeOut = 60000; // timestamp to remember when the PIR was triggered.
int alarmValueTop = LOW;    // Variable to hold the PIR status
int alarmValueBottom = LOW; // Variable to hold the PIR status

void setup() {
    Serial.begin(9600);
    pinMode(PIR1InputPin, INPUT);
    pinMode(PIR2InputPin, INPUT);

    addFastLEDs();
    FastLED.clear();
    colorChase(CRGB::Black, CRGB::Black, onAndOffDelay, true);
}

void loop() {
    checkPIR();
}

void addFastLEDs() {
    // Uncomment one of the following lines for your leds arrangement.
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    FastLED.addLeds<WS2811, DATA_PIN2, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2801, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<SM16716, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<LPD8806, RGB>(leds, TOTAL_LEDS);
    //FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);
}

void checkPIR() {
    alarmValueTop = digitalRead(PIR1InputPin);
    alarmValueBottom = digitalRead(PIR2InputPin);

    if (alarmValueTop == HIGH && downUp != 2) {
        //    Serial.println("CHECKING PIRS");
        downUp = 1;
        Serial.println("UP");
        runCycle(onColor, true);
    }
    else if (alarmValueBottom == HIGH && downUp != 1) {
        downUp = 2;
        Serial.println("DOWN");
        runCycle(onColor, false);
    }
    else {
        downUp = 0;
    }
}

void runCycle(uint32_t onColor, boolean forward) {
    timeOut = millis();
    FastLED.clear();
    FastLED.setBrightness(MAX_BRIGHTNESS);

    //turn strip on
    fadeAllOn(onColor, forward);

    //chase with black to give pulse effect
    for (int i = 0; i < cycleOffChaseTimes; i++) {
        colorChase(onColor, onColor, offChaseDelay, forward);
    }

    //turn strip off
    colorChase(CRGB::Black, CRGB::Black, onAndOffDelay, !forward);
}

void colorChase(uint32_t onColor, uint32_t offColor, uint8_t wait, boolean forward) {
    int stepBy = 5;

    if (forward) {
        for (int ledNumber = 0; ledNumber < TOTAL_LEDS; ledNumber += LEDS_IN_A_GROUP) {
            for (int faderAmount = 255; faderAmount >= 0; faderAmount -= stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
            for (int faderAmount = 0; faderAmount <= 255; faderAmount += stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
        }
    } else {
        for (int ledNumber = TOTAL_LEDS; ledNumber >= 0; ledNumber -= LEDS_IN_A_GROUP) {
            for (int faderAmount = 255; faderAmount >= 0; faderAmount -= stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
            for (int faderAmount = 0; faderAmount <= 255; faderAmount += stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
        }
    }
}

void fadeAllOn(uint32_t color, boolean forward) {
    int thisLedBrightness;

    int maxStep = 256;

    for (int currentStep = 0; currentStep < maxStep; currentStep++) {
        for (int thisLed = 0; thisLed < TOTAL_LEDS; thisLed++) {
            int ledToChange = thisLed;
            if (!forward) {
                ledToChange = TOTAL_LEDS - 1 - thisLed;
            }
            leds[ledToChange] = color;
            int brightnessToUse = currentStep - (thisLed / TOTAL_LEDS) * currentStep;
            if (brightnessToUse < 0) {
                brightnessToUse = 0;
            }
            else if (brightnessToUse > MAX_BRIGHTNESS) {
                brightnessToUse = MAX_BRIGHTNESS;
            }
            leds[ledToChange] %= brightnessToUse;
        }
        FastLED.show();
        delay(FADEONDELAY);
    }
}

void showGroup(int ledNumber, uint32_t color, int faderPercent) {
    leds[ledNumber] = color;
    leds[ledNumber] %= faderPercent;
    FastLED.show();
    delay(PULSEDELAY);
}

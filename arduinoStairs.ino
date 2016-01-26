//ArduinoStairs
//
//Written by: Chris Parent
//Date:October 1, 2014
//
//Uses an Arduino, 2 PIR sensors and set of LED lights.
//This code was written using WS2811 strand of 13 X 2 LED lights
//
//Uses FastLED library to talk to LEDS.
//https://github.com/FastLED/FastLED
#include "FastLED.h"

int PIR1InputPin = 2;
int PIR2InputPin = 3;
#define DATA_PIN 4
#define DATA_PIN2 5
#define TOTAL_LEDS 13

static const uint32_t onColor = CRGB::AliceBlue;
#define COLORCHASESPEED 1
#define PULSEDELAY 1
#define FADEONSPEED 100
#define NUMBER_OF_TIMES_TO_CYCLE_PULSE 60
//#define CLOCK_PIN 8 //green
#define POTENTIOMETER_PIN A0;

#define MAX_BRIGHTNESS 255
#define NO_DELAY 0
#define GOING_UP 1
#define GOING_DOWN 2
#define NOT_GOING_UP_OR_DOWN 0
CRGB leds[TOTAL_LEDS];
int alarmValueTop = LOW;
int alarmValueBottom = LOW;

void setLedWithFadedBrightness(const uint32_t &color, int currentStep, int thisLed, int ledToChange);

void setFadingColorsForStrip(const uint32_t &color, const boolean &forward, int currentStep);

void setup() {
    Serial.begin(9600);
    pinMode(PIR1InputPin, INPUT);
    pinMode(PIR2InputPin, INPUT);

    addFastLEDs();
    FastLED.clear();
    colorChase(CRGB::Black, CRGB::Black, NO_DELAY, true);
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
    int goingDownOrUp = NOT_GOING_UP_OR_DOWN;

    alarmValueTop = digitalRead(PIR1InputPin);
    alarmValueBottom = digitalRead(PIR2InputPin);

    if (alarmValueTop == HIGH && goingDownOrUp != GOING_DOWN) {
        goingDownOrUp = GOING_UP;
        runCycle(onColor, true);
    }
    else if (alarmValueBottom == HIGH && goingDownOrUp != GOING_UP) {
        goingDownOrUp = GOING_DOWN;
        runCycle(onColor, false);
    }
    else {
        goingDownOrUp = NOT_GOING_UP_OR_DOWN;
    }
}

void runCycle(uint32_t onColor, boolean isGoingUp) {
    FastLED.clear();
    FastLED.setBrightness(MAX_BRIGHTNESS);

    //turn strip on fading leds further away from the PIR
    fadeAllOn(onColor, isGoingUp);

    //chase with black to give chase pulse effect
    for (int i = 0; i < NUMBER_OF_TIMES_TO_CYCLE_PULSE; i++) {
        colorChase(onColor, onColor, COLORCHASESPEED, isGoingUp);
    }

    //turn strip off
    colorChase(CRGB::Black, CRGB::Black, COLORCHASESPEED, !isGoingUp);
}

void colorChase(uint32_t onColor, uint32_t offColor, uint8_t wait, boolean isGoingUp) {
    int stepBy = 5;

    if (isGoingUp) {
        for (int ledNumber = 0; ledNumber < TOTAL_LEDS; ledNumber += 1) {
            for (int faderAmount = MAX_BRIGHTNESS; faderAmount >= 0; faderAmount -= stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
            for (int faderAmount = 0; faderAmount <= MAX_BRIGHTNESS; faderAmount += stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
        }
    } else {
        for (int ledNumber = TOTAL_LEDS; ledNumber >= 0; ledNumber -= 1) {
            for (int faderAmount = MAX_BRIGHTNESS; faderAmount >= 0; faderAmount -= stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
            for (int faderAmount = 0; faderAmount <= MAX_BRIGHTNESS; faderAmount += stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
        }
    }
}

void fadeAllOn(uint32_t color, boolean forward) {
    int thisLedBrightness;

    int maxStep = 256;

    for (int currentStep = 0; currentStep < maxStep; currentStep++) {
        setFadingColorsForStrip(color, forward, currentStep);
        FastLED.show();
        delay(FADEONSPEED);
    }
}

void setFadingColorsForStrip(const uint32_t &color, const boolean &forward, int currentStep) {
    for (int thisLed = 0; thisLed < TOTAL_LEDS; thisLed++) {
        int ledToChange = thisLed;
        if (!forward) {
            ledToChange = TOTAL_LEDS - 1 - thisLed;
        }
        setLedWithFadedBrightness(color, currentStep, thisLed, ledToChange);
    }
}

void setLedWithFadedBrightness(const uint32_t &color, int currentStep, int thisLed, int ledToChange) {
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

void showGroup(int ledNumber, uint32_t color, int faderPercent) {
    leds[ledNumber] = color;
    leds[ledNumber] %= faderPercent;
    FastLED.show();
    delay(PULSEDELAY);
}

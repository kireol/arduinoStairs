//https://github.com/FastLED/FastLED

#include "FastLED.h"

//Tell it how many leds are in the strip.
#define TOTAL_LEDS 50
//if you want to turn them on and off as a unit.
#define LEDS_IN_A_GROUP 2
static const uint32_t onColor = CRGB::Blue;
#define cycleOffChaseTimes 10
#define MAX_BRIGHTNESS 255

CRGB leds[TOTAL_LEDS];

//Which pins in your arduino to talk to the LED strip
#define DATA_PIN 6
#define CLOCK_PIN 8


void setup() {
    // Uncomment one of the following lines for your leds arrangement.
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<WS2801, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<SM16716, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<LPD8806, RGB>(leds, TOTAL_LEDS);

    FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);

    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, TOTAL_LEDS);
}

void loop() {
    FastLED.clear();
    FastLED.setBrightness(MAX_BRIGHTNESS);
    runCycle(onColor, false);
}

void runCycle(uint32_t onColor, boolean forward) {
    int onAndOffDelay = 50;
    int offChaseDelay = 20;
    int increment256Amount = 5;

    colorChase(onColor, onColor, onAndOffDelay, forward, increment256Amount);
    for (int i = 0; i < cycleOffChaseTimes; i++) {
        colorChase(CRGB::Black, onColor, offChaseDelay, forward, 256);
    }
    colorChase(CRGB::Black, CRGB::Black, onAndOffDelay, !forward, increment256Amount);
}

void showGroup(int setsLowestLedNumber, uint32_t color, int faderPercent) {
    for (int group = 0; group < LEDS_IN_A_GROUP; group++) {
        if (setsLowestLedNumber + group >= 0 && setsLowestLedNumber <= TOTAL_LEDS - 1) {
            leds[setsLowestLedNumber + group] = color;
            leds[setsLowestLedNumber + group] %= faderPercent;
        }
    }
    FastLED.show();
}

void colorChase(uint32_t onColor, uint32_t offColor, uint8_t wait, boolean forward, int stepBy) {
    if (forward) {
        for (int ledNumber = 0; ledNumber < TOTAL_LEDS; ledNumber += LEDS_IN_A_GROUP) {
            for (int faderAmount = 0; faderAmount <= 255; faderAmount += stepBy) {
                showGroup(ledNumber, onColor, faderAmount);
            }
            delay(wait);
            if (onColor != offColor) {
                showGroup(ledNumber, offColor, 100);
            }
        }
    } else {
        for (int led_number = TOTAL_LEDS - 1; led_number >= 0; led_number -= LEDS_IN_A_GROUP) {
            for (int faderAmount = 0; faderAmount <= 255; faderAmount += stepBy) {
                showGroup(led_number, onColor, faderAmount);
            }
            delay(wait);
            if (onColor != offColor) {
                showGroup(led_number, offColor, 100);
            }
        }
    }
    return;
}

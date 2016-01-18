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
int previousPir1State = LOW;

int PIR2InputPin = 3;
int previousPir2State = LOW;

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
#define offChaseDelay 50
//milliseconds between turning next led on or off
#define onAndOffDelay 75

//number of times to cycle the entire set with a dark led cycle
#define cycleOffChaseTimes 20

#define MAX_BRIGHTNESS 255

CRGB leds[TOTAL_LEDS];

//Which pins in your arduino to talk to the LED strip
#define DATA_PIN 4  //white
#define DATA_PIN2 5  //white
//#define CLOCK_PIN 8 //green
//blue ground
//red 5v


void setup() {
    Serial.begin(9600);
    pinMode(PIR1InputPin, INPUT);
    pinMode(PIR2InputPin, INPUT);
    
    
    addFastLEDs();
    FastLED.clear();

}

void loop() {
    int sensorValue = analogRead(A0);
    Serial.println(sensorValue);

  
    val1 = digitalRead(PIR1InputPin);
    Serial.println("Checking PIR1");
    previousPir1State = checkPIR(val1, previousPir1State, true);
    val2 = digitalRead(PIR2InputPin);
    Serial.println("Checking PIR2");
    previousPir2State = checkPIR(val2, previousPir2State, false);
}

void addFastLEDs(){
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

int checkPIR(int pirCurrentValue, int pirPreviousState, int runForward){
    Serial.println("Checking PIR");
    if (pirCurrentValue == HIGH) {
        Serial.println("Motion detected");
        if (pirPreviousState == LOW) {
            Serial.println("LEDs started");
            FastLED.clear();
            FastLED.setBrightness(MAX_BRIGHTNESS);
            runCycle(onColor, runForward);
            pirPreviousState = HIGH;
        }
    } else {
        if (pirPreviousState == HIGH){
            Serial.println("Motion ended!");
            pirPreviousState = LOW;
        }
    }
    return pirPreviousState;
}
void runCycle(uint32_t onColor, boolean forward) {
    int increment256Amount = 5;
    //turn strip on
    colorChase(onColor, onColor, onAndOffDelay, forward, increment256Amount);

    //chase with black to give pulse effect
    for (int i = 0; i < cycleOffChaseTimes; i++) {
        colorChase(CRGB::Black, onColor, offChaseDelay, forward, MAX_BRIGHTNESS);
    }

    //turn strip off
    colorChase(CRGB::Black, CRGB::Black, onAndOffDelay, !forward, increment256Amount);
    resetPIRStates();
}

void resetPIRStates(){
    previousPir1State = LOW;
    previousPir2State = LOW;
}

void showGroup(int setsLowestLedNumber, uint32_t color, int faderPercent) {
    for (int group = 0; group < LEDS_IN_A_GROUP; group++) {
        if (setsLowestLedNumber + group >= 0 && setsLowestLedNumber  + group <= TOTAL_LEDS - 1) {
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
                if(onColor == CRGB::Black){
                  showGroup(ledNumber, onColor, faderAmount);                                    
                }else{
                  showGroup(ledNumber, leadColor, faderAmount);
                  showGroup(ledNumber-1, onColor, faderAmount);                  
                }
            }
            delay(wait);
            if (onColor != offColor) {
                showGroup(ledNumber, offColor, MAX_BRIGHTNESS);
            }
        }
    } else {
        for (int led_number = TOTAL_LEDS ; led_number >= 0; led_number -= LEDS_IN_A_GROUP) {
            for (int faderAmount = 0; faderAmount <= 255; faderAmount += stepBy) {
                if(onColor == CRGB::Black){
                  showGroup(led_number, onColor, faderAmount);                  
                }else{
                  showGroup(led_number, leadColor, faderAmount);
                  showGroup(led_number+1, onColor, faderAmount);
                }
            }
            delay(wait);
            if (onColor != offColor) {
                showGroup(led_number, offColor, MAX_BRIGHTNESS);
            }
        }
    }
    return;
}

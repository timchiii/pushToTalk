/*
    Wiring Diagram
    GND -> 10kR 
    GND -> LED-
    10kR -> PIN 4
    PIN 5 -> button bottom
    VCC -> button side
    VCC -> LED+
    PIN 5 -> LED Data

*/


#include "Keyboard.h"
#include "HardwareSerial.h"
#include <FastLED.h>

#define NUM_LEDS 24
#define DATA_PIN 5
#define BRIGHTNESS 200

int button = 4;
int lastState = HIGH;
bool wasMuted = 1;

CRGB leds[NUM_LEDS];

void setup()
{
    pinMode(button, INPUT);
    pinMode(DATA_PIN, OUTPUT);
    Keyboard.begin();
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    fill_solid(leds, NUM_LEDS, CRGB::Purple);
    FastLED.show();
}

// LOW = pressed
// HIGH = not pressed

void loop()
{

    int nowState = digitalRead(button);
    //Serial.println(String(lastState) + String(nowState));

    /* if button state has changed send
       CTRL-SHIFT-m to toggle mute */
    if (lastState != nowState)
    {
        if (nowState == HIGH)
        {
            fill_solid(leds, NUM_LEDS, CRGB::Purple);
            FastLED.show();
            wasMuted = 0;
            Serial.println(nowState);
        }

        if (nowState == LOW)
        {
            fill_solid(leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
            wasMuted = 1;
            Serial.println(nowState);
        }

        //toggle mute
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('m');
        
        // pause briefly
        delay(100);

        // release all keys
        Keyboard.releaseAll();
    }

    // set lastState to current state
    lastState = nowState;
}
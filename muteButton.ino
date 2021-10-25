/*
    Wiring Diagram
    GND -> 10kR 
    GND -> LED-
    10kR -> PIN 4
    PIN 4 -> button bottom
    VCC -> button side
    VCC -> LED+
    PIN 5 -> LED Data

*/

#include "Keyboard.h"
#include "HardwareSerial.h"
#include <FastLED.h>

#define ledCount 24 // number of leds
#define dataPin 5   // led ring data pin
#define buttonPin 4 // pin for button
//#define rgbTimeout 7000   // test timeout at 2 seconds
#define rgbTimeout 3600000 // prod timeout shoudl be 1 hour
#define speakDelay 500     // ms to hold button before engaging speaking mode
#define debounceTimeout 15 // delay each loop by this to debounce button input
#define modeChangeTaps 3   // number of taps to change modes
#define maxTapInterval 250 // max interval between taps

int breathBrightness = 0; // global integer for tracking color in idle mode
int lastState = LOW;      // global for last state of button
int meetingMode = 0;      // 0 = teams, 1 = zoom
int modePresses = 0;      // track number of clicks to change modes
int mode = 0;
int lastFlickerableState; // 0 = idle, 1 = holding
int lastSteadyState;
long holdTime = 0;
long pressStartTime;
long lastTapTime;
long idleStart; // idle start time in millis()
long lastDebounceTime;

CRGB leds[ledCount]; // initialize LED strip

void setup()
{
    pinMode(buttonPin, INPUT);                              // set button pin to input
    pinMode(dataPin, OUTPUT);                               // set led ring pin to output
    Keyboard.begin();                                       // initialize keyboard emmulation
    Serial.begin(115200);                                   // begin serial output
    FastLED.addLeds<WS2812B, dataPin, GRB>(leds, ledCount); // initialize FastLED library
    delay(1000);
    fill_solid(leds, ledCount, CRGB::Purple); // fill led ring purple
    FastLED.show();                           // show led changes
    FastLED.delay(2000);
    fill_solid(leds, ledCount, CRGB::Red); // fill led ring purple
    FastLED.show();                        // show led changes
    FastLED.delay(20);                     // pause 2 seconds
    idleStart = millis();
    pressStartTime = millis();
}

// LOW = pressed
// HIGH = not pressed

void loop()
{
    delay(debounceTimeout);
    int nowState = digitalRead(buttonPin); // read button state
    
    if (lastState == LOW && nowState == HIGH)
    {                              //new button press
        pressStartTime = millis(); // reset last press time
    }

    if (nowState == HIGH)
    {

        holdTime = millis() - pressStartTime;

        if (mode == 0 && holdTime > speakDelay) // not a quick tap, switch to speaking mode
        {
            mode = 1;     // switch to speaking mode
            toggleMute(); // send keystrokes to toggle mute

            fill_solid(leds, ledCount, CRGB::Purple);
            FastLED.show();
            idleStart = millis();
        }
    }

    if (nowState == LOW && lastState == HIGH) // if new button release
    {
        holdTime = millis() - pressStartTime;
        int tapInterval = millis() - lastTapTime;
        //Serial.println("button release, held for:" + String(holdTime) + ". pressStartTime:" + String(pressStartTime) + " / tapInterval:" + String(tapInterval) + " / modePresses:" + String(modePresses));

        idleStart = millis();

        if (mode == 0 && holdTime < speakDelay) // not in speaking mode and a quick tap, increment modePresses
        {
            if (tapInterval < maxTapInterval)
            { // if tap interval is recent, increment taps

                modePresses++;
                //Serial.println("incrementing mode presses to:" + String(modePresses));
                if (modePresses >= modeChangeTaps) // mode presses exceed threshold, change mode
                {
                    handleModeChange(); // handle mode change as this is the 3rd tap
                }
            }
            else
            { // if there wasn't a recent tap, start the tap count over
                modePresses = 1;
            }
            lastTapTime = millis();
        }
        else
        {
            modePresses = 0;
        }

        if (mode == 1) // speaking mode is engaged and button has been release
        {
            toggleMute();                          // mute mic
            fill_solid(leds, ledCount, CRGB::Red); // fill ring with red
            FastLED.show();                        // show color changes
            mode = 0;                              // switch back to idle mode
            holdTime = 0;                          // reset hold time to zero
        }
    }

    if (mode == 0 && ((millis() - idleStart) > rgbTimeout)) // check to see if idle exceeds timeout
    {
        patternRainbow();
    }

    // set lastState to current state
    lastState = nowState;
}

void patternRainbow()
{

    if (breathBrightness++ > 255) // which color to starthue
        breathBrightness = 0;     // handle rollover after 255

    fill_rainbow(leds, ledCount, breathBrightness);

    FastLED.show();
    //FastLED.delay(20);
}

void handleModeChange()
{
    int flashes = 4;     // number of times to flash for mode indication
    int lightTime = 250; // time to light up during flash
    int darkTime = 50;  // time to be dark during flash
    CRGB color;

    // if mode presses equal 3, change modes

    if (meetingMode == 0)
    {
        meetingMode = 1;
        color = CRGB::Blue;
    }
    else
    {
        meetingMode = 0;
        color = CRGB::Purple;
    }

    modePresses = 0;

    for (int i = 0; i < flashes; i++)
    {
        fill_solid(leds, ledCount, CRGB::Black); // fill led ring purple
        FastLED.show();                          // show led changes
        FastLED.delay(darkTime);                 // pause 2 seconds

        fill_solid(leds, ledCount, color); // fill led ring purple
        FastLED.show();                    // show led changes
        FastLED.delay(lightTime);          // pause 2 seconds
    }
    fill_solid(leds, ledCount, CRGB::Red);
    FastLED.show();
}

void toggleMute()
{
    //Serial.println("toggleMute()");

    if (meetingMode == 0)
    {                                  // teams is active
        Keyboard.press(KEY_LEFT_CTRL); // press left CTRL
        Keyboard.press(' ');           // press space
        //Keyboard.press('M');            // press m
    }
    else
    {
        Keyboard.press(KEY_LEFT_ALT); // press left ALT
        Keyboard.press('a');          // press a
    }
    delay(50); // pause to confirm keystrokes register

    Keyboard.releaseAll(); // release all keys
}
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

#define NUM_LEDS 24    // number of leds
#define DATA_PIN 5     // led ring data pin
#define BRIGHTNESS 200 // led ring brightness
#define BUTTON 4       // pin for button
//#define TIMEOUT 7000   // test timeout at 2 seconds
#define TIMEOUT 3600000 // prod timeout shoudl be 1 hour
#define SPEAKDELAY 500  // ms to hold button before engaging speaking mode
#define ISDEBUG 1       // if ISDEBUT == 1 then don't send keystrokes (for debugging)
#define DEBOUNCE 15

int breathBrightness = 0; // global integer for tracking color in idle mode
int lastState = HIGH;     // global for last state of button
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

CRGB leds[NUM_LEDS]; // initialize LED strip

void setup()
{
    pinMode(BUTTON, INPUT);                                  // set button pin to input
    pinMode(DATA_PIN, OUTPUT);                               // set led ring pin to output
    Keyboard.begin();                                        // initialize keyboard emmulation
    Serial.begin(115200);                                    // begin serial output
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // initialize FastLED library

    showCurrentMode();

    idleStart = millis();
    pressStartTime = millis();
}

// LOW = pressed
// HIGH = not pressed

void loop()
{
    //int nowState = digitalRead(BUTTON); // read button state
    delay(DEBOUNCE);
    int nowState = getButtonState();

    if (lastState == LOW && nowState == HIGH)
    {                              //new button press
        pressStartTime = millis(); // reset last press time
        //Serial.println("resetting last press time to now. pressStartTime:" + String(millis()));
    }

    if (nowState == HIGH)
    {

        holdTime = millis() - pressStartTime;

        if (mode == 0 && holdTime > SPEAKDELAY) // not a quick tap, switch to speaking mode
        {
            //Serial.println("button held longer than 500ms");
            mode = 1;     // switch to speaking mode
            toggleMute(); // send keystrokes to toggle mute

            fill_solid(leds, NUM_LEDS, CRGB::Purple);
            FastLED.show();
            idleStart = millis();
        }
    }

    if (nowState == LOW && lastState == HIGH) // if new button release
    {
        holdTime = millis() - pressStartTime;
        int tapInterval = millis() - lastTapTime;
        Serial.println("button release, held for:" + String(holdTime) + ". pressStartTime:" + String(pressStartTime) + " / tapInterval:" + String(tapInterval) + " / modePresses:" + String(modePresses));

        idleStart = millis();

        if (mode == 0 && holdTime < SPEAKDELAY) // not in speaking mode and a quick tap, increment modePresses
        {
            if (tapInterval < 500)
            { // if tap interval is recent, increment taps

                modePresses++;
                Serial.println("incrementing mode presses to:" + String(modePresses));
                if (modePresses >= 4) // mode presses exceed threshold, change mode
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
            fill_solid(leds, NUM_LEDS, CRGB::Red); // fill ring with red
            FastLED.show();                        // show color changes
            mode = 0;                              // switch back to idle mode
            holdTime = 0;                          // reset hold time to zero
        }
    }

    if (mode == 0 && ((millis() - idleStart) > TIMEOUT)) // check to see if idle exceeds timeout
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

    fill_rainbow(leds, NUM_LEDS, breathBrightness);

    FastLED.show();
    FastLED.delay(20);
}

void handleModeChange()
{
    // if mode presses equal 3, change modes
    Serial.print("Old meeting mode:");
    Serial.println(meetingMode);

    if (meetingMode == 0)
    {
        meetingMode = 1;
    }
    else
    {
        meetingMode = 0;
    }

    Serial.print("meeting mode after incrementing:");
    Serial.println(meetingMode);

    modePresses = 0;

    Serial.println("new meeting mode:" + String(meetingMode));

    showCurrentMode(); // show the current meeting mode
}

void showCurrentMode()
{
    Serial.print("Current meeting mode: ");
    Serial.println(meetingMode);

    int flashes = 3;     // number of times to flash for mode indication
    int lightTime = 500; // time to light up during flash
    int darkTime = 250;  // time to be dark during flash
    CRGB color;

    if (meetingMode == 0)
    {
        color = CRGB::Purple;
    }
    else
    {
        color = CRGB::Blue;
    }

    Serial.println(color);

    for (int i = 0; i < flashes; i++)
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black); // fill led ring purple
        FastLED.show();                          // show led changes
        FastLED.delay(darkTime);                 // pause 2 seconds

        fill_solid(leds, NUM_LEDS, color); // fill led ring purple
        FastLED.show();                    // show led changes
        FastLED.delay(lightTime);          // pause 2 seconds
    }
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
}

void toggleMute()
{
    //Serial.println("toggleMute()");

    if (ISDEBUG == 0)
    {
        switch (meetingMode)
        {
        case 0:                             // teams is active
            Keyboard.press(KEY_LEFT_CTRL);  // press left CTRL
            Keyboard.press(KEY_LEFT_SHIFT); // press left shift
            Keyboard.press('m');            // press m

            delay(100); // pause to confirm keystrokes register

            Keyboard.releaseAll(); // release all keys
            break;

        case 1:                           // zoom is active
            Keyboard.press(KEY_LEFT_ALT); // press left ALT
            Keyboard.press('a');          // press a

            delay(100); // pause to confirm keystrokes register

            Keyboard.releaseAll(); // release all keys
            break;
        }
    }
}

bool getButtonState()
{
    // read the state of the switch/button:
    int currentState = digitalRead(BUTTON);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // If the switch/button changed, due to noise or pressing:
    if (currentState != lastFlickerableState)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
        // save the the last flickerable state
        lastFlickerableState = currentState;
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE)
    {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (lastSteadyState == LOW && currentState == HIGH)
            Serial.println("The button is pressed");
        else if (lastSteadyState == HIGH && currentState == LOW)
            Serial.println("The button is released");

        // save the the last steady state
        lastSteadyState = currentState;
    }
    return currentState;
}
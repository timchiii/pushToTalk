/*

# pushToTalk

**Wiring Diagram**
- GND -> 10kR 
- GND -> LED-
- 10kR -> PIN 4
- PIN 4 -> button bottom
- VCC -> button side
- VCC -> LED+
- PIN 5 -> LED Data

    
**List of Materials**
1. 1x Arduino Pro Micro 
   - https://www.amazon.com/dp/B08THVMQ46?psc=1&ref=ppx_yo2_dt_b_product_details
2. 1x Addressable RGB led
   - I'm using WS2812B's but any version should work
   - https://www.amazon.com/gp/product/B01D1FFVOA/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
3. 1x Translucent arcade button with built in LED
   - The LED will be removed and replaced with the RGB variant
   - https://www.amazon.com/dp/B07XYVN1GY?ref=ppx_yo2_dt_b_product_details&th=1
4. 1x 10k ohm resistor
   - https://www.amazon.com/EDGELEC-Resistor-Tolerance-Multiple-Resistance/dp/B07QJB31M7/ref=sr_1_1_sspa?keywords=10k%2Bohm%2Bresistor&qid=1637011805&sr=8-1-spons&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFLMVgwRk5aTjM1TlYmZW5jcnlwdGVkSWQ9QTEwMjcwMzMyMzlCV0ZUWUpMUExYJmVuY3J5cHRlZEFkSWQ9QTA4MDc3MjEyQU1UQlZFV08xQUdMJndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ&th=1
5. Hookup wire
   - Recommend red, black, and 3 other colors to distinguish functions
6. Hot glue gun, epoxy or 3d printer pen to mount the Arduino micro inside the button.


**Directions**
1. Take apart the arcade button, remove the built in LED, and discard it.
2. Use needle nose pliers to remove the metal LED contacts.
3. Solder power and control wires to the new RGB LED.
3. Run the LED wires down through the button housing, so the new RGB LED sits inside the black cylinder, on top of where the old LED contacts where mounted. Be sure to leave room so you can put the switch back. ![Button Housing](https://github.com/timchiii/pushToTalk/blob/main/switchAndButtonHousing.jpg?raw=true)
4. Put the switch back on the button housing.
5. Solder the remaining connection points per the wiring diagram above.
6. 3d print (or otherwise acquire) a small case to mount the button. A very simple 3d model is included.
7. Put the button through the top of the case and connect it to the button housing. Set the Arduino inside so the micro USB plug is accessable from the hole in the back. ![Bottom View](https://github.com/timchiii/pushToTalk/blob/main/wiredBottom.jpg?raw=true)
8. Connect a micro USB cable from the Arduino to your PC
9. Upload this sketch.
            

**Usage**
1. Open the Teams app and join a meeting.
2. Ensure the meeting window has focus while using the button.
3. Press and hold the button to talk.
   - It will light up purple and un-mute your mic.
   - ![Purple While Pressed](https://github.com/timchiii/pushToTalk/blob/main/purpleWhilePressed.jpg?raw=true)
4. Release the button when you are finished speaking.
   - It will re-mute your mic and light up red to indicate mute.
   - ![Button Lit Red](https://github.com/timchiii/pushToTalk/blob/main/idleRed.jpg?raw=true)
5. To switch between Teams and Zoom, tap 3 times quickly.
   - Purple flashes indicate Teams mode.
   - Blue flashes indicate Zoom mode.
6. After being idle for approx. 1 hour, the LED will switch to idle mode and show rainbow colors.
   - ![Rainbow Example](https://github.com/timchiii/pushToTalk/blob/main/ledBlue.jpg?raw=true)


**PS**
I know what you're thinking, the gold is ugly. It was already in the printer.

*/

#include "Keyboard.h"
#include "HardwareSerial.h"
#include <FastLED.h>

#define ledCount 1  // number of leds
#define dataPin 5   // led ring data pin
#define buttonPin 4 // pin for button
//#define rgbTimeout 7000   // test timeout at 2 seconds
#define rgbTimeout 3600000 // prod timeout shoudl be 1 hour
#define speakDelay 200     // ms to hold button before engaging speaking mode
#define debounceTimeout 15 // delay each loop by this to debounce button input
#define modeChangeTaps 3   // number of taps to change modes
#define maxTapInterval 199 // max interval between taps

int rainbowColor = 0; // global integer for tracking color in idle mode
int lastState = LOW;  // global for last state of button
int meetingMode = 0;  // 0 = teams, 1 = zoom
int modePresses = 0;  // track number of clicks to change modes
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

    if (rainbowColor++ > 255) // which color to starthue
        rainbowColor = 0;     // handle rollover after 255

    fill_rainbow(leds, ledCount, rainbowColor);

    FastLED.show();
    //FastLED.delay(20);
}

void handleModeChange()
{
    int flashes = 4;     // number of times to flash for mode indication
    int lightTime = 250; // time to light up during flash
    int darkTime = 50;   // time to be dark during flash
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
    {                                   // teams is active
        Keyboard.press(KEY_LEFT_CTRL);  // press left CTRL
        Keyboard.press(KEY_LEFT_SHIFT); // press left shift
        //Keyboard.press(' ');           // press space
        Keyboard.press('M'); // press m
    }
    else
    {
        Keyboard.press(KEY_LEFT_ALT); // press left ALT
        Keyboard.press('a');          // press a
    }
    delay(10); // pause to confirm keystrokes register

    Keyboard.releaseAll(); // release all keys
}
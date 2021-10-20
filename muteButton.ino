#include "Keyboard.h"

const int button = 4;
const int led = 5;
int lastState = HIGH;

void setup()
{
    pinMode(button, INPUT);
    pinMode(led, OUTPUT);
    Keyboard.begin;
}

// LOW = pressed
// HIGH = not pressed

void loop()
{
    int nowState = digitalRead(button);

    /* if button state has changed send
       CTRL-SHIFT-m to toggle mute */
    if (nowState != lastState)
    {
        //press left ctrl
        Keyboard.press(KEY_LEFT_CTRL);

        // press left shift
        Keyboard.press(KEY_LEFT_SHIFT);

        // press m
        Keyboard.press('m')

        // pause briefly
        delay(100);

        // release all keys
        Keyboard.releaseAll();

        if (nowState == HIGH)
        {
            // illuminate green led
            digitalWrite(led, HIGH);
        }
        if (nowState == LOW)
        {
            // illuminate green led
            digitalWrite(led, LOW);
        }
    }
    
    // set lastState to current state
    lastState = nowState;
}
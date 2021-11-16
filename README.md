## What is this?
This is code, wiring diagram, BOM, and directions for a "push to talk" button for MS Teams and Zoom. It emmulates a USB keyboard. On button press, it sends keystrokes to toggle mute and changes the LED to purple or blue, depending on your mode. On button release, it toggles mute and changes the LED to red to indicate mute. Tap 3 times quickly to switch between Zoom and Teams modes. 

**Note: You must join the meeting and mute yourself manually for this to work properly.**

**Zoom mode is experimental at this point.**

**Don't @ me if you accidentally swear at your boss.**

## Wiring Diagram
- GND -> 10kR 
- GND -> LED-
- 10kR -> PIN 4
- PIN 4 -> button bottom
- VCC -> button side
- VCC -> LED+
- PIN 5 -> LED Data
    
## List of Materials
- 1x Arduino Pro Micro 
   - https://www.amazon.com/dp/B08THVMQ46?psc=1&ref=ppx_yo2_dt_b_product_details
- 1x Addressable RGB led
   - I'm using WS2812B's but any version should work
   - https://www.amazon.com/gp/product/B01D1FFVOA/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
- 1x Translucent arcade button with built in LED
   - The LED will be removed and replaced with the RGB variant
   - https://www.amazon.com/dp/B07XYVN1GY?ref=ppx_yo2_dt_b_product_details&th=1
- 1x 10k ohm resistor
   - https://www.amazon.com/EDGELEC-Resistor-Tolerance-Multiple-Resistance/dp/B07QJB31M7/ref=sr_1_1_sspa?keywords=10k%2Bohm%2Bresistor&qid=1637011805&sr=8-1-spons&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFLMVgwRk5aTjM1TlYmZW5jcnlwdGVkSWQ9QTEwMjcwMzMyMzlCV0ZUWUpMUExYJmVuY3J5cHRlZEFkSWQ9QTA4MDc3MjEyQU1UQlZFV08xQUdMJndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ&th=1
- Hookup wire
   - Recommend red, black, and 3 other colors to distinguish functions
- Hot glue gun, epoxy or 3d printer pen to mount the Arduino micro inside the button.


## Directions
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
            

## Usage
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


### PS
I know what you're thinking, the gold is ugly. You are correct. It was already in the printer. Better pictures will be uploaded after printing in a different filament.

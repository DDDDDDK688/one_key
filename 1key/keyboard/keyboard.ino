/*
  HID Keyboard example


  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
//#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidKeyboard/USBHIDKeyboard.h"

#define BUTTON1_PIN 15
//#define BUTTON2_PIN 31
//#define BUTTON3_PIN 32
#define ENTER 0x0A



bool button1PressPrev = false;
bool button2PressPrev = false;
bool button3PressPrev = false;


void setup() {
  USBInit();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  //pinMode(BUTTON2_PIN, INPUT_PULLUP);
  //pinMode(BUTTON3_PIN, INPUT_PULLUP);
 
}

void loop() {
  //button 1 is mapped to letter LF_ENTER
  bool button1Press = !digitalRead(BUTTON1_PIN);
  if (button1PressPrev != button1Press) {
    button1PressPrev = button1Press;
    if (button1Press) {
      Keyboard_press(ENTER);
    } else {
      Keyboard_release(ENTER);
    }
  }
}





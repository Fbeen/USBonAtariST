#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include "hidjoystickrptparser.h"
// #define debug 1

uint16_t timer = 0;
bool turboOn = false;
bool turboState = false;

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents(&turboOn);
JoystickReportParser Joy(&JoyEvents);

void setup() {
        Serial.begin(115200);
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Start");
#ifdef debug
        Serial.println("DEBUG MODE ON!");
#endif
        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");

        delay(200);

        if (!Hid.SetReportParser(0, &Joy))
                ErrorMessage<uint8_t > (PSTR("SetReportParser"), 1);

  digitalWrite(BTNUP, LOW);
  digitalWrite(BTNDOWN, LOW);
  digitalWrite(BTNLEFT, LOW);
  digitalWrite(BTNRIGHT, LOW);
  digitalWrite(FIRE1, LOW);
  digitalWrite(FIRE2, LOW);

  pinMode(BTNUP, INPUT);
  pinMode(BTNDOWN, INPUT);
  pinMode(BTNLEFT, INPUT);
  pinMode(BTNRIGHT, INPUT);
  pinMode(FIRE1, INPUT);
  pinMode(FIRE2, INPUT);
  
  cli();
  TCCR1A = 0;
  TCCR1B = 0; 
  OCR1A = 250;
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11); 
  TIMSK1 = (1<<OCIE1A); 
  sei(); 
}

void loop() {
        Usb.Task();
}

ISR(TIMER1_COMPA_vect)
{
  timer++;
  if(timer >= 20) {
    timer = 0;
    if(turboOn) {
      if(turboState) {
        turboState = false;
        pinMode(FIRE1, INPUT);
      } else {
        turboState = true;
        pinMode(FIRE1, OUTPUT);
      }
    }
  }
}

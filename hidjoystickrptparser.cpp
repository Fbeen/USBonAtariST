#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
joyEvents(evt),
oldHat(0xDE),
oldButtons(0) {
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                oldPad[i] = 0xD;
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
        bool match = true;

        // Checking if there are changes in report since the method was last called
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                if (buf[i] != oldPad[i]) {
                        match = false;
                        break;
                }

        // Calling Game Pad event handler
        if (!match && joyEvents) {
                joyEvents->OnGamePadChanged((const GamePadEventData*)buf);

                for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
        }

        uint8_t hat = (buf[5] & 0xF);

        // Calling Hat Switch event handler
        if (hat != oldHat && joyEvents) {
                joyEvents->OnHatSwitch(hat);
                oldHat = hat;
        }

        uint16_t buttons = (0x0000 | buf[6]);
        buttons <<= 4;
        buttons |= (buf[5] >> 4);
        uint16_t changes = (buttons ^ oldButtons);

        // Calling Button Event Handler for every button changed
        if (changes) {
                for (uint8_t i = 0; i < 0x0C; i++) {
                        uint16_t mask = (0x0001 << i);

                        if (((mask & changes) > 0) && joyEvents) {
                                if ((buttons & mask) > 0)
                                        joyEvents->OnButtonDn(i + 1);
                                else
                                        joyEvents->OnButtonUp(i + 1);
                        }
                }
                oldButtons = buttons;
        }
}

JoystickEvents::JoystickEvents(bool *turboOn) {
  this->turboOn = turboOn;
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {

  
#ifdef debug
        Serial.print("X1: ");
        PrintHex<uint8_t > (evt->X, 0x80);
        Serial.print("\tY1: ");
        PrintHex<uint8_t > (evt->Y, 0x80);
        Serial.print("\tX2: ");
        PrintHex<uint8_t > (evt->Z1, 0x80);
        Serial.print("\tY2: ");
        PrintHex<uint8_t > (evt->Z2, 0x80);
        Serial.print("\tRz: ");
        PrintHex<uint8_t > (evt->Rz, 0x80);
        Serial.println("");
#endif

        if(evt->Y == 0x00) {
          pinMode(BTNUP, OUTPUT);
        } else {
          pinMode(BTNUP, INPUT);
        }

        if(evt->Y == 0xFF) {
          pinMode(BTNDOWN, OUTPUT);
        } else {
          pinMode(BTNDOWN, INPUT);
        }

        if(evt->X == 0x00) {
          pinMode(BTNLEFT, OUTPUT);
        } else {
          pinMode(BTNLEFT, INPUT);
        }

        if(evt->X == 0xFF) {
          pinMode(BTNRIGHT, OUTPUT);
        } else {
          pinMode(BTNRIGHT, INPUT);
        }
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
#ifdef debug
        Serial.print("Hat Switch: ");
        PrintHex<uint8_t > (hat, 0x80);
        Serial.println("");
#endif
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
#ifdef debug
        Serial.print("Up: ");
        Serial.println(but_id, DEC);
#endif
        
        if(but_id == 1) {
          *this->turboOn = false;
          pinMode(FIRE1, INPUT);
        }
        
        if(but_id == 2) {
          pinMode(FIRE1, INPUT);
        }
        
        if(but_id == 3) {
          pinMode(FIRE2, INPUT);
        }
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
#ifdef debug
        Serial.print("Dn: ");
        Serial.println(but_id, DEC);
#endif
        
        if(but_id == 1) {
          *this->turboOn = true;
          pinMode(FIRE1, OUTPUT);
        }
        
        if(but_id == 2) {
          pinMode(FIRE1, OUTPUT);
        }
        
        if(but_id == 3) {
          pinMode(FIRE2, OUTPUT);
        }
}

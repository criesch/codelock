/*
 * code lock
 *
 * Copyright (c) 2019 Christian Riesch, christian@riesch.at
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <Keypad.h>

// keypad definition
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {2, 3, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// output relays
const byte NUMRELAYS = 4;
byte relaypins[NUMRELAYS] = { 7, 6, 5, 4 };
const byte defaultRelay = 1 << 0;

// relay selection, pressing 0* before the code selects the relays defined in relaySets[0],
// pressing 1* selects those in relaySets[1] etc.
byte relaySets[10] = { 0x01, 0x01, 0x02, 0x04, 0x08, 0x03, 0x0b, 0x0f, 0x01, 0x01 };

const unsigned long BUFFERTIMEOUT = 5000;
const unsigned long RELAYONTIME = 2000;

// code buffer
const char CODE[] = "12345";
const byte MAXCODELENGTH = 10;
String codeBuf = "";

// keyboard timeout
unsigned long lastkeypresstime = 0;

// relays selection and unlocking
byte selectedRelay = defaultRelay;
byte unlockedRelay = 0;

// relay operation
unsigned long relayActivationTime;
int activeRelay = -1;

// relay 1 push button
byte pinRelay1Pb = 13;

// fail counter: lock all relays for X milliseconds after Y failed attempts
const byte LOCKOUT_THRESHOLD = 3;
const unsigned long LOCKOUT_DURATION = 60000;
byte lockoutCnt = 0;
unsigned long lockoutTime = 0;

void setup() {
  Serial.begin(9600);

  clearbuf();

  for (byte i = 0; i < NUMRELAYS; i++) {
    digitalWrite(relaypins[i], LOW);
    pinMode(relaypins[i], OUTPUT);
  }
  pinMode(pinRelay1Pb, INPUT_PULLUP);

  Serial.println("starting...");
}

void loop() {
  // handle buffer timeout, if no key was pressed in the last BUFFERTIMEOUT
  // milliseconds, clear the code buffer and select the default relay
  if ((millis() - lastkeypresstime) > BUFFERTIMEOUT) {
    if (codeBuf.length() != 0) {
      Serial.println("buffer timeout: clear buffer");
      clearbuf();
    }
    if (selectedRelay != defaultRelay) {
      selectedRelay = defaultRelay;
      Serial.println("buffer timeout: select relays 0x" + String(selectedRelay, HEX));
    }
  }

  // reset the lockout counter after LOCKOUT_DURATION time
  if (((millis() - lockoutTime) > LOCKOUT_DURATION) && (lockoutCnt > 0)) {
    Serial.println("lockout: Reset lockout counter");
    lockoutCnt = 0;
  }

  // check the dedicated second relay pushbutton. Pressing this button selects
  // the second relay.
  if ((lockoutCnt < LOCKOUT_THRESHOLD) && (digitalRead(pinRelay1Pb) == LOW)) {
    lastkeypresstime = millis();
    if (selectedRelay != (1 << 1)) {
      selectedRelay = (1 << 1);
      Serial.println("pushbutton: select relay 0x" + String(selectedRelay, HEX));
    }
  }

  // handle keyboard events
  char key;
  if (lockoutCnt < LOCKOUT_THRESHOLD) {
    key = keypad.getKey();
  } else {
    key = NO_KEY;
  }

  if (key != NO_KEY){
    Serial.println(key);

    switch(key) {
      case '#':
        // verify code
        if (isCodeGood()) {
          Serial.println("unlocking relays 0x" + String(selectedRelay, HEX));
          unlockedRelay |= selectedRelay;
          selectedRelay = defaultRelay;
          lockoutCnt = 0;
        } else {
          Serial.println("#: wrong code entered.");
          lockoutCnt++;
          lockoutTime = millis();
        }
        clearbuf();
        break;
      case '*':
        // relay selection, select one of the relay sets defined in relaySets[]
        if (codeBuf.length() == 1) {
          // only if a single key is in the buffer
          if ((codeBuf.charAt(0) >= '0') && (codeBuf.charAt(0) <= '9')) {
            selectedRelay = relaySets[codeBuf.charAt(0) - '0'];
            Serial.println("*: select relays 0x" + String(selectedRelay, HEX));
          }
        }
        clearbuf();
        break;
      default:
        // any other key is just added to the buffer
        pushkey(key);
        break;
    }
    lastkeypresstime = millis();
  }

  // operate the relays when they are unlocked in the unlockedRelay bit field
  if (activeRelay >= 0) {
    // currently one relay is in on state, switch it off after RELAYONTIME
    if (millis() - relayActivationTime > RELAYONTIME) {
      digitalWrite(relaypins[activeRelay], LOW);
      Serial.println("relay: switch off relay " + String(activeRelay));
      unlockedRelay &= ~(1 << activeRelay); // clear bit
      activeRelay = -1;
    }
  } else {
    // no relay in on state, check if there is an unlocked one
    for (byte i = 0; i < NUMRELAYS; i++) {
      if (unlockedRelay & (1 << i)) { // test bit
        digitalWrite(relaypins[i], HIGH);
        Serial.println("relay: switch on relay " + String(i));
        relayActivationTime = millis();
        activeRelay = i;
        break; // exit loop, we only have one relay in on state at a time
      }
    }
  }
}

void clearbuf()
{
  codeBuf = "";
}

void pushkey(char c)
{
  if (codeBuf.length() < MAXCODELENGTH)
    codeBuf += String(c);
}

bool isCodeGood()
{
  if (codeBuf == CODE) {
    return true;
  } else {
    return false;
  }
}

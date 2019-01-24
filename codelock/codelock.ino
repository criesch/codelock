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

const unsigned long BUFFERTIMEOUT = 5000;
const unsigned long RELAYONTIME = 2000;

// code buffer
const char CODE[] = "12345";
const byte MAXCODELENGTH = 10;
String codeBuf = "";

// keyboard timeout
unsigned long lastkeypresstime = 0;

// relays
byte selectedRelay = 0;
unsigned long relayActivationTime[NUMRELAYS];

// relay 1 push button
byte pinRelay1Pb = 13;

void setup() {
  Serial.begin(9600);

  clearbuf();

  for (byte i = 0; i < NUMRELAYS; i++) {
    digitalWrite(relaypins[i], LOW);
    pinMode(relaypins[i], OUTPUT);
  }
  pinMode(pinRelay1Pb, INPUT_PULLUP);
}

void loop() {
  char key = keypad.getKey();

  if ((millis() - lastkeypresstime) > BUFFERTIMEOUT) {
    if (codeBuf.length() != 0) {
      Serial.println("timeout: clear buffer");
      clearbuf();
    }
    if (selectedRelay != 0) {
      selectedRelay = 0;
      Serial.println("Selected relay " + String(selectedRelay));
    }
  }

  // set back the relays after RELAYONTIME
  for (byte i = 0; i < NUMRELAYS; i++){
    if (millis() - relayActivationTime[i] > RELAYONTIME) {
      digitalWrite(relaypins[i], LOW);
    }
  }

  // check the dedicated relay 1 pushbutton
  if (digitalRead(pinRelay1Pb) == LOW) {
    lastkeypresstime = millis();
    if (selectedRelay != 1) {
      selectedRelay = 1;
      Serial.println("Selected relay " + String(selectedRelay));
    }
  }

  if (key != NO_KEY){
    Serial.println(key);

    switch(key) {
      case '#':
        // verify code
        if (isCodeGood()) {
          Serial.println("Unlocking relay " + String(selectedRelay));
          digitalWrite(relaypins[selectedRelay], HIGH);
          relayActivationTime[selectedRelay] = millis();
        }
        clearbuf();
        break;
      case '*':
        // relay selection
        if (codeBuf.length() == 1) {
          // only if a single key is in the buffer
          if ((codeBuf.charAt(0) >= '1') && (codeBuf.charAt(0) < '1' + NUMRELAYS)) {
            selectedRelay = codeBuf.charAt(0) - '1';
            Serial.println("Selected relay " + String(selectedRelay));
          }
        }
        clearbuf();
        break;
      default:
        // add key to buffer
        pushkey(key);
        break;
    }
    lastkeypresstime = millis();
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
    Serial.println("unlocked");
    return true;
  } else {
    Serial.println("wrong code");
    return false;
  }
}

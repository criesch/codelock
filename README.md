# Codelock

This is a code lock for house doors, garage openers etc.

## Required Hardware

I use this piece of code on
* an Arduino UNO SMD Rev 3,
* the Seeed Technology Relay Shield
* A 12 button numeric keypad (10 digits plus * and #) like the SparkFun Electronics 14662.

Digikey part numbers are
| pcs | Description                  | Digikey part number |
|-----|------------------------------|---------------------|
| 1   | Arduino UNO SMD Rev 3        | 1050-1041-ND        |
| 1   | Seed Technology Relay Shield | 1597-1146-ND        |
| 1   | SparkFun 14662 Keypad        | 1568-14662-ND       |

I have 3D printed a housing for the SparkFun keyboard, see the STL files in the housing
subdirectory. The housing is for the keyboard only. I mount the Arduino and the relay
shield in a box inside the building to prevent someone from bridging the relay contacts.

## Wiring

The keypad is wired to the Arduino as follows:

| keypad pin | Arduino pin | Description   |
|------------|-------------|---------------|
| 1          | n/a         | not connected |
| 2          | 11          | column 2
| 3          | 2           | row 1
| 4          | 10          | column 1
| 5          | 9           | row 4
| 6          | 12          | column 3
| 7          | 8           | row 3
| 8          | 3           | row 2
| 9          | n/a         | not connected |

## Usage

Type the code and push the # key to close relay 1 for two seconds. The other
3 relays (to open e.g. the garage door instead of the house door) can be addressed
by typing N*code#, where N is the number of the relay 1-4. Also adressing of
several relays at the same time is possible, as configured in the
variable relaySets. The relays are then operated in a sequence, one after the other.

The code is hardcoded in the variable CODE.

## License

code lock

Copyright (c) 2019-2026 Christian Riesch, christian@riesch.at

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include <Keypad.h>
#include <TextKeypad.h>

const char keys2[4][3] PROGMEM = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}, 
  {'*', '0', '#'}
};

byte pin_rows[4] = {14, 32, 33, 26};
byte pin_cols[3] = {27, 12, 25};

Keypad keypad = Keypad(makeKeymap(keys2), pin_rows, pin_cols, 4, 3);
TextKeypad test = TextKeypad(&keypad);

void setup() {
  Serial.begin(115200);
}

String prevstr = "";
String str = "";
void loop() {
  test.scan();
  prevstr = str;
  str = test.getString();
  if(prevstr.equals(str) == false) {
    Serial.println(str);
  }
}

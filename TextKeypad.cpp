/*
||
|| @file TextKeypad.cpp
|| @version 1.0
|| @author Luka Petković
||
|| @description
|| | This library provides an easy way to use keypads for inputting text,
|| | like on older mobile phones.
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#include <Arduino.h>
#include "Keypad.h"
#include "TextKeypad.h"

#define KEYPRESS_TIME 1000
#define KEYPAD_WIDTH 3
#define KEYPAD_HEIGHT 4
#define MAX_TXT_LENGTH 50 //can't be more than 254

/*
    CAN'T BE MODIFIED 
*/
const char keys[KEYPAD_HEIGHT][KEYPAD_WIDTH] PROGMEM = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

/*
  CAN BE MODIFIED
*/
const char characters_lowercase[KEYPAD_HEIGHT][KEYPAD_WIDTH][4] PROGMEM = {
  {{}, {'a', 'b', 'c'}, {'d', 'e', 'f'}},
  {{'g', 'h', 'i'}, {'j', 'k', 'l'}, {'m', 'n', 'o'}},
  {{'p', 'q', 'r', 's'}, {'t', 'u', 'v'}, {'w', 'x', 'y', 'z'}},
  {{}, {' '}, {}}
};

/*
  CAN BE MODIFIED
*/
const char special_chars[KEYPAD_HEIGHT][KEYPAD_WIDTH] PROGMEM = {
  {'.', ',', '?'},
  {'!', '*', '-'},
  {'(', ')', '@'},
  {'.', '+', '.'}
};

/*
  HOW "DEEP" THE KEYBOARD GOES FOR ARRAY characters_lowercase
*/
const uint8_t character_count[KEYPAD_HEIGHT][KEYPAD_WIDTH] PROGMEM = {
  {0, 3, 3},
  {3, 3, 3},
  {4, 3, 4}
};

Keypad* keypadvar;

/*
  STORES THE STRING WHICH IS OUTPUTTED
*/
char* txt;
uint8_t txtlength = 0;

/*
    0 -> numbers
    1 -> lowercase
    2 -> uppercase
    3 -> special chars
*/
//used for checking in which state we are in
uint8_t casestate = 0;

TextKeypad::TextKeypad(Keypad* keypad) {
    keypadvar = keypad;
    txt = (char*)calloc((MAX_TXT_LENGTH + 1), sizeof(char));
}

char keyprev = 0;
char keywrite;
unsigned long prevMillis;
boolean wr = false;
char holdKey;
uint8_t z = 0;

/*
    -1 -> cancel
     0 -> not done yet
     1 -> done
*/
uint8_t TextKeypad::scan() {
  char key = keypadvar->getKey();
  if (key) {
    
    // '*' shortpress - delete
    if(key == '*') {
      if(txtlength > 0) {
        txt[txtlength-1] = '\0';
        txtlength -= 1;
      }
      return txtlength;
    } 

    // '#' shortpress - change state for lowercase, uppercase and special character case
    if(key == '#') {
      if(casestate != 3)
        casestate++;
      else
        casestate = 0;
      return txtlength;
    }

    holdKey = key;
    keywrite = key;
    wr = true;
    /*
      If you press the same key multiple times in KEYPRESS_TIME interval
      Increases the z axis
    */
    if(key == keyprev && millis()-prevMillis <= KEYPRESS_TIME) {
      uint8_t x, y;
      y = findkeyindex(key, &x);
      if(z == character_count[y][x]-1)
        z = 0;
      else
        z++;
    } 
    /*
      If you press a different key than the last one, then there is no waiting, the character is appended instantly
      to the output string variable txt
    */
    else {
      if(keyprev != 0) {
        uint8_t x, y;
        y = findkeyindex(keyprev, &x);
        if(append(y, x, z) == -1)
          return -1;
        z = 0;
      }
      keyprev = key;
    }
    prevMillis = millis();
  }

  /*
    If the time between keypresses exceeds KEYPRESS_TIME, character is appended to the output string variable txt
  */
  if(wr == true && millis()-prevMillis > KEYPRESS_TIME) {
    uint8_t x, y;
    y = findkeyindex(keywrite, &x);
    if(append(y, x, z) == -1)
      return -1;
    keyprev = key;
    z = 0;
    wr = false;
  }

  //HOLD '*' - cancel
  if(keypadvar->getState() == HOLD && holdKey == '*') {
    return -1;
  }

  //HOLD '#' - finish
  if(keypadvar->getState() == HOLD && holdKey == '#') {
    return 1;
  }

  return 0;
}

char* TextKeypad::getString() {
  if(txtlength == 0) {
    return "";
  }
  return txt;
}

uint8_t TextKeypad::length() {
  return txtlength;
}

/*
  Function that appends the character from the array specified by casestate variable to the output string variable txt
*/
uint8_t TextKeypad::append(uint8_t y, uint8_t x, uint8_t z) {
  if(txtlength >= MAX_TXT_LENGTH)
    return -1;
  char c;
  switch(casestate) {
    //numbers
    case 0:
      c = keys[y][x];
    break;
    //lowercase
    case 1:
      c = characters_lowercase[y][x][z];
    break;
    //uppercase
    case 2:
      if(characters_lowercase[y][x][z] != ' ')
        c = characters_lowercase[y][x][z] - 32;
      else 
        c = characters_lowercase[y][x][z];
    break;
    //special chars
    case 3:
      c = special_chars[y][x];
    break;
  }
  txt[txtlength] = c;
  txt[txtlength+1] = '\0';
  txtlength += 1;
  return txtlength;
}

void TextKeypad::end() {
  free(txt);
}

//takes key and finds its index in "keys" array
//returns y
uint8_t TextKeypad::findkeyindex(char key, uint8_t* x) {
  for(uint8_t i = 0; i < KEYPAD_HEIGHT; i++) {
    for(uint8_t k = 0; k < KEYPAD_WIDTH; k++) {
      if(key == keys[i][k]) {
        *x = k;
        return i;
      }
    }
  }
  return -1;
}

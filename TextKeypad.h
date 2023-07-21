/*
||
|| @file TextKeypad.h
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

#ifndef TEXTKEYPAD_H
#define TEXTKEYPAD_H

#include "Arduino.h"
#include "Keypad.h"

class TextKeypad {
    public:
        TextKeypad(Keypad* keypad);
        uint8_t scan();
        char* getString();
        uint8_t length();
        void end();
    private:
        uint8_t append(uint8_t y, uint8_t x, uint8_t z);
        uint8_t findkeyindex(char key, uint8_t* x);
        //const char** keys;
        // const char*** characters_lowercase;
        // const char** special_chars;
        // const uint8_t** character_count;
        // Keypad* keypadvar;
        // char* txt;
        // uint8_t txtlength;
        // uint8_t casestate;
        // char keyprev;
        // char keywrite;
        // unsigned long prevMillis;
        // boolean wr;
        // char holdKey;
};

#endif

/*
  Faces In Time (c)2023 Ehle

  MVP for Lucius.

  Changes:
  2023 06 25 = created cndrbrbr
  2024 06 22 = extended by tiny Receiver.cpp "example IR integration", cndrbrbr
               inserted Pixel Matrix commands into the tiny receiver code for showing a certain face, 
               when a certain number is clicked on the IR remote control. 
*/

#include <Arduino.h>


/*
 *  TinyReceiver.cpp
 *
 *  Small memory footprint and no timer usage!
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts.
 *  On complete received IR command the function handleReceivedIRData(uint16_t aAddress, uint8_t aCommand, uint8_t aFlags)
 *  is called in Interrupt context but with interrupts being enabled to enable use of delay() etc.
 *  !!!!!!!!!!!!!!!!!!!!!!
 *  Functions called in interrupt context should be running as short as possible,
 *  so if you require longer action, save the data (address + command) and handle it in the main loop.
 *  !!!!!!!!!!!!!!!!!!!!!
 *
 * The FAST protocol is a proprietary modified JVC protocol without address, with parity and with a shorter header.
 *  FAST Protocol characteristics:
 * - Bit timing is like NEC or JVC
 * - The header is shorter, 3156 vs. 12500
 * - No address and 16 bit data, interpreted as 8 bit command and 8 bit inverted command,
 *     leading to a fixed protocol length of (6 + (16 * 3) + 1) * 526 = 55 * 526 = 28930 microseconds or 29 ms.
 * - Repeats are sent as complete frames but in a 50 ms period / with a 21 ms distance.
 *
 *
 *  This file is part of IRMP https://github.com/IRMP-org/IRMP.
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2022-2024 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Set IR_RECEIVE_PIN for different CPU's

//#define DEBUG // to see if attachInterrupt is used
//#define TRACE // to see the state of the ISR state machine

/*
 * Protocol selection
 */
//#define DISABLE_PARITY_CHECKS // Disable parity checks. Saves 48 bytes of program memory.
//#define USE_EXTENDED_NEC_PROTOCOL // Like NEC, but take the 16 bit address as one 16 bit value and not as 8 bit normal and 8 bit inverted value.
//#define USE_ONKYO_PROTOCOL    // Like NEC, but take the 16 bit address and command each as one 16 bit value and not as 8 bit normal and 8 bit inverted value.
//#define USE_FAST_PROTOCOL     // Use FAST protocol instead of NEC / ONKYO.
//#define ENABLE_NEC2_REPEATS // Instead of sending / receiving the NEC special repeat code, send / receive the original frame for repeat.
/*
 * Set compile options to modify the generated code.
 */
//#define DISABLE_PARITY_CHECKS // Disable parity checks. Saves 48 bytes of program memory.
//#define USE_CALLBACK_FOR_TINY_RECEIVER  // Call the fixed function "void handleReceivedTinyIRData()" each time a frame or repeat is received.

#include "TinyIRReceiver.hpp" // include the code

/*
 * Helper macro for getting a macro definition as string
 */
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

// LED ===========================
#include <FastLED.h>
#define NUM_LEDS 64
#define DATA_PIN 3
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];
// LED ===========================

void setup() {
    Serial.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
#if defined(ESP8266) || defined(ESP32)
    Serial.println();
#endif
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_TINYIR));

    // Enables the interrupt generation on change of IR input signal
    if (!initPCIInterruptForTinyReceiver()) {
        Serial.println(F("No interrupt available for pin " STR(IR_RECEIVE_PIN))); // optimized out by the compiler, if not required :-)
    }
#if defined(USE_FAST_PROTOCOL)
    Serial.println(F("Ready to receive Fast IR signals at pin " STR(IR_RECEIVE_PIN)));
#else
    Serial.println(F("Ready to receive NEC IR signals at pin " STR(IR_RECEIVE_PIN)));
#endif

 // LED ===========================
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
// LED ===========================
 
}
#define DELAYVAL 3000 // Time (in milliseconds) to pause between pixels
/*=================================================================================== */
/* MATRIX DISPLAY Functions */
/*=================================================================================== */
/*
static int NeutralFace[19]  = {62,65,61,66,190,193,189,194,89,102,105,118,121,134,137,150,153,166,999};
static int SlimEyeFace[15]  = {62,65,190,193,89,102,105,118,121,134,137,150,153,166,999};
static int Smiley[15]  = {62,65,190,193,189,194,89,102,105,118,121,134,137,150,999};
static int theLine[33] = {1,14,17,30,33,46,49,62,65,78,81,94,97,110,113,126,129,142,145,158,161,174,177,190,193,206,209,222,225,238,241,254,999};
static int secLine[12] = {0,1,2,3,4,5,6,7,8,9,10,999};
*/

static int NeutralFace[19]  = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,999};
static int SlimEyeFace[11]  = {40,41,42,43,44,45,46,47,48,49,999};
static int Smiley[6]  = {55,56,57,58,59,999};
static int theLine[33] = {1,14,17,30,33,46,49,62,65,78,81,94,97,110,113,126,129,142,145,158,161,174,177,190,193,206,209,222,225,238,241,254,999};
static int secLine[12] = {0,1,2,3,4,5,6,7,8,9,10,999};

int NumberofFaces = 5;
int WalkingFaceNumber = 1;
int* GetFace (int facenumber)
{
  switch (facenumber){
    case(1):  return (&(NeutralFace[0]));break;
    case(2):  return (&(SlimEyeFace[0]));break;
    case(3):  return (&(Smiley[0]));break;
    case(4):  return (&(theLine[0]));break;
    case(5):  return (&(secLine[0]));break;
    default: return (&(NeutralFace[0]));
 }
} 
CRGB GetColor (int facenumber)
{
  switch (facenumber){
    case(1):  return (CRGB::Red);break;
    case(2):  return (CRGB::Yellow);break;
    case(3):  return (CRGB::AliceBlue);break;
    case(4):  return (CRGB::Coral);break;
    case(5):  return (CRGB::ForestGreen);break;
    default: return (CRGB::Gray);
 }
} 
void incrementFaceNumber()
{
    if (WalkingFaceNumber < NumberofFaces) WalkingFaceNumber+= 1;
    else WalkingFaceNumber = 2;
}

void faceOn (int* face,CRGB color) {
     int k = 0;
     while (face[k]!=999){ 
      if (face[k]>=NUM_LEDS) face[k] = 0;
      leds[face[k]] = color;
      k+=1;
    }
}
void faceOff (){
  for (int i=0; i<NUM_LEDS;i++){
    leds[i] = CRGB::Black;  
  }
}
void showMatrixTurn(int faceid) {
     
    //WalkingFaceNumber = num;
    faceOff ();
    FastLED.show(); 
    //incrementFaceNumber();
    int* theFace = GetFace (faceid);
    CRGB thecolor = GetColor (faceid);
    faceOn (theFace,thecolor);
    FastLED.show(); 
    delay(DELAYVAL); // Pause before next pass through loop

  }
// LED ===========================
void Blinkonce(int num) {
  if (num >= NUM_LEDS) num = 0;
  leds[num] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[num] = CRGB::Black;
  FastLED.show();
  delay(500);
}
// LED ===========================

int  GetNumberofFace (uint8_t number)
{
   switch (number){
    //zahlen
    case(12):  return 1;break;
    case(24):  return 2;break;
    case(94):  return 3;break;
    case(8):   return 4;break;
    case(28):  return 5;break;
    case(90):  return 6;break;
    case(66):  return 7;break;
    case(82):  return 8;break;
    case(74):  return 9;break;
    //oben
    // 69 70 71
    // 68 64 67
    // 7  21 9
    // 22 25 13
   default: return 1;
  }
}


void loop() {
    if (TinyIRReceiverData.justWritten) {
        TinyIRReceiverData.justWritten = false;
#if !defined(USE_FAST_PROTOCOL)
        // We have no address at FAST protocol
        Serial.print(F("Address=0x"));
        Serial.print(TinyIRReceiverData.Address, DEC);
        Serial.print(' ');
#endif
        Serial.print(F("Command=0x"));
        Serial.print(TinyIRReceiverData.Command, DEC);
        if (TinyIRReceiverData.Flags == IRDATA_FLAGS_IS_REPEAT) {
            Serial.print(F(" Repeat"));
        }
        if (TinyIRReceiverData.Flags == IRDATA_FLAGS_PARITY_FAILED) {
            Serial.print(F(" Parity failed"));
#if !defined(USE_EXTENDED_NEC_PROTOCOL) && !defined(USE_ONKYO_PROTOCOL)
            Serial.print(F(", try USE_EXTENDED_NEC_PROTOCOL or USE_ONKYO_PROTOCOL"));
#endif
        }
        Serial.println();
        //Blinkonce(0);
        int faceid = GetNumberofFace (TinyIRReceiverData.Command);
        showMatrixTurn(faceid);
    }
    //showMatrixTurn(WalkingFaceNumber);
  }

/*
 * Optional code, if you require a callback
 */
#if defined(USE_CALLBACK_FOR_TINY_RECEIVER)
/*
 * This is the function, which is called if a complete frame was received
 * It runs in an ISR context with interrupts enabled, so functions like delay() etc. should work here
 */
#  if defined(ESP8266) || defined(ESP32)
IRAM_ATTR
#  endif

void handleReceivedTinyIRData() {
#  if defined(ARDUINO_ARCH_MBED) || defined(ESP32)
    /*
     * Printing is not allowed in ISR context for any kind of RTOS, so we use the slihjtly more complex,
     * but recommended way for handling a callback :-). Copy data for main loop.
     * For Mbed we get a kernel panic and "Error Message: Semaphore: 0x0, Not allowed in ISR context" for Serial.print()
     * for ESP32 we get a "Guru Meditation Error: Core  1 panic'ed" (we also have an RTOS running!)
     */
    // Do something useful here...
#  else
    // As an example, print very short output, since we are in an interrupt context and do not want to miss the next interrupts of the repeats coming soon
    printTinyReceiverResultMinimal(&Serial);
#  endif
}
#endif


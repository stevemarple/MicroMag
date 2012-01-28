/*
 * MicroMagConvGetRes.ino
 * Author: Steve Marple
 * License: GNU Lesser General Public License, version 2.1
 *
 * Example sketch to demonstrate reading from a MicroMag2 or MicroMag3
 * module using the convert() and getResult() functions.
 *
 * Requirements:
 *   Arduino >= 1.0
 *   MicroMag library, see https://github.com/stevemarple/MicroMag
 *   MicroMag2 or MicroMag3 module
 */


#include <SPI.h>
#include <MicroMag.h>

// Adjust these pin definitions to suit your hardware. They are chosen
// to match the RN-XV and SD shield with the MicroMag3 module option.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// Arduino Mega/Mega2560
#define DRDY_PIN A8
const uint8_t mmResetPin = A3;
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3

#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) \
  || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) \
  || defined(__AVR_ATmega1284P__)

// Calunium
#define DRDY_PIN 14
const uint8_t mmResetPin = A3;
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3

#else
// Standard Arduino, DRDY does not connect by default to the Arduino
#define DRDY_PIN 0xff
const uint8_t mmResetPin = 8;
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3

#endif

#if DRDY_PIN == 0xff
#error You must manually wire the DRDY pin to an unused input since the \
  default pin on the shield does not connect to the Arduino 
#endif

const uint8_t mmDrdyPin = DRDY_PIN;

// Change this to suit your version.
const uint8_t model = 3;

// The sampling period
uint8_t period = MM_PERIOD_4096;

// Create a MicroMag object
MicroMag MM = MicroMag::MicroMag3(mmSsPin, mmDrdyPin, mmResetPin);

// Use an 8-bit variable to obtain atomic read/write access. Disabling
// interrupts is therefore not needed.
volatile uint8_t dataReady = false;

// Only one ISR needs to be defined, but which is it? Use the pin
// mapping macros to define the appropriate one.
#if digitalPinToPCICRbit(DRDY_PIN) == 0
ISR(PCINT0_vect){
  if (digitalRead(DRDY_PIN))
    dataReady = true;
}
#endif
#if digitalPinToPCICRbit(DRDY_PIN) == 1
ISR(PCINT1_vect){
  if (digitalRead(DRDY_PIN))
    dataReady = true;
}
#endif
#if digitalPinToPCICRbit(DRDY_PIN) == 2
ISR(PCINT2_vect){
  if (digitalRead(DRDY_PIN))
    dataReady = true;
}
#endif
#if digitalPinToPCICRbit(DRDY_PIN) == 3
ISR(PCINT3_vect){
  if (digitalRead(DRDY_PIN))
    dataReady = true;
}
#endif

void setupPinChangeInterrupts(void)
{
  *(digitalPinToPCMSK(DRDY_PIN)) |= _BV(digitalPinToPCMSKbit(DRDY_PIN));
  *(digitalPinToPCIR(DRDY_PIN)) |= _BV(digitalPinToPCICRbit(DRDY_PIN));
}

void setup()
{
  Serial.begin(9600);
  if (MM.begin() != 0) {
    Serial.println("Could not initialise MicroMag");
    while (1)
      ; // no point in continuing
  }
  setupPinChangeInterrupts();
}

// The 2 or 3 samples are taken in different invocations of loop() so
// use static variables to hold the results until all 2 or 3 have been
// taken and can be printed.
int16_t data[model];
unsigned long timestamps[model] = {0};
uint8_t axis = 0;
double total = 0.0;

void loop()
{
  // Check if new data is ready
  if (dataReady) {
    // Get the result
    data[axis] = MM.getResult();
    dataReady = false; // Data has been read
    ++axis;

    if (axis < model) {
      timestamps[axis] = millis();
      MM.convert(axis, period);
    }
    else {
      // Done all axes, print the results
      for (uint8_t i = 0; i < model; ++i) {
	if (i)
	  Serial.print("   ");
	Serial.print(char('X' + i));
	Serial.print(" = ");
	Serial.print(data[i]);
	total += sqrt(double(data[i]) * data[i]);
	data[i] = 0; // Reset for next time
      }
      Serial.print("   Total = ");
      Serial.println(total);

      for (uint8_t i = 0; i < model; ++i) {
	if (i)
	  Serial.print("   ");
	Serial.print(timestamps[i]);
      }
      Serial.println();
    }
  }

  // Check whether a new set of samples should be taken
  unsigned long now = millis();
  if (now >= timestamps[0] + 2000) {  
    // Start a new set of samples
    axis = 0;
    total = 0.0;
    timestamps[axis] = now;
    MM.convert(axis, period);
  }
}

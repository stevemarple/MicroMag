/*
 * MicroMagRead.ino
 * Author: Steve Marple
 * License: GNU Lesser General Public License, version 2.1
 */

#include <SPI.h>
#include <MicroMag.h>

// Adjust these pin definitions to suit your hardware. They are chosen
// to match the RN-XV and SD shield with the MicroMag3 module option.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// Arduino Mega/Mega2560
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3
const uint8_t mmDrdyPin = A8
const uint8_t mmResetPin = A3;

#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)

// Calunium
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3
const uint8_t mmDrdyPin = 14;
const uint8_t mmResetPin = A3;

#else
// Standard Arduino
const uint8_t mmSsPin = A0; // This is the chip select pin on the MM3
const uint8_t mmDrdyPin = 0xff;
const uint8_t mmResetPin = 8;
#endif

// Change this to suit your version.
const uint8_t model = 3;

// Create a MicroMag object
MicroMag MM = MicroMag::MicroMag3(mmSsPin, mmDrdyPin, mmResetPin);


void setup()
{
  Serial.begin(9600);
  if (MM.begin() != 0) {
    Serial.println("Could not initialise MicroMag");
    while (1)
      ; // no point in continuing
  }
}

void loop()
{
  int16_t data[model];
  double total = 0.0; // total field value
  for (uint8_t axis = 0; axis < model; ++axis) {
    int8_t r;
    if ((r = MM.read(axis, MM_PERIOD_4096, data[axis])) == 0) {
      if (axis)
	Serial.print("   ");
      Serial.print(char('X' + axis));
      Serial.print(" = ");
      Serial.print(data[axis]);
      total += sqrt(double(data[axis]) * data[axis]);
    }
    else {
      Serial.print("Cannot read from ");
      Serial.print(char('X' + axis));
      Serial.print("axis: ");
      Serial.print(r, DEC);
    }
  }
  Serial.print("   Total = ");
  Serial.println(total);
  
  delay(2000);

}

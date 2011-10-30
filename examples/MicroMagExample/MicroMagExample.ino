#include <SPI.h>
#include <MicroMag.h>

#if defined(__AVR_ATmega2560__)
const int8_t drdyPin = 47;
const int8_t resetPin = 45;
#else
const int8_t drdyPin = 9;
const int8_t resetPin = 8;
#endif

// Change this to suit your version.
const uint8_t model = 3;

// Create a MicroMag object
MicroMag MM = MicroMag::MicroMag3(SS, drdyPin, resetPin);


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
  int16_t d[model];
  double total = 0.0; // total field value
  for (uint8_t i = 0; i < model; ++i) {
    char axis = 'x' + i;
    int8_t r;
    if ((r = MM.read(axis, MM_PERIOD_4096, d[i])) == 0) {
      if (i)
	Serial.print("   ");
      Serial.print(axis);
      Serial.print(" = ");
      Serial.print(d[i]);
      total += sqrt(double(d[i]) * d[i]);
    }
    else {
      Serial.print("Cannot read from ");
      Serial.print(axis);
      Serial.print("axis: ");
      Serial.print(r, DEC);
    }
  }
  Serial.print("   Total = ");
  Serial.println(total);
  
  delay(2000);

}

#include <MicroMag.h>

MicroMag::MicroMag(uint8_t ssPin, uint8_t drdyPin, uint8_t resetPin,
		   uint8_t axes)
  : _axes(axes), _ssPin(ssPin), _drdyPin(drdyPin), _resetPin(resetPin)
{
  ;
}

MicroMag::MicroMag(const MicroMag& mm) \
  : _axes(mm._axes), _ssPin(mm._ssPin), _drdyPin(mm._drdyPin), _resetPin(mm._resetPin)
{
  ;
}

int8_t MicroMag::begin(void) const
{
  pinMode(_ssPin, OUTPUT);
  pinMode(_drdyPin, INPUT);
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_ssPin, HIGH);
  digitalWrite(_resetPin, LOW);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  // Make one reading to switch device into low power mode
  int16_t tmp;
  return read('x', MM_PERIOD_32, tmp);
}


int8_t MicroMag::read(char axis, uint8_t period, int16_t& result,
		      uint16_t timeout_us) const
{
  uint8_t ax = tolower(axis) - 'x' + 1;
  if (ax > _axes)
    return errorBadAxis;
  
  if (period > MM_PERIOD_4096)
    return errorBadPeriod;
  
  uint8_t cmd = 0;
  cmd |= ax;
  cmd |= (period << 4);
  
  // Select the device
  digitalWrite(_ssPin, LOW);

  // Reset
  digitalWrite(_resetPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(_resetPin, LOW);

  SPI.transfer(cmd); // Send the command byte

  // Wait for ready signal
  if (timeout_us == 0)
    // Set a default timeout which is approriate to the selected
    // period. See data sheet for details. Values used are 1us larger
    // to account for +/-1 jitter.
    switch (period) {
    case MM_PERIOD_32:
      timeout_us = 501;
      break;
    case MM_PERIOD_64:
      timeout_us = 1001;
      break;
    case MM_PERIOD_128:
      timeout_us = 2001;
      break;
    case MM_PERIOD_256:
      timeout_us = 4001;
      break;
    case MM_PERIOD_512:
      timeout_us = 7501;
      break;
    case MM_PERIOD_1024:
      timeout_us = 15001;
      break;
    case MM_PERIOD_2048:
      timeout_us = 35501;
      break;
    case MM_PERIOD_4096:
      timeout_us = 60001;
      break;
    default:
      return errorBadPeriod;
    }
  
  unsigned long t = micros();
  while (!digitalRead(_drdyPin)) {
    if (micros() - t > timeout_us)
      return errorTimeout;
  }
  
  // Read 2 bytes
  result = SPI.transfer(0);
  result <<= 8;
  result |= SPI.transfer(0);

  // De-select the device
  digitalWrite(_ssPin, HIGH);

  return errorNoError;
}

int8_t MicroMag::readHighPrec(char axis, int32_t& result,
			      uint16_t timeout_us) const
{
  int8_t r;
  int16_t dLow, dHigh;
  if ((r = read(axis, MM_PERIOD_2048, dLow, timeout_us)) != 0)
    return r;

  if ((r = read(axis, MM_PERIOD_4096, dHigh, timeout_us)) != 0)
    return r;

  if (dLow <= -16000 || dLow >= 16000) {
    // Assume high precision version has overflowed
    result = dLow * 2;
    result += (dHigh % 2); // try to keep extra precision
  }
  else
    result = dHigh;
  
  return errorNoError;
}

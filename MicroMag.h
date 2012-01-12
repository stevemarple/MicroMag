#ifndef _MICROMAG_H
#define _MICROMAG_H

#include "SPI.h"

#define MM_PERIOD_32 0
#define MM_PERIOD_64 1
#define MM_PERIOD_128 2
#define MM_PERIOD_256 3
#define MM_PERIOD_512 4
#define MM_PERIOD_1024 5
#define MM_PERIOD_2048 6
#define MM_PERIOD_4096 7

class MicroMag
{
public:
  enum modelTypes {
    twoAxisModel = 2,
    threeAxisModel = 3,
  };
  
  enum returnValues {
    errorNoError = 0,
    errorBadAxis = 1,
    errorBadPeriod = 2,
    errorTimeout = 3,
  };

  static MicroMag MicroMag2(uint8_t ssPin, uint8_t drdyPin, uint8_t resetPin)
  {
    return MicroMag(ssPin, drdyPin, resetPin, twoAxisModel);
  }
  
  static MicroMag MicroMag3(uint8_t ssPin, uint8_t drdyPin, uint8_t resetPin)
  {
    return MicroMag(ssPin, drdyPin, resetPin, threeAxisModel);
  }

  MicroMag(const MicroMag& mm);
  
  int8_t begin(void) const;
  int8_t convert(char axis, uint8_t period) const;
  int16_t getResult(void) const;
  int8_t read(char axis, uint8_t period, int16_t& result,
	      uint16_t timeout_us = 0) const;
  int8_t readHighPrec(char axis, int32_t& result,
		      uint16_t timeout_us = 0) const;
  
private:
  uint8_t _axes; // 2 = MicroMag2, 3 = MicroMag3
  uint8_t _ssPin;
  uint8_t _drdyPin; // Use 0xff to indicate DRDY not wired up
  uint8_t _resetPin;
  
  MicroMag(uint8_t ssPin, uint8_t drdyPin, uint8_t resetPin, uint8_t axes);

  MicroMag(void); // Declare but do not define
  
};

#endif

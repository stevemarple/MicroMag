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

  MicroMag(uint8_t ssPin, uint8_t drdyPin, uint8_t resetPin, uint8_t axes);

  // Convenient helper functions to create the MicroMag object
  inline static MicroMag MicroMag2(uint8_t ssPin, uint8_t drdyPin,
				   uint8_t resetPin) {
    return MicroMag(ssPin, drdyPin, resetPin, twoAxisModel);
  }
  
  inline static MicroMag MicroMag3(uint8_t ssPin, uint8_t drdyPin,
				   uint8_t resetPin) {
    return MicroMag(ssPin, drdyPin, resetPin, threeAxisModel);
  }

  MicroMag(const MicroMag& mm);
  
  uint8_t begin(void) const;
  uint8_t convert(uint8_t axis, uint8_t period) const;
  int16_t getResult(void) const;
  uint8_t read(uint8_t axis, uint8_t period, int16_t& result,
	      uint16_t timeout_us = 0) const;
  uint8_t readHighPrec(uint8_t axis, int32_t& result,
		      uint16_t timeout_us = 0) const;
  
private:
  uint8_t _axes; // 2 = MicroMag2, 3 = MicroMag3
  uint8_t _ssPin;
  uint8_t _drdyPin; // Use 0xff to indicate DRDY not wired up
  uint8_t _resetPin;
  
  // Do not allow default constructor; declare but do not define.
  MicroMag(void); 
  
};

#endif

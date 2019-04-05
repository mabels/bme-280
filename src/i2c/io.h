#ifndef _I2C_IO__
#define _I2C_IO__
#include "Arduino.h"

namespace I2C { 
  class Io {
    private:
      uint8_t addr;
    public:
    void begin(uint8_t sdaPIN, uint8_t sclPIN, uint8_t _addr) {
      addr = _addr;
      Wire.begin(sdaPIN, sclPIN);
    }

    void write8(uint8_t reg, uint8_t value)
    {
      Wire.beginTransmission(addr);
      Wire.write(reg);
      Wire.write(value);
      Wire.endTransmission();
    } 
    uint8_t read8(uint8_t reg) {
      Wire.beginTransmission(addr);
      Wire.write(reg);
      Wire.endTransmission();
      Wire.requestFrom(addr, (byte)1);
      return Wire.read();
    } 

    uint16_t read16(uint8_t reg)
    {
      Wire.beginTransmission(addr);
      Wire.write(reg);
      Wire.endTransmission();
      Wire.requestFrom(addr, (byte)2);
      return (Wire.read() << 8) | Wire.read();
    }

    uint16_t read16_LE(uint8_t reg) {
      uint16_t temp = read16(reg);
      return (temp >> 8) | (temp << 8);
    }

    int16_t readS16(uint8_t reg)
    {
      return (int16_t)read16(reg);

    }

    int16_t readS16_LE(uint8_t reg)
    {
      return (int16_t)read16_LE(reg);
    }

    uint32_t read24(uint8_t reg)
    {
      uint32_t value;
      Wire.beginTransmission((uint8_t)addr);
      Wire.write((uint8_t)reg);
      Wire.endTransmission();
      Wire.requestFrom((uint8_t)addr, (uint8_t)3);

      value = Wire.read();
      value <<= 8;
      value |= Wire.read();
      value <<= 8;
      value |= Wire.read();
      return value;
    }
  };
}

#endif
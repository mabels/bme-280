#include <Arduino.h>

#include "register.h"
#include "../i2c/io.h"
#include "calibration-data.h"

#define BME280_ADDRESS                (0x76)

namespace BME280 {
class Sensor {
  public:
    Sensor(I2C::Io &_io): io(_io) {
    }

    bool begin() {
      return begin(D1, D2);
    }

    bool begin(uint8_t sdaPin, uint8_t sclPin) {
      io.begin(sdaPin, sclPin, BME280_ADDRESS);

      sensorId = io.read8(BME280_REGISTER_CHIPID);
      Serial.println(sensorId);
      if (sensorId != 0x60) {
        return false;
      }
  
      readCoefficients();
      io.write8(BME280_REGISTER_CONTROL_HUMID, 0x1);
      io.write8(BME280_REGISTER_CONTROL, 0x3F);
      return true;
    }

    float readTemperature(void)
    {
      float T  = (t_fine() * 5 + 128) >> 8;
      return T/100;
    }

    float readPressure(void) {
      // Must be done first to get the t_fine variable set up
      auto t_fine = readTemperature();

      int32_t adc_P = io.read24(BME280_REGISTER_PRESSUREDATA);
      adc_P >>= 4;

      int64_t var1 = ((int64_t)t_fine) - 128000;
      int64_t var2 = var1 * var1 * (int64_t)calibrationData.dig_P6;
      var2 = var2 + ((var1*(int64_t)calibrationData.dig_P5)<<17);
      var2 = var2 + (((int64_t)calibrationData.dig_P4)<<35);
      var1 = ((var1 * var1 * (int64_t)calibrationData.dig_P3)>>8) +
        ((var1 * (int64_t)calibrationData.dig_P2)<<12);
      var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calibrationData.dig_P1)>>33;

      if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
      }
      int64_t p = 1048576 - adc_P;
      p = (((p<<31) - var2)*3125) / var1;
      var1 = (((int64_t)calibrationData.dig_P9) * (p>>13) * (p>>13)) >> 25;
      var2 = (((int64_t)calibrationData.dig_P8) * p) >> 19;

      p = ((p + var1 + var2) >> 8) + (((int64_t)calibrationData.dig_P7)<<4);
      return (float)p/256;
    }

    float readAltitude(float seaLevelhPa = 1013.25) {
      float pressure = readPressure(); // in Si units for Pascal
      pressure /= 100;
      float altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
      return altitude;
    }

    float readHumid() {
      int32_t humid = io.read16(BME280_REGISTER_HUMID);
      return humid;
    }


  private:
    int32_t t_fine() {
      int32_t adc_T = io.read24(BME280_REGISTER_TEMPDATA);
      adc_T >>= 4;
      int32_t var1 = ((((adc_T>>3) - ((int32_t)calibrationData.dig_T1 <<1))) *
        ((int32_t)calibrationData.dig_T2)) >> 11;

      int32_t var2  = (((((adc_T>>4) - ((int32_t)calibrationData.dig_T1)) *
          ((adc_T>>4) - ((int32_t)calibrationData.dig_T1))) >> 12) *
        ((int32_t)calibrationData.dig_T3)) >> 14;

      return var1 + var2;
    }

    
    void readCoefficients(void)
    {
        calibrationData.dig_T1 = io.read16_LE(BME280_REGISTER_DIG_T1);
        calibrationData.dig_T2 = io.readS16_LE(BME280_REGISTER_DIG_T2);
        calibrationData.dig_T3 = io.readS16_LE(BME280_REGISTER_DIG_T3);

        calibrationData.dig_P1 = io.read16_LE(BME280_REGISTER_DIG_P1);
        calibrationData.dig_P2 = io.readS16_LE(BME280_REGISTER_DIG_P2);
        calibrationData.dig_P3 = io.readS16_LE(BME280_REGISTER_DIG_P3);
        calibrationData.dig_P4 = io.readS16_LE(BME280_REGISTER_DIG_P4);
        calibrationData.dig_P5 = io.readS16_LE(BME280_REGISTER_DIG_P5);
        calibrationData.dig_P6 = io.readS16_LE(BME280_REGISTER_DIG_P6);
        calibrationData.dig_P7 = io.readS16_LE(BME280_REGISTER_DIG_P7);
        calibrationData.dig_P8 = io.readS16_LE(BME280_REGISTER_DIG_P8);
        calibrationData.dig_P9 = io.readS16_LE(BME280_REGISTER_DIG_P9);
    }
    I2C::Io io;
    int8_t sensorId;
    
    CalibrationData calibrationData;
};
}

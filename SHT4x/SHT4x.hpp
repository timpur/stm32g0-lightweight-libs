#pragma once

#include <Arduino.h>
#include <TinyWire.hpp>

#define SHT4x_I2C_ADDR 0x44

#define SHT4x_REG_NOHEAT_HIGHPRECISION 0xFD
#define SHT4x_REG_NOHEAT_MEDPRECISION 0xF6
#define SHT4x_REG_NOHEAT_LOWPRECISION 0xE0

#define SHT4x_REG_HIGHHEAT_1S 0x39
#define SHT4x_REG_HIGHHEAT_100MS 0x32
#define SHT4x_REG_MEDHEAT_1S 0x2F
#define SHT4x_REG_MEDHEAT_100MS 0x24
#define SHT4x_REG_LOWHEAT_1S 0x1E
#define SHT4x_REG_LOWHEAT_100MS 0x15

#define SHT4x_REG_READSERIAL 0x89
#define SHT4x_REG_SOFTRESET 0x94

static uint8_t crc8(const uint8_t *data, int len);

class SHT4x {
  private:
  public:
    SHT4x() {}
    bool begin() {
        // Check that we can get the ID (not going to parse it to save code)
        uint8_t id[6];
        if (!Wire.requestFrom(SHT4x_I2C_ADDR, SHT4x_REG_READSERIAL, 6, id, sizeof(id)))
            return false;

        // Reset the sensor
        if (!Wire.write(SHT4x_I2C_ADDR, SHT4x_REG_SOFTRESET, nullptr, 0))
            return false;
        delay(1);
        return true;
    }

    bool readTempHumidity(uint16_t *temperature, uint16_t *humidity, uint8_t cmd = SHT4x_REG_NOHEAT_HIGHPRECISION) {
        uint8_t buff[6];
        uint16_t delay;
        switch (cmd) {
        case SHT4x_REG_NOHEAT_HIGHPRECISION:
            delay = 10;
            break;
        case SHT4x_REG_NOHEAT_MEDPRECISION:
            delay = 6;
            break;
        case SHT4x_REG_NOHEAT_LOWPRECISION:
            delay = 3;
            break;
        case SHT4x_REG_HIGHHEAT_1S:
        case SHT4x_REG_MEDHEAT_1S:
        case SHT4x_REG_LOWHEAT_1S:
            delay = 1100;
            break;
        case SHT4x_REG_HIGHHEAT_100MS:
        case SHT4x_REG_MEDHEAT_100MS:
        case SHT4x_REG_LOWHEAT_100MS:
            delay = 110;
            break;
        }
        // Send command with the right wait to read the results (blocking)
        if (!Wire.requestFrom(SHT4x_I2C_ADDR, cmd, 6, buff, sizeof(buff), delay))
            return false;

        // Check CRC of the results
        if (buff[2] != crc8(buff, 2) || buff[5] != crc8(&buff[3], 2))
            return false;

        uint16_t t_ticks = (uint16_t)buff[0] << 8 + (uint16_t)buff[1];
        uint16_t rh_ticks = (uint16_t)buff[3] << 8 + (uint16_t)buff[4];
        float _temperature = -45 + 175 * t_ticks / 65535;
        float _humidity = -6 + 125 * rh_ticks / 65535;

        return true;
    }
};

static uint8_t crc8(const uint8_t *data, int len) {
    const uint8_t POLYNOMIAL(0x31);
    uint8_t crc(0xFF);

    for (int j = len; j; --j) {
        crc ^= *data++;

        for (int i = 8; i; --i) {
            crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
        }
    }
    return crc;
}
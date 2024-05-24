#pragma once

#include <Arduino.h>
#include <TinyWire.hpp>

#define APDS9930_I2C_ADDR 0x39

#define APDS9930_CMD__REPEATED_BYTE 0x80
#define APDS9930_CMD_AUTO_INCREMENT 0xA0
#define APDS9930_CMD_SPECIAL_FN 0xE0

#define APDS9930_REG_ENABLE 0x00
#define APDS9930_REG_PTIME 0x02
#define APDS9930_REG_CONFIG 0x0D
#define APDS9930_REG_PPULSE 0x0E
#define APDS9930_REG_CONTROL 0x0F
#define APDS9930_REG_ID 0x12
#define APDS9930_REG_PDATAL 0x18

#define APDS9930_PROX_WAIT 32
#define APDS9930_PROX_WAIT_TOTAL_MS                                                                                    \
    int(0.128 + 2.73 + (2.73 * APDS9930_PROX_WAIT)) // Pulse (PPULSE) + Fixed Wait + Wait (PTIME)

class APDS9930 {
  private:
  public:
    bool begin() {
        uint8_t val;

        // Validate Sensors is on the bus
        uint8_t id;
        if (!Wire.requestFrom(APDS9930_I2C_ADDR, APDS9930_REG_ID, 1, &id, 1) && id == APDS9930_I2C_ADDR)
            return false;

        // Setup Proximity Sensing

        // Set Proximity Wait Time
        val = (0xff - APDS9930_PROX_WAIT);
        if (!Wire.write(APDS9930_I2C_ADDR, APDS9930_REG_PTIME, &val, 1))
            return false;

        // Set Config, PDL=1
        // val = 1;
        // if (!Wire.write(APDS9930_I2C_ADDR, APDS9930_REG_CONFIG, &val, 1))
        //     return false;

        // Set Pulse Count to 8
        val = 8;
        if (!Wire.write(APDS9930_I2C_ADDR, APDS9930_REG_PPULSE, &val, 1))
            return false;

        // Set Control Reg, enable PDRIVE=100ma PDIODE=ch1 PGAIN=8x AGAIN=1x
        val = (0B00 << 6 | 0B10 << 4 | 0B11 << 2 | 0b00 << 0);
        if (!Wire.write(APDS9930_I2C_ADDR, APDS9930_REG_CONTROL, &val, 1))
            return false;

        // Set Enable Reg, PEN=1, PON=1
        val = (0B1 << 2 | 0B1 << 0);
        if (!Wire.write(APDS9930_I2C_ADDR, APDS9930_REG_ENABLE, &val, 1))
            return false;

        // End Setup Proximity Sensing

        return true;
    }

    bool readProximity(uint16_t *proximity) {
        uint8_t buff[2];
        if (!Wire.requestFrom(APDS9930_I2C_ADDR, APDS9930_REG_PDATAL, 2, buff, sizeof(buff)))
            return false;

        *proximity = (uint16_t)buff[1] << 8 + (uint16_t)buff[0];
        return true;
    }
};
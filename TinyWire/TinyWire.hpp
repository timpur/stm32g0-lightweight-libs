#pragma once

#include <Arduino.h>
#include <utility/twi.h>

class TinyWire
{
private:
    i2c_t _i2c;

public:
    TinyWire() : TinyWire(SDA, SCL) {}

    TinyWire(uint32_t sda, uint32_t scl)
    {
        memset((void *)&_i2c, 0, sizeof(_i2c));
        _i2c.sda = digitalPinToPinName(sda);
        _i2c.scl = digitalPinToPinName(scl);
    }

    ~TinyWire()
    {
        i2c_deinit(&_i2c);
    }

    void begin()
    {
        _i2c.__this = (void *)this;
        _i2c.isMaster = 1;
        _i2c.generalCall = 0;
        _i2c.NoStretchMode = 0;
        i2c_custom_init(&_i2c, 100000, I2C_ADDRESSINGMODE_7BIT, 0);
    }

    bool write(uint8_t address, uint8_t iaddress, uint8_t *data, uint8_t len)
    {
        if (len > 16)
            return false;
        uint8_t buff[1 + 16] = {};
        memcpy(&buff[1], data, len);
        _i2c.handle.XferOptions = I2C_OTHER_AND_LAST_FRAME;
        i2c_status_e res = i2c_master_write(&_i2c, address << 1, buff, 1 + len);
        return res == I2C_OK;
    }

    uint8_t requestFrom(uint8_t address, uint8_t iaddress, uint8_t quantity, uint8_t *buff, uint8_t buff_len, uint16_t delay_ms = 0)
    {
        if (quantity > buff_len)
            return false;

        _i2c.handle.XferOptions = I2C_OTHER_FRAME;
        i2c_master_write(&_i2c, address, &iaddress, 1);

        if (delay_ms > 0)
            delay(delay_ms);

        _i2c.handle.XferOptions = I2C_OTHER_AND_LAST_FRAME;
        i2c_status_e res = i2c_master_read(&_i2c, address << 1, buff, quantity);
        return res == I2C_OK ? quantity : 0;
    }
};

TinyWire Wire;
#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#define IFLOAT_DEFAULT_EXPONENT 2

static int pow(int x, int n);
// static uint abs(int v);

struct IFloat;
typedef IFloat ifloat_t;

struct IFloat {
  private:
    int _val = 0;
    uint8_t _exp = IFLOAT_DEFAULT_EXPONENT;

    int _normalise(int val, uint8_t exp) {
        int exp_dif = _exp - exp;
        if (exp_dif > 0)
            return val * pow(10, exp_dif);
        else if (exp_dif < 0)
            return val / pow(10, abs(exp_dif));
        return val;
    }

  public:
    IFloat(const int val = 0, const uint8_t val_exp = 0, const uint8_t exp = IFLOAT_DEFAULT_EXPONENT) {
        _exp = exp;
        if (val != 0)
            _val = _normalise(val, val_exp);
    }

    IFloat &add(int val, uint8_t exp = 0) {
        int v = _normalise(val, exp);
        _val += v;
        return *this;
    }

    IFloat &subtract(int val, uint8_t exp = 0) {
        val = _normalise(val, exp);
        _val -= val;
        return *this;
    }

    IFloat &multiply(int val, uint8_t exp = 0) {
        val = _normalise(val, exp);
        _val = _normalise(_val * val, _exp + 1);
        return *this;
    }

    IFloat &divide(int val, uint8_t exp = 0) {
        val = _normalise(val, exp);
        _val = _normalise(_val * val, _exp - 1);
        return *this;
    }

    IFloat operator=(int lhs) { return IFloat(lhs, 0); }

    IFloat &operator+(IFloat &lhs) { return *this += lhs; }
    IFloat &operator-(IFloat &lhs) { return *this -= lhs; }
    IFloat &operator*(IFloat lhs) { return *this *= lhs; }
    IFloat &operator/(IFloat &lhs) { return *this /= lhs; }

    IFloat &operator+=(IFloat &lhs) { return add(lhs._val, lhs._exp); }
    IFloat &operator-=(IFloat &lhs) { return subtract(lhs._val, lhs._exp); }
    IFloat &operator*=(IFloat &lhs) { return multiply(lhs._val, lhs._exp); }
    IFloat &operator/=(IFloat &lhs) { return divide(lhs._val, lhs._exp); }
};

IFloat &operator+(IFloat &rhs, int lhs) { return rhs.add(lhs); }
IFloat &operator+(int lhs, IFloat &rhs) { return rhs.add(lhs); }
IFloat &operator-(IFloat &rhs, int lhs) { return rhs.subtract(lhs); }
IFloat &operator-(int lhs, IFloat &rhs) { return rhs.subtract(lhs); }
IFloat &operator*(IFloat &rhs, int lhs) { return rhs.multiply(lhs); }
IFloat &operator*(int lhs, IFloat &rhs) { return rhs.multiply(lhs); }
IFloat &operator/(IFloat &rhs, int lhs) { return rhs.divide(lhs); }
IFloat &operator/(int lhs, IFloat &rhs) { return rhs.divide(lhs); }

static int pow(int x, int n) {
    int number = 1;
    for (int i = 0; i < n; ++i)
        number *= x;
    return (number);
}

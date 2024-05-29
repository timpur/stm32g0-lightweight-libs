#pragma once

// #include <Arduino.h>

#include <inttypes.h>
#include <stdlib.h>

#ifdef UNITY
#include <string>
#endif

static constexpr int iabs(int v);
static constexpr int ipow(int x, int n);
static constexpr int ilog(int x, int b);

#define IFLOAT_DEFAULT_EXPONENT 2

struct IFloat;
typedef IFloat ifloat_t;

struct IFloat {
  private:
    int _val = 0;
    uint8_t _exp = IFLOAT_DEFAULT_EXPONENT;

    int _normalise(int val, uint8_t exp) {
        int exp_dif = _exp - exp;
        if (exp_dif > 0)
            return val * ipow(10, exp_dif);
        else if (exp_dif < 0)
            return val / ipow(10, iabs(exp_dif));
        return val;
    }

  public:
    IFloat(IFloat &val) {
        _val = val._val;
        _exp = val._exp;
    }

    IFloat(const int val = 0, const uint8_t val_exp = 0, const uint8_t exp = IFLOAT_DEFAULT_EXPONENT) {
        _exp = exp;
        if (val != 0)
            _val = _normalise(val, val_exp);
    }

    int rawValue() { return _val; }
    int intValue() { return _val / ipow(10, _exp); }
    // Decimal value needs to be sifter by the exponent. eg 1 with exp 2 -> .01
    uint32_t decimanValue() { return _val % ipow(10, _exp); }
    uint8_t decimalExponent() { return _exp; }

#ifdef HAL_UART_MODULE_ENABLED
    void print(Print &f) {
        unsigned int i, d;
        i = intValue();
        d = decimanValue();
        f.print(i);
        if (_exp > 0) {
            f.print(F("."));
            uint8_t shift = _exp - ilog(d, 10);
            while (shift++ > 0)
                f.print(F("0"));
            f.print(d);
        }
    }
#endif

#ifdef UNITY
    std::string toString() {
        std::string f;
        unsigned int i, d;
        i = intValue();
        d = decimanValue();
        f.append(std::to_string(i));
        if (_exp > 0) {
            f.append(".");
            if (d < ipow(10, _exp - 1))
                f.append("0");
            f.append(std::to_string(d));
        }
    }
#endif

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

    ifloat_t operator=(int lhs) { return IFloat(lhs, 0); }

    ifloat_t operator+(ifloat_t lhs) { return IFloat(*this).add(lhs._val, lhs._exp); }
    ifloat_t operator-(ifloat_t lhs) { return IFloat(*this).subtract(lhs._val, lhs._exp); }
    ifloat_t operator*(ifloat_t lhs) { return IFloat(*this).multiply(lhs._val, lhs._exp); }
    ifloat_t operator/(ifloat_t lhs) { return IFloat(*this).divide(lhs._val, lhs._exp); }

    ifloat_t &operator+=(ifloat_t lhs) { return add(lhs._val, lhs._exp); }
    ifloat_t &operator-=(ifloat_t lhs) { return subtract(lhs._val, lhs._exp); }
    ifloat_t &operator*=(ifloat_t lhs) { return multiply(lhs._val, lhs._exp); }
    ifloat_t &operator/=(ifloat_t lhs) { return divide(lhs._val, lhs._exp); }
};

ifloat_t operator+(ifloat_t rhs, int lhs) { return IFloat(rhs).add(lhs); }
ifloat_t operator+(int rhs, ifloat_t lhs) { return IFloat(lhs).add(rhs); }
ifloat_t operator-(ifloat_t rhs, int lhs) { return IFloat(rhs).subtract(lhs); }
ifloat_t operator-(int rhs, ifloat_t lhs) { return IFloat(lhs).subtract(rhs); }
ifloat_t operator*(ifloat_t rhs, int lhs) { return IFloat(rhs).multiply(lhs); }
ifloat_t operator*(int rhs, ifloat_t lhs) { return IFloat(lhs).multiply(rhs); }
ifloat_t operator/(ifloat_t rhs, int lhs) { return IFloat(rhs).divide(lhs); }
ifloat_t operator/(int rhs, ifloat_t lhs) { return IFloat(lhs).divide(rhs); }

static constexpr int iabs(int v) {
    if (v < 0)
        return v * -1;
    return v;
}

static constexpr int ipow(int x, int n) {
    int number = 1;
    for (int i = 0; i < n; ++i)
        number *= x;
    return (number);
}

static constexpr int ilog(int x, int b) {
    if (x < b)
        return 0;
    int i = 0;
    do {
        x /= b;
        i++;
    } while (x > 0);

    return i;
}
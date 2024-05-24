#pragma once

#include <Arduino.h>

extern "C" {
constexpr int iabs(int v);
constexpr int ipow(int x, int n);
}

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
    IFloat(const int val = 0, const uint8_t val_exp = 0, const uint8_t exp = IFLOAT_DEFAULT_EXPONENT) {
        _exp = exp;
        if (val != 0)
            _val = _normalise(val, val_exp);
    }

    unsigned int intValue() { return _val / ipow(10, _exp); }
    unsigned int decimanValue() { return _val % ipow(10, _exp); }

    void print(Print &f) {
        unsigned int i, d;
        i = intValue();
        d = decimanValue();
        f.print(i);
        if (_exp > 0) {
            f.print(F("."));
            if (d < ipow(10, _exp - 1))
                f.print(F("0"));
            f.print(d);
        }
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

    ifloat_t &operator+(ifloat_t lhs) { return add(lhs._val, lhs._exp); }
    ifloat_t &operator-(ifloat_t lhs) { return subtract(lhs._val, lhs._exp); }
    ifloat_t &operator*(ifloat_t lhs) { return multiply(lhs._val, lhs._exp); }
    ifloat_t &operator/(ifloat_t lhs) { return divide(lhs._val, lhs._exp); }

    ifloat_t &operator+=(ifloat_t lhs) { return add(lhs._val, lhs._exp); }
    ifloat_t &operator-=(ifloat_t lhs) { return subtract(lhs._val, lhs._exp); }
    ifloat_t &operator*=(ifloat_t lhs) { return multiply(lhs._val, lhs._exp); }
    ifloat_t &operator/=(ifloat_t lhs) { return divide(lhs._val, lhs._exp); }
};

ifloat_t &operator+(ifloat_t &rhs, int lhs) { return rhs.add(lhs); }
ifloat_t &operator+(int lhs, ifloat_t &rhs) { return rhs.add(lhs); }
ifloat_t &operator-(ifloat_t &rhs, int lhs) { return rhs.subtract(lhs); }
ifloat_t &operator-(int lhs, ifloat_t &rhs) { return rhs.subtract(lhs); }
ifloat_t &operator*(ifloat_t rhs, int lhs) { return rhs.multiply(lhs); }
ifloat_t &operator*(int lhs, ifloat_t &rhs) { return rhs.multiply(lhs); }
ifloat_t &operator/(ifloat_t &rhs, int lhs) { return rhs.divide(lhs); }
ifloat_t &operator/(int lhs, ifloat_t &rhs) { return rhs.divide(lhs); }

extern "C" {
constexpr int iabs(int v) {
    if (v < 0)
        return v * -1;
    return v;
}

constexpr int ipow(int x, int n) {
    int number = 1;
    for (int i = 0; i < n; ++i)
        number *= x;
    return (number);
}
}

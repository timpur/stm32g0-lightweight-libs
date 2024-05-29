#pragma oncee

#include <Arduino.h>
#include <IFloat.hpp>

class TinyPid {
  private:
    ifloat_t _prev_err = 0;
    ifloat_t _i_err = 0;
    uint32_t _prev_time;
    unsigned int _interval = -1;

  public:
    ifloat_t kP;
    ifloat_t kI;
    ifloat_t kD;
    ifloat_t target;

    void begin(ifloat_t p, ifloat_t i, ifloat_t d, unsigned int interval_ms = 100) {
        _prev_time = millis();
        _interval = interval_ms;
        kP = p;
        kI = i;
        kD = d;
    }

    ifloat_t update(ifloat_t input) {
        if (_interval == 0)
            return -1;

        uint32_t now = millis();
        uint32_t time_diff = now - _prev_time;

        if (time_diff < _interval)
            return -1;

        ifloat_t p_error = target - input;
        ifloat_t d_error = (_prev_err - p_error) / time_diff;

        _i_err += kI * p_error * _interval;
        ifloat_t out = (kP * p_error + kD * d_error) / _i_err;

        _prev_time = now;
        _prev_err = p_error;

        return out;
    }
};

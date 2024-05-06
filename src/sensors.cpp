#include "sensors.hpp"

#include "voltage.hpp"

#include <math.h>

namespace
{
float fpl(float x, float a, float b, float c, float d)
{
    return d + (a - d) / (1.f + powf(x / c, b));
}

constexpr auto rawLimitLow = 2;
constexpr auto rawLimitHigh = 1022;

constexpr auto ectLimitLow = 20.f;
constexpr auto ectLimitHigh = 150.f;

bool ectDirty = false;
int16_t ectRaw = 0;
float ectCalculated = 0;

float ectCalculate()
{
    static constexpr float a = 1898162.f, b = 0.4399849f, c = 1.866142e-10f, d = -29.38087f;
    const auto v = ectRaw * RAW_ADC_TO_VOLT * foreignVoltageCorrection();
    return fpl(v, a, b, c, d);
}

constexpr auto eotLimitLow = 20.f;
constexpr auto eotLimitHigh = 200.f;

bool eotDirty = false;
int16_t eotRaw = 0;
float eotCalculated = 0;

float eotCalculate()
{
    static constexpr float a = 482.649, b = 0.2944, c = 1255.1238, d = -446.6494;
    return fpl(eotRaw, a, b, c, d);
}
} // namespace

void ectUpdateRaw(int16_t value)
{
    ectRaw = value;
    ectDirty = true;
}

float ectGetCelsius()
{
    if (ectDirty)
    {
        ectCalculated = ectCalculate();
        ectDirty = false;
    }

    return ectCalculated;
}

float ectGetVolt()
{
    return ectRaw * RAW_ADC_TO_VOLT;
}

int16_t ectGetRaw()
{
    return ectRaw;
}

SensorRange ectIsValid()
{
    const auto t = ectGetCelsius();

    if (ectRaw > rawLimitHigh)
    {
        return SensorRange::TOO_LOW;
    }

    if (ectRaw < rawLimitLow)
    {
        return SensorRange::TOO_HIGH;
    }

    if (t < ectLimitLow)
    {
        return SensorRange::TOO_LOW;
    }

    if (t > ectLimitHigh)
    {
        return SensorRange::TOO_HIGH;
    }

    return SensorRange::OK;
}

void eotUpdateRaw(int16_t value)
{
    eotRaw = value;
    eotDirty = true;
}

float eotGetCelsius()
{
    if (eotDirty)
    {
        eotCalculated = eotCalculate();
        eotDirty = false;
    }

    return eotCalculated;
}

float eotGetVolt()
{
    return eotRaw * RAW_ADC_TO_VOLT;
}

int16_t eotGetRaw()
{
    return eotRaw;
}

SensorRange eotIsValid()
{
    const auto t = eotGetCelsius();

    if (eotRaw > rawLimitHigh)
    {
        return SensorRange::TOO_LOW;
    }

    if (eotRaw < rawLimitLow)
    {
        return SensorRange::TOO_HIGH;
    }

    if (t < eotLimitLow)
    {
        return SensorRange::TOO_LOW;
    }

    if (t > eotLimitHigh)
    {
        return SensorRange::TOO_HIGH;
    }
}

#include "ect.hpp"

#include "voltage.hpp"

#include <math.h>

namespace
{
bool dirty = false;
int16_t ectRaw = 0;
float ectCalculated = 0;

float ectCalculate()
{
    // symmetrical sigmoid 4PL coefficients
    constexpr float a = 1898162.f, b = 0.4399849f, c = 1.866142e-10f, d = -29.38087f;

    const auto v = ectRaw * RAW_ADC_TO_VOLT * voltageCorrection();
    return d + (a - d) / (1.f + powf(v / c, b));
}
} // namespace

void ectUpdateRaw(int16_t value)
{
    ectRaw = value;
    dirty = true;
}

float ectGetCelsius()
{
    if (dirty)
    {
        ectCalculated = ectCalculate();
        dirty = false;
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

EctRange ectIsValid()
{
    const auto t = ectGetCelsius();

    if (ectRaw >= 1023)
    {
        return EctRange::TOO_LOW;
    }

    if (ectRaw <= 1)
    {
        return EctRange::TOO_HIGH;
    }

    if (t < 20.f)
    {
        return EctRange::TOO_LOW;
    }

    if (t > 150.f)
    {
        return EctRange::TOO_HIGH;
    }

    return EctRange::OK;
}

#include "sensors.hpp"

#include "Arduino.h"
#include "voltage.hpp"

namespace
{
float fpl(float x, float a, float b, float c, float d)
{
    return d + (a - d) / (1.f + powf(x / c, b));
}

float poly(float x, float coeff)
{
    return coeff;
}

template <typename... T> float poly(float x, float coeff, T... values)
{
    return coeff + x * poly(x, values...);
}

constexpr auto rawLimitLow = 2;
constexpr auto rawLimitHigh = 1022;
} // namespace

namespace
{
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

namespace
{
constexpr auto eotLimitLow = 20.f;
constexpr auto eotLimitHigh = 200.f;

bool eotDirty = false;
int16_t eotRaw = 0;
float eotCalculated = 0;

float eotCalculate()
{
    static constexpr float a = 1.6911545901523502e+002f;
    static constexpr float b = -2.3234353162296584e-001f;
    static constexpr float c = 1.3584517967950071e-004f;
    static constexpr float d = -2.5465668950594277e-009f;
    static constexpr float e = -4.0678862370423916e-011f;

    return poly(eotRaw, a, b, c, d, e);
}
} // namespace

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

    return SensorRange::OK;
}

namespace
{
constexpr auto eopLimitLow = -0.01f;
constexpr auto eopLimitHigh = 10.1f;

bool eopDirty = false;
int16_t eopRaw = 0;
float eopCalculated = 0;

float eopCalculate()
{
    // assumes presence of 0.5 multiplier voltage divider
    return poly(eopRaw * RAW_ADC_TO_VOLT * 2.f, -1.25f, 2.5f);
}
} // namespace

void eopUpdateRaw(int16_t value)
{
    eopRaw = value;
    eopDirty = true;
}

float eopGetBar()
{
    if (eopDirty)
    {
        eopCalculated = eopCalculate();
        eopDirty = false;
    }

    return eopCalculated;
}

float eopGetVolt()
{
    return eopRaw * RAW_ADC_TO_VOLT;
}

int16_t eopGetRaw()
{
    return eopRaw;
}

SensorRange eopIsValid()
{
    const auto t = eopGetBar();

    if (eopRaw < rawLimitLow)
    {
        return SensorRange::TOO_LOW;
    }

    if (eopRaw > rawLimitHigh)
    {
        return SensorRange::TOO_HIGH;
    }

    if (t < eopLimitLow)
    {
        return SensorRange::TOO_LOW;
    }

    if (t > eopLimitHigh)
    {
        return SensorRange::TOO_HIGH;
    }

    return SensorRange::OK;
}

int hardenedAnalogRead(uint8_t pin)
{
    static constexpr auto readCount = 11;
    static_assert(readCount % 2 == 1, "Must be an odd number");

    int reads[readCount];

    for (auto &read : reads)
    {
        read = analogRead(pin);
    }

    qsort(reads, readCount, sizeof(*reads), [](const void *pLhs, const void *pRhs) {
        const auto lhs = *reinterpret_cast<const int *>(pLhs);
        const auto rhs = *reinterpret_cast<const int *>(pRhs);
        return lhs < rhs ? -1 : (lhs > rhs ? 1 : 0);
    });

    return reads[readCount / 2];
}

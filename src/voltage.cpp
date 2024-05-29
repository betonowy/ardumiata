#include "voltage.hpp"
#include "Arduino.h"

namespace
{
    constexpr auto batteryVoltageMultiplier = 8.02f;
    constexpr auto batteryCenterVoltage = 14.7f;
    constexpr auto batteryVoltageCorrectionMultiplier = 0.6f;
}

float getVoltage()
{
    return analogRead(A3) * RAW_ADC_TO_VOLT * batteryVoltageMultiplier;
}

float foreignVoltageCorrection()
{
    return 1 + batteryVoltageCorrectionMultiplier * (1 - getVoltage() / batteryCenterVoltage);
}

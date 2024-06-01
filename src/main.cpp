#include <Arduino.h>

#include "buzzer.hpp"
#include "display.hpp"
#include "sensors.hpp"
#include "voltage.hpp"

void setup()
{
    analogReference(EXTERNAL);
    setupBuzzer();
    setupDisplay();

    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    // Beep to signal setup completion
    setBuzzer(true);
    delay(30);
    setBuzzer(false);
}

void updatePeriodicalReadings()
{
    ectUpdateRaw(hardenedAnalogRead(A0));
    eotUpdateRaw(hardenedAnalogRead(A1));
    eopUpdateRaw(hardenedAnalogRead(A2));

    {
        setSlotIcon(DisplaySlot::UL, Icon::OIL, Icon::TEMP);

        switch (eotIsValid())
        {
        case SensorRange::OK:
            setSlot(DisplaySlot::UL, eotGetCelsius());
            break;
        case SensorRange::TOO_LOW:
            setSlot(DisplaySlot::UL, "Cold");
            break;
        case SensorRange::TOO_HIGH:
            setSlot(DisplaySlot::UL, "Hot");
        }
    }
    {
        setSlotIcon(DisplaySlot::UR, Icon::OIL, Icon::PRESSURE);

        switch (eopIsValid())
        {
        case SensorRange::OK:
        case SensorRange::OK_DISABLE_ALARM:
            setSlot(DisplaySlot::UR, eopGetBar());
            break;
        case SensorRange::TOO_LOW:
            setSlot(DisplaySlot::UR, 0.0f);
            break;
        case SensorRange::TOO_HIGH:
            setSlot(DisplaySlot::UR, 10.0f);
        }
    }
    {
        setSlotIcon(DisplaySlot::BL, Icon::WATER, Icon::TEMP);

        switch (ectIsValid())
        {
        case SensorRange::OK:
            setSlot(DisplaySlot::BL, ectGetCelsius());
            break;
        case SensorRange::TOO_LOW:
            setSlot(DisplaySlot::BL, "Cold");
            break;
        case SensorRange::TOO_HIGH:
            setSlot(DisplaySlot::BL, "Hot");
        }
    }
    {
        setSlotIcon(DisplaySlot::BR, Icon::BAT_A, Icon::BAT_B);
        setSlot(DisplaySlot::BR, getVoltage());
    }
}

namespace
{
constexpr auto safeCoolantTempLimit = 115.f;
constexpr auto safeOilTempLimit = 130.f;
constexpr auto safePressureLimitLow = 0.65f;
constexpr auto safePressureLimitHigh = 5.0f;
} // namespace

void updateAlarm()
{
    bool turnOnAlarm = false;

    turnOnAlarm |= ectGetCelsius() > safeCoolantTempLimit && ectIsValid() == SensorRange::OK;
    turnOnAlarm |= eotGetCelsius() > safeOilTempLimit && eotIsValid() == SensorRange::OK;
    turnOnAlarm |= eopGetBar() > safePressureLimitHigh && eopIsValid() == SensorRange::OK;
    turnOnAlarm |= eopGetBar() < safePressureLimitLow && eopIsValid() == SensorRange::OK;

    if (turnOnAlarm)
    {
        setBuzzer(true);
    }
    else
    {
        setBuzzer(false);
    }
}

unsigned long nextUpdateTime = 0;
unsigned long screenDelay = 250;

bool shouldUpdateDisplay()
{
    const uint32_t time = millis();

    if (static_cast<int32_t>(nextUpdateTime - time) > 0)
    {
        return false;
    }

    nextUpdateTime = time + screenDelay;

    return true;
}

void loop()
{
    if (shouldUpdateDisplay())
    {
        updatePeriodicalReadings();
        updateDisplay();
        updateAlarm();
    }

    loopBuzzer();
}

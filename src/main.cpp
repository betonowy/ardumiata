#include <Arduino.h>

#include "buzzer.hpp"
#include "display.hpp"
#include "sensors.hpp"
#include "voltage.hpp"

namespace
{
// constexpr uint8_t iconTemp = 0;
// constexpr uint8_t charmapTemp[8] = {
//     0b00100, 0b00110, 0b00100, 0b00110, 0b00100, 0b00100, 0b01010, 0b00100,
// };

// constexpr uint8_t iconOil = 1;
// constexpr uint8_t charmapOil[8] = {
//     0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110,
// };

// constexpr uint8_t iconPressure = 2;
// constexpr uint8_t charmapPressure[8] = {
//     0b01110, 0b01110, 0b01110, 0b11111, 0b01110, 0b00100, 0b10001, 0b01110,
// };

// constexpr uint8_t iconWater = 3;
// constexpr uint8_t charmapWater[8] = {
//     0b10000, 0b11111, 0b10001, 0b11111, 0b10001, 0b11111, 0b00001, 0b00000,
// };

// constexpr uint8_t iconRpmA = 4;
// constexpr uint8_t charmapRpmA[8] = {
//     0b00000, 0b00000, 0b00111, 0b01000, 0b10000, 0b10000, 0b10000, 0b00000,
// };

// constexpr uint8_t iconRpmB = 5;
// constexpr uint8_t charmapRpmB[8] = {
//     0b00000, 0b00000, 0b11100, 0b00010, 0b00101, 0b01001, 0b10001, 0b00000,
// };

// constexpr uint8_t iconBatA = 6;
// constexpr uint8_t charmapBatA[8] = {
//     0b00000, 0b01100, 0b11111, 0b10000, 0b10010, 0b10111, 0b10010, 0b10000,
// };

// constexpr uint8_t iconBatB = 7;
// constexpr uint8_t charmapBatB[8] = {
//     0b00000, 0b00110, 0b11111, 0b00001, 0b00001, 0b11101, 0b00001, 0b00001,
// };

constexpr auto safeCoolantTempLimit = 115.f;
} // namespace

void setup()
{
    analogReference(EXTERNAL);
    setupBuzzer();
    setupDisplay();

    // Beep to signal setup completion
    setBuzzer(true);
    delay(30);
    setBuzzer(false);
}

void updatePeriodicalReadings()
{
    ectUpdateRaw(analogRead(A0));
    eotUpdateRaw(analogRead(A1));

    const auto ectVolt = ectGetVolt();
    const auto ectRaw = ectGetRaw();
    const auto ectCelsius = ectGetCelsius();
    const auto eotVolt = eotGetVolt();
    const auto eotRaw = eotGetRaw();
    const auto eotCelsius = eotGetCelsius();
    const auto batteryVolt = getVoltage();

    setSlotIcon(DisplaySlot::UL, Icon::OIL, Icon::TEMP);
    setSlot(DisplaySlot::UL, eotVolt);

    setSlotIcon(DisplaySlot::UR, Icon::OIL, Icon::PRESSURE);
    setSlot(DisplaySlot::UR, eotRaw);

    setSlotIcon(DisplaySlot::BR, Icon::BAT_A, Icon::BAT_B);
    setSlot(DisplaySlot::BR, batteryVolt);

    setSlotIcon(DisplaySlot::BL, Icon::WATER, Icon::TEMP);

    switch (ectIsValid())
    {
    case SensorRange::TOO_LOW:
        setSlot(DisplaySlot::BL, "Cold");
        break;
    case SensorRange::TOO_HIGH:
        setSlot(DisplaySlot::BL, "Hot");
        break;
    case SensorRange::OK:
        setSlot(DisplaySlot::BL, ectCelsius);
    }
}

void updateAlarm()
{
    if (ectGetCelsius() > safeCoolantTempLimit && ectIsValid() == SensorRange::OK)
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

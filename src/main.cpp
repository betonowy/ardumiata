#include <Arduino.h>

#include "buzzer.hpp"
#include "ect.hpp"
#include "lcdi2c.hpp"
#include "voltage.hpp"

LiquidCrystal_I2C lcd(0x3f, 16, 2);

namespace
{
constexpr uint8_t iconTemp = 0;
constexpr uint8_t charmapTemp[8] = {
    0b00100, 0b00110, 0b00100, 0b00110, 0b00100, 0b00100, 0b01010, 0b00100,
};

constexpr uint8_t iconOil = 1;
constexpr uint8_t charmapOil[8] = {
    0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110,
};

constexpr uint8_t iconPressure = 2;
constexpr uint8_t charmapPressure[8] = {
    0b01110, 0b01110, 0b01110, 0b11111, 0b01110, 0b00100, 0b10001, 0b01110,
};

constexpr uint8_t iconWater = 3;
constexpr uint8_t charmapWater[8] = {
    0b10000, 0b11111, 0b10001, 0b11111, 0b10001, 0b11111, 0b00001, 0b00000,
};

constexpr uint8_t iconRpmA = 4;
constexpr uint8_t charmapRpmA[8] = {
    0b00000, 0b00000, 0b00111, 0b01000, 0b10000, 0b10000, 0b10000, 0b00000,
};

constexpr uint8_t iconRpmB = 5;
constexpr uint8_t charmapRpmB[8] = {
    0b00000, 0b00000, 0b11100, 0b00010, 0b00101, 0b01001, 0b10001, 0b00000,
};

constexpr uint8_t iconBatA = 6;
constexpr uint8_t charmapBatA[8] = {
    0b00000, 0b01100, 0b11111, 0b10000, 0b10010, 0b10111, 0b10010, 0b10000,
};

constexpr uint8_t iconBatB = 7;
constexpr uint8_t charmapBatB[8] = {
    0b00000, 0b00110, 0b11111, 0b00001, 0b00001, 0b11101, 0b00001, 0b00001,
};

constexpr auto safeCoolantTempLimit = 115.f;
} // namespace

void setup()
{
    analogReference(EXTERNAL);
    setupBuzzer();

    lcd.begin();
    lcd.backlight();

    lcd.createChar(iconTemp, charmapTemp);
    lcd.createChar(iconPressure, charmapPressure);
    lcd.createChar(iconOil, charmapOil);
    lcd.createChar(iconWater, charmapWater);
    lcd.createChar(iconRpmA, charmapRpmA);
    lcd.createChar(iconRpmB, charmapRpmB);
    lcd.createChar(iconBatA, charmapBatA);
    lcd.createChar(iconBatB, charmapBatB);

    setBuzzer(true);
    delay(30);
    setBuzzer(false);
}

void updatePeriodicalReadings()
{
    ectUpdateRaw(analogRead(A0));
}

void updateAlarm()
{
    if (ectGetCelsius() > safeCoolantTempLimit && ectIsValid() == EctRange::OK)
    {
        setBuzzer(true);
    }
    else
    {
        setBuzzer(false);
    }
}

void updateDisplay()
{
    const auto voltMv = ectGetVolt();
    const auto voltDivResult = div(static_cast<int32_t>(round(voltMv * 1000.f)), 1000);

    const auto voltA = voltDivResult.quot;
    const auto voltB = voltDivResult.rem;

    const auto celsius = ectGetCelsius();
    const auto celsiusDivResult = div(static_cast<int32_t>(round(celsius * 10.f)), 10);

    const auto celsiusA = celsiusDivResult.quot;
    const auto celsiusB = celsiusDivResult.rem;

    const auto raw = ectGetRaw();

    lcd.clear();

    lcd.setCursor(0, 0);
    {
        lcd.write(iconOil);
        lcd.write(iconTemp);

        lcd.write(' ');

        lcd.print(voltA);
        lcd.print("V");

        if (voltB < 100)
        {
            lcd.print('0');
        }

        if (voltB < 10)
        {
            lcd.print('0');
        }

        lcd.print(voltB);
    }

    lcd.setCursor(9, 0);
    {
        lcd.write(iconOil);
        lcd.write(iconPressure);

        lcd.write(' ');

        if (raw < 1000)
        {
            lcd.write(' ');
        }

        if (raw < 100)
        {
            lcd.write(' ');
        }

        if (raw < 10)
        {
            lcd.write(' ');
        }

        lcd.print(raw);
    }

    lcd.setCursor(9, 1);
    {
        lcd.write(iconBatA);
        lcd.write(iconBatB);

        lcd.print(' ');
        lcd.print(getVoltage());
    }

    lcd.setCursor(0, 1);
    {
        const auto isValid = ectIsValid();

        lcd.write(iconWater);
        lcd.write(iconTemp);

        lcd.write(' ');

        switch (ectIsValid())
        {
        case EctRange::TOO_LOW:
            lcd.print("Cold");
            break;
        case EctRange::TOO_HIGH:
            lcd.print("Hot");
            break;
        case EctRange::OK:
            if (celsiusA < 100)
            {
                lcd.print(' ');
            }

            lcd.print(celsiusA);
            lcd.print('.');
            lcd.print(celsiusB);
        }
    }
}

unsigned long nextUpdateTime = 0;
unsigned long screenDelay = 1000;

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

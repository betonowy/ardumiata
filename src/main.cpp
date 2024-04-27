#include <Arduino.h>

#include "ect.hpp"
#include "lcdi2c.hpp"

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup()
{
    analogReference(EXTERNAL);

    lcd.begin();
    lcd.backlight();
}

void updatePeriodicalReadings()
{
    ectUpdateRaw(analogRead(A0));
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

    lcd.setCursor(8, 0);
    {
        lcd.print(raw);
        lcd.print("R");
    }

    lcd.setCursor(0, 1);
    {
        const auto isValid = ectIsValid();

        switch (ectIsValid())
        {
        case EctRange::TOO_LOW:
            lcd.print("Cold");
            break;
        case EctRange::TOO_HIGH:
            lcd.print("Hot");
            break;
        case EctRange::OK:
            lcd.print(celsiusA);
            lcd.print('C');
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
    }
}

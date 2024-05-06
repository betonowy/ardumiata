#ifndef DEBUG_DISPLAY
#include "display.hpp"

#include "lcdi2c.hpp"

#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

struct ColRow
{
    uint8_t col, row;
};

const ColRow baseOffsets[4] = {{0, 0}, {9, 0}, {0, 1}, {9, 1}};
char buffer[4][6] = {};
uint8_t icons[4][2] = {0xff};

int displaySlotToIndex(DisplaySlot slot)
{
    return static_cast<int>(slot);
}

void clearBuffer(int index)
{
    memset(buffer[index], ' ', 5);
    buffer[index][5] = 0;
}
} // namespace

void setupDisplay()
{
    lcd.begin();
    lcd.backlight();

    lcd.createChar(iconTemp, charmapTemp);
    lcd.createChar(iconOil, charmapOil);
    lcd.createChar(iconPressure, charmapPressure);
    lcd.createChar(iconWater, charmapWater);
    lcd.createChar(iconRpmA, charmapRpmA);
    lcd.createChar(iconRpmB, charmapRpmB);
    lcd.createChar(iconBatA, charmapBatA);
    lcd.createChar(iconBatB, charmapBatB);

    for (int i = 0; i < 4; ++i)
    {
        clearBuffer(i);
    }
}

void updateDisplay()
{
    for (int i = 0; i < 4; ++i)
    {
        const auto baseOffset = baseOffsets[i];

        lcd.setCursor(baseOffset.col, baseOffset.row);

        if (icons[i][0] < 8)
        {
            for (int j = 0; j < 2; ++j)
            {
                lcd.write(icons[i][j]);
            }

            lcd.print(' ');
        }
        else
        {
            for (int j = 0; j < 3; ++j)
            {
                lcd.print(' ');
            }
        }

        lcd.print(buffer[i]);
    }
}

void setSlot(DisplaySlot slot, int value)
{
    const auto index = displaySlotToIndex(slot);
    clearBuffer(index);
    snprintf(buffer[index], 5, "%4d", value);
}

void setSlot(DisplaySlot slot, float value)
{
    const auto index = static_cast<int>(slot);
    clearBuffer(index);

    if (value >= 1000)
    {
        snprintf(buffer[index], 6, "%4d", static_cast<int32_t>(value));
    }
    else
    {
        const auto d = div(static_cast<int32_t>(round(value * 10.f)), 10);

        if (slot == DisplaySlot::UR || slot == DisplaySlot::BR)
        {
            snprintf(buffer[index], 6, "%2d.%1d", d.quot, d.rem);
        }
        else
        {
            snprintf(buffer[index], 6, "%3d.%1d", d.quot, d.rem);
        }
    }
}

void setSlot(DisplaySlot slot, const char *str, int len)
{
    const auto index = static_cast<int>(slot);
    clearBuffer(index);

    if (slot == DisplaySlot::UR || slot == DisplaySlot::BR)
    {
        snprintf(buffer[index], 6, "%4s", str);
    }
    else
    {
        snprintf(buffer[index], 6, "%5s", str);
    }
}

void setSlot(DisplaySlot slot, const char *str)
{
    setSlot(slot, str, strlen(str));
}

void setSlotIcon(DisplaySlot slot, Icon lhs, Icon rhs)
{
    const auto index = displaySlotToIndex(slot);
    icons[index][0] = static_cast<int>(lhs);
    icons[index][1] = static_cast<int>(rhs);
}
#endif

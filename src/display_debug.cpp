#ifdef DEBUG_DISPLAY
#include "display.hpp"

#include <Arduino.h>

namespace
{
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
    Serial.begin(9600);

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

        Serial.print(static_cast<char>('0' + i));
        Serial.print(": ");
        Serial.print(buffer[i]);
        Serial.print(';');
    }

    Serial.print('\r');
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

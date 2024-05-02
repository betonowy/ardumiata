#pragma once

void setupDisplay();
void updateDisplay();

enum class DisplaySlot
{
    UL,
    UR,
    BL,
    BR,
};

enum class Icon
{
    TEMP,
    OIL,
    PRESSURE,
    WATER,
    RPM_A,
    RPM_B,
    BAT_A,
    BAT_B,
    NONE,
};

void setSlot(DisplaySlot, int);
void setSlot(DisplaySlot, float);
void setSlot(DisplaySlot, const char *, int);
void setSlot(DisplaySlot, const char *);
void setSlotIcon(DisplaySlot, Icon, Icon);

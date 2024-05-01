#include "buzzer.hpp"
#include "Arduino.h"

namespace
{
constexpr auto buzzerPin = 2;

constexpr auto buzzerDelayMs = 300;

bool buzzerBeeping = false;
bool buzzerState = false;
unsigned long buzzerNextChange = 0;
} // namespace

void setupBuzzer()
{
    pinMode(buzzerPin, OUTPUT);
}

void setBuzzer(bool state)
{
    if (state == buzzerBeeping)
    {
        return;
    }

    buzzerBeeping = state;
    buzzerState = state;
    buzzerNextChange = millis() + buzzerDelayMs;

    digitalWrite(buzzerPin, state ? HIGH : LOW);
}

void loopBuzzer()
{
    const auto tp = millis();

    if (buzzerBeeping && buzzerNextChange < tp)
    {
        buzzerState = !buzzerState;
        buzzerNextChange = tp + buzzerDelayMs;

        digitalWrite(buzzerPin, buzzerState ? HIGH : LOW);
    }
}

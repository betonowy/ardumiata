#pragma once

#include <stdint.h>

void ectUpdateRaw(int16_t);

float ectGetCelsius();
float ectGetVolt();
int16_t ectGetRaw();

enum class EctRange
{
    OK,
    TOO_LOW,
    TOO_HIGH,
};

EctRange ectIsValid();

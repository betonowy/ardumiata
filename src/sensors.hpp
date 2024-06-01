#pragma once

#include <stdint.h>

enum class SensorRange
{
    OK,
    TOO_LOW,
    TOO_HIGH,
    OK_DISABLE_ALARM,
};

// Engine Coolant Temperature

void ectUpdateRaw(int16_t);
float ectGetCelsius();
float ectGetVolt();
int16_t ectGetRaw();
SensorRange ectIsValid();

// Engine Oil Temperature

void eotUpdateRaw(int16_t);
float eotGetCelsius();
float eotGetVolt();
int16_t eotGetRaw();
SensorRange eotIsValid();

// Engine Oil Pressure

void eopUpdateRaw(int16_t);
float eopGetBar();
float eopGetVolt();
int16_t eopGetRaw();
SensorRange eopIsValid();

// Miscelaneous

int hardenedAnalogRead(uint8_t);

#pragma once

#define RAW_ADC_TO_VOLT (1.0f / 376.f)

float getVoltage();
float foreignVoltageCorrection();

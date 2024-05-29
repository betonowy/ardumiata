#pragma once

#define AREF_VOLTAGE (2.717f)
#define RAW_ADC_TO_VOLT (1.0f / 377.f)

float getVoltage();
float foreignVoltageCorrection();

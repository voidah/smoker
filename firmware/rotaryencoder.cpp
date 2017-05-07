#include "rotaryencoder.h"
#include <Arduino.h>

int RotaryEncoder::m_pinA;
int RotaryEncoder::m_pinB;
volatile unsigned long RotaryEncoder::m_threshold;

volatile long RotaryEncoder::m_rotaryHalfSteps;

volatile unsigned long RotaryEncoder::m_int0time;
volatile unsigned long RotaryEncoder::m_int1time;
volatile uint8_t RotaryEncoder::m_int0signal;
volatile uint8_t RotaryEncoder::m_int1signal;
volatile uint8_t RotaryEncoder::m_int0history;
volatile uint8_t RotaryEncoder::m_int1history;

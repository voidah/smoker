#ifndef ROTARYENCODER_H__
#define ROTARYENCODER_H__

#include <Arduino.h>

// Inspired by http://playground.arduino.cc/Main/RotaryEncoders#Example16
class RotaryEncoder
{
    public:
    void Setup(int pinA, int pinB)
    {
        m_pinA = pinA;
        m_pinB = pinB;

        m_threshold = 10000;

        // 'rotaryHalfSteps' is the counter of half-steps. The actual
        // number of steps will be equal to rotaryHalfSteps / 2
        //
        m_rotaryHalfSteps = 0;

        // Working variables for the interrupt routines
        //
        m_int0time = 0;
        m_int1time = 0;
        m_int0signal = 0;
        m_int1signal = 0;
        m_int0history = 0;
        m_int1history = 0;

        pinMode(m_pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
        pinMode(m_pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)

        attachInterrupt(0, int0, CHANGE);
        attachInterrupt(1, int1, CHANGE);
    }

    long GetValue() const
    {
        return m_rotaryHalfSteps;
    }

    void ResetValue() const
    {
        m_rotaryHalfSteps = 0;
    }

    enum ACTION
    {
        NONE,
        LEFT,
        RIGHT
    };
    ACTION GetLastAction()
    {
        static long lastValue = GetValue();

        ACTION action = NONE;

        long value = GetValue();
        if(value > lastValue)
            action = RIGHT;
        else if(value < lastValue)
            action = LEFT;

        lastValue = value;

        return action;
    }

    private:
    static int m_pinA;
    static int m_pinB;

    static volatile unsigned long m_threshold;

    // 'rotaryHalfSteps' is the counter of half-steps. The actual
    // number of steps will be equal to rotaryHalfSteps / 2
    //
    static volatile long m_rotaryHalfSteps;

    // Working variables for the interrupt routines
    //
    static volatile unsigned long m_int0time;
    static volatile unsigned long m_int1time;
    static volatile uint8_t m_int0signal;
    static volatile uint8_t m_int1signal;
    static volatile uint8_t m_int0history;
    static volatile uint8_t m_int1history;

    private:
    static void int0()
    {
        if(micros() - m_int0time < m_threshold)
            return;
        m_int0history = m_int0signal;
        m_int0signal = digitalRead(m_pinA);
        if(m_int0history == m_int0signal)
            return;
        m_int0time = micros();
        if(m_int0signal == m_int1signal)
            m_rotaryHalfSteps++;
        else
            m_rotaryHalfSteps--;
    }

    static void int1()
    {
        if(micros() - m_int1time < m_threshold)
            return;
        m_int1history = m_int1signal;
        m_int1signal = digitalRead(m_pinB);
        if(m_int1history == m_int1signal)
            return;
        m_int1time = micros();
    }
};

#endif // ROTARYENCODER_H__

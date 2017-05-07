#ifndef MENU_H__
#define MENU_H__

#include "globals.h"
#include <LiquidCrystal_I2C.h>

void SetPIDValues(double kp, double ki, double kd);

class Menu
{
    public:
    Menu() : next(0) {}

    virtual void UpdateDisplay(LiquidCrystal_I2C &lcd) = 0;

    virtual void OnRight() {}
    virtual void OnLeft() {}

    Menu *next;
};

class MainMenu : public Menu
{
    public:
    virtual void UpdateDisplay(LiquidCrystal_I2C &lcd)
    {
        char line1[16 + 1] = {' '};
        snprintf(line1, 16, "T:%4d C:%4d %c", (int)tempF, (int)setPoint, (manualMode ? 'M' : 'A'));
        //line1[strlen(line1) - 1] = ' ';
        line1[16] = 0;

        char line2[16 + 1] = {' '};
        snprintf(line2, 16, "F:%3d%% A:%3d%% ", (int)fanSpeed, (int)airInput);
        line2[strlen(line2) - 1] = ' ';
        line2[16] = 0;

        //lcd.clear();
        lcd.home();
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }

    virtual void OnRight()
    {
        void SetAirInput(int percent, bool force = false);

        if(manualMode)
            SetAirInput(airInput + 3);
        else
            setPoint += 5;
    }

    virtual void OnLeft()
    {
        void SetAirInput(int percent, bool force = false);

        if(manualMode)
            SetAirInput(airInput - 3);
        else
            setPoint -= 5;
    }
};

template <class T>
class ChangeValueMenu : public Menu
{
    public:
    ChangeValueMenu(const char *title, T *value, T step, void (*callback)() = 0) : m_title(title), m_value(value), m_step(step), m_callback(callback) {}

    virtual void UpdateDisplay(LiquidCrystal_I2C &lcd)
    {
        char line1[16 + 1] = {' '};
        snprintf(line1, 16, m_title);
        line1[16] = 0;

        int multiplier = 100;
        if(m_step < 0.1)
            multiplier = 10;

        double dec = *m_value - (int)*m_value;
        dec *= multiplier;

        int decd = (int)dec;
        decd %= multiplier;

        int moreLen = 0;
        if(m_step < 0.1)
            moreLen++;

        char line2[16 + 1] = {0};
        dtostrf(*m_value, -4 - moreLen, 1 + moreLen, line2);

        lcd.clear();
        lcd.home();
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }

    virtual void OnRight()
    {
        *m_value += m_step;

        if(m_callback)
            m_callback();
    }

    virtual void OnLeft()
    {
        *m_value -= m_step;

        if(m_callback)
            m_callback();
    }

    private:
    const char *m_title;
    T *m_value;
    T m_step;
    void (*m_callback)();
};

class TestMenu : public Menu
{
    public:
    virtual void UpdateDisplay(LiquidCrystal_I2C &lcd)
    {
        lcd.clear();
        lcd.home();
        lcd.print("TEST");
    }
};

#endif // MENU_H__

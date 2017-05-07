#include <Wire.h>

// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
// http://arduino-info.wikispaces.com/LCD-Blue-I2C#v1
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// https://github.com/br3ttb/Arduino-PID-Library/
#include <PID_v1.h>

#include <SPI.h>
#include "printf.h"
#include "rotaryencoder.h"
#include "menu.h"
#include "globals.h"

//#define USE_PT100_THERMOCOUPLE
#define USE_TYPEK_THERMOCOUPLE

#ifdef USE_PT100_THERMOCOUPLE
#include "pt100.h"
#endif

#ifdef USE_TYPEK_THERMOCOUPLE
// https://github.com/adafruit/MAX6675-library
#include "max6675.h"
#endif

int TIP122pin = 11;
int ROTARY_BUTTON_PIN = 7;
int MODE_BUTTON_PIN = 8;
int BACK_BUTTON_PIN = 9;
int SERVO_PIN = 14;

double pidOutput = 0;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

Servo servo;

RotaryEncoder rotaryEncoder;

PID pid(&tempF, &pidOutput, &setPoint, kp, ki, kd, DIRECT);

#ifdef USE_TYPEK_THERMOCOUPLE
MAX6675 thermocouple(6, 5, 4);
#endif

long lastServoUpdate = 0;

void SetFanSpeed(int percent)
{
    percent = constrain(percent, 0, maximumFanSpeed);
    fanSpeed = percent;

    if(percent < minimumFanSpeed)
        percent = 0;

    analogWrite(TIP122pin, map(percent, 0, 100, 0, 255));
}

void SetAirInput(int percent, bool force = false)
{
    percent = constrain(percent, 0, 100);
    airInput = percent;

    static int lastValue = -1;

    if(force || lastValue != percent)
    {
        servo.attach(SERVO_PIN); //analog pin 0
        servo.write(map(percent, 0, 100, SERVO_FULL_CLOSE, SERVO_FULL_OPEN));
        lastServoUpdate = millis();
        //delay(200);
        //servo.detach();
        lastValue = percent;
    }

    // If the air vent is open more than some value, activate the fan
    if(percent >= minimumFanStart)
    {
        float fan = map(100 - percent, 0, 100 - minimumFanStart, maximumFanSpeed + 1, minimumFanSpeed); // min 20% for motor
        SetFanSpeed(fan);
    }
    else
    {
        SetFanSpeed(0);
    }
}

void UpdatePID()
{
    SetPIDValues(kp, ki, kd);
}

void UpdateAirAndFan()
{
    SetAirInput(airInput, true);
}

MainMenu mainMenu;
ChangeValueMenu<double> pMenu("Change P:", &kp, 0.1, UpdatePID);
ChangeValueMenu<double> iMenu("Change I:", &ki, 0.1, UpdatePID);
ChangeValueMenu<double> dMenu("Change D:", &kd, 0.01, UpdatePID);
ChangeValueMenu<int> fanStartValueMenu("Fan start value:", &minimumFanStart, 1, UpdateAirAndFan);
ChangeValueMenu<int> minFanSpeedMenu("Fan min speed:", &minimumFanSpeed, 1, UpdateAirAndFan);
ChangeValueMenu<int> maxFanSpeedMenu("Fan max speed:", &maximumFanSpeed, 1, UpdateAirAndFan);
//PMenu pMenu;
TestMenu testMenu;

Menu *currentMenu = &mainMenu;

void DetectI2cLCDAddress()
{
    // http://playground.arduino.cc/Main/I2cScanner
    for(byte address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if(error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if(address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
}

// http://playground.arduino.cc/Code/PwmFrequency
void setPwmFrequency(int pin, int divisor)
{
    byte mode;
    if(pin == 5 || pin == 6 || pin == 9 || pin == 10)
    {
        switch(divisor)
        {
            case 1:
                mode = 0x01;
                break;
            case 8:
                mode = 0x02;
                break;
            case 64:
                mode = 0x03;
                break;
            case 256:
                mode = 0x04;
                break;
            case 1024:
                mode = 0x05;
                break;
            default:
                return;
        }
        if(pin == 5 || pin == 6)
        {
            TCCR0B = TCCR0B & 0b11111000 | mode;
        }
        else
        {
            TCCR1B = TCCR1B & 0b11111000 | mode;
        }
    }
    else if(pin == 3 || pin == 11)
    {
        switch(divisor)
        {
            case 1:
                mode = 0x01;
                break;
            case 8:
                mode = 0x02;
                break;
            case 32:
                mode = 0x03;
                break;
            case 64:
                mode = 0x04;
                break;
            case 128:
                mode = 0x05;
                break;
            case 256:
                mode = 0x06;
                break;
            case 1024:
                mode = 0x7;
                break;
            default:
                return;
        }
        TCCR2B = TCCR2B & 0b11111000 | mode;
    }
}

void SetPIDValues(double newKp, double newKi, double newKd)
{
    kp = newKp;
    ki = newKi;
    kd = newKd;
    pid.SetTunings(kp, ki, kd);
}

void ReadTemperature()
{
#ifdef USE_PT100_THERMOCOUPLE
    // Code to read PT100
    tempF = analogRead(A2);

    float R1 = 1000; // ohm
    float Vout = tempF * (1.1 / 1023.0);
    float R2 = R1 * 1 / (5.0 / Vout - 1);

    // Adjust R2 to account my manual calibration in iced water:
    R2 += 7;

    tempF = ResistanceToFDegree(R2);
#endif

#ifdef USE_TYPEK_THERMOCOUPLE
    // Code to read type-k using max6675
    tempF = thermocouple.readFahrenheit();
    tempF -= 10; // calibration in iced and boiling water

    // Arrondir:
    tempF = (int)(tempF + .5);
#endif
}

void UpdateDisplay()
{
    currentMenu->UpdateDisplay(lcd);
}

void setup()
{
    //Serial.begin(115200);
    //Serial.println(F("!!!"));
    //DetectI2cLCDAddress();

    // rotary encoder
    rotaryEncoder.Setup(2, 3);

    // rotary encoder push button
    pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);

    // Mode and back button
    pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BACK_BUTTON_PIN, INPUT_PULLUP);

#ifdef USE_PT100_THERMOCOUPLE
    analogReference(INTERNAL); // 1.1v reference voltage
#endif

    // Motor:
    setPwmFrequency(TIP122pin, 256);
    pinMode(TIP122pin, OUTPUT); // Set pin for output to control TIP120 Base pin
    SetFanSpeed(0);

    SetAirInput(DEFAULT_AIR_INPUT_PERCENT);

    lcd.begin(16, 2);
    lcd.clear();
    lcd.backlight();

    lcd.home();
    lcd.print("Smoker v1.0");
    lcd.setCursor(0, 1);
    lcd.print("Arthur Ouellet");
    delay(1000);
    lcd.clear();

    // Init pid controller
    ReadTemperature();
    setPoint = 250.f;
    pid.SetOutputLimits(0, 100);
    pid.SetMode(AUTOMATIC);
    UpdateDisplay();

    mainMenu.next = &pMenu;

    pMenu.next = &iMenu;
    iMenu.next = &dMenu;

    dMenu.next = &fanStartValueMenu;
    fanStartValueMenu.next = &minFanSpeedMenu;
    minFanSpeedMenu.next = &maxFanSpeedMenu;
}

#define LOOP_TIME 5000 // ms
void loop()
{
    bool okButton = (digitalRead(ROTARY_BUTTON_PIN) == 0);
    bool modeButton = (digitalRead(MODE_BUTTON_PIN) == 0);
    bool backButton = (digitalRead(BACK_BUTTON_PIN) == 0);

    RotaryEncoder::ACTION action = rotaryEncoder.GetLastAction();

    bool turnRight = (action == RotaryEncoder::RIGHT);
    bool turnLeft = (action == RotaryEncoder::LEFT);

    if(okButton)
    {
        currentMenu = currentMenu->next;
        if(!currentMenu)
            currentMenu = &mainMenu;

        // wait until okButton is released
        while(okButton)
            okButton = (digitalRead(ROTARY_BUTTON_PIN) == 0);
    }

    if(turnRight)
        currentMenu->OnRight();
    if(turnLeft)
        currentMenu->OnLeft();

    manualMode = modeButton;

    static long lastT = 0;
    if(lastT == 0 || (millis() - lastT) >= LOOP_TIME)
    {
        // Compute pid
        ReadTemperature();

        lastT = millis();
    }

    if(!manualMode)
    {
        pid.Compute();
        SetAirInput(pidOutput);
    }

    if(backButton)
        currentMenu = &mainMenu;

    UpdateDisplay();

    if(millis() - lastServoUpdate > 1000)
        servo.detach();

    delay(50);
}

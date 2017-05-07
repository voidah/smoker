#ifndef GLOBALS_H__
#define GLOBALS_H__

#include <LiquidCrystal_I2C.h>

#define SERVO_FULL_OPEN 1650
#define SERVO_FULL_CLOSE 750

#define DEFAULT_AIR_INPUT_PERCENT 50
#define DEFAULT_FAN_SPEED_PERCENT 0

extern double tempF;
extern double setPoint;
extern double airInput;
extern double fanSpeed;
extern bool manualMode;

extern int minimumFanStart;
extern int minimumFanSpeed;
extern int maximumFanSpeed;

// PID
extern double kp;
extern double ki;
extern double kd;

#endif // GLOBALS_H__

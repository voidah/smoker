#include "globals.h"

double tempF = 0;
double setPoint = 0;
double airInput = DEFAULT_AIR_INPUT_PERCENT;
double fanSpeed = DEFAULT_FAN_SPEED_PERCENT;
bool manualMode = false;

int minimumFanStart = 85;
int minimumFanSpeed = 30;
int maximumFanSpeed = 100;

//double kp = 2;
//double ki = 0.1;
//double kd = 0;
double kp = 4.0;
double ki = 0.2;
double kd = 0.0;

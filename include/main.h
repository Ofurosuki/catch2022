#pragma once

#include <Motor.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <Stepper.h>

extern Motor motor;
extern Servo servo;
extern Solenoid solenoid;
extern Sensor sensor;
extern Stepper thetaStepper;
extern Stepper rStepper;
extern Stepper upDownStepper;

enum AreaColor { Red, Blue };
extern AreaColor areaColor;
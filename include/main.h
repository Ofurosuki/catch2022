#pragma once
#include "Gamepad.h"
#include "Motor.h"
#include "Sensor.h"
#include "Servo.h"
#include "Solenoid.h"
#include "stepper_rotate.h"

extern rotate_stepper stepper_z;
extern rotate_stepper stepper_theta;
extern rotate_stepper stepper_r;
extern Motor motor;
extern Servo servo;
extern Solenoid solenoid;
extern Gamepad gamepad;
extern Gui gui;
extern Sensor sensor;
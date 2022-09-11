#pragma once
#include "Motor.h"
#include "Servo.h"
#include "Solenoid.h"
#include "stepper_rotate.h"

#include "Gamepad.h"

extern rotate_stepper stepper_z;
extern rotate_stepper stepper_theta;
extern rotate_stepper stepper_r;
extern Motor motor;
extern Servo servo;
extern Solenoid solenoid;
extern Gamepad gamepad;
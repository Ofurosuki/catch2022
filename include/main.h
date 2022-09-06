#pragma once
#include <stepper_rotate.h>
#include <Motor.h>
#include <Solenoid.h>
#include <Servo.h>

extern rotate_stepper stepper_z;
extern rotate_stepper stepper_theta;
extern rotate_stepper stepper_r;
extern Motor motor;
extern Servo servo;
extern Solenoid solenoid;
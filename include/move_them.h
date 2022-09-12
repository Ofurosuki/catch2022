#pragma once
#include <field_data.h>
#include <main.h>
#include <mbed.h>
void move(position pos, float phi = 45.0f);
float cal_theta(position pos);
void catch_jaga();
void release_jaga();
void take_down(float z);
void take_up();
void gamepad_input_to_command();
void getDegree();

// extern const int delta_time_to_resuck;
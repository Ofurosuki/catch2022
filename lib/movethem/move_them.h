#pragma once
#include <field_data.h>
#include <mbed.h>
void move(position pos, bool is_common = 0);
float cal_theta(position pos);
void catch_jaga(float z);
void release_jaga(bool sucker0 = 1, bool sucker1 = 1, bool sucker2 = 1);
void take_down(float z);
void take_up();
void gamepad_input_to_command();
float phi(int phi);
void getDegree();

extern const int delta_time_to_resuck;
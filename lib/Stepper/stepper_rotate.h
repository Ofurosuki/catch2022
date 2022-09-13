#ifndef __STEPPER_ROTATE_H__
#define __STEPPER_ROTATE_H__
#include "mbed.h"
#include "stepper.h"
class rotate_stepper : public Stepper {
  float step_per_theta = 10;
  float theta_zero = 0;
  int step_mode = 1;
  bool is_vel_moving = false;
  int freq_diff_tmp = 0;
  float freq_ini_tmp = 0;

 public:
  void rotate(float theta);
  float progress_cnt();
  void set_theta_config(float theta_0, float theta_per_stp);
  void set_step_mode(int num);
  void rotate_vel(int vel);
  void emergency();
  void reset(int global_step);
  rotate_stepper(PinName dir, PinName stp);
};
#endif
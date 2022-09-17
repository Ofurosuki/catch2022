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
  // for rotate_vel_loop
  float last_vel = 0.0f;
  const float max_vel_diff = 1.0f;
  //
 public:
  int is_plus = 0;
  void rotate(float theta);
  float progress_cnt();
  void set_theta_config(float theta_0, float stp_per_theta);
  void set_step_mode(int num);
  void rotate_vel(int velocity);
  void rotate_vel_loop(int vel);
  // void rotate_acc(int vel);
  void emergency();
  void reset(int global_step);
  rotate_stepper(PinName dir, PinName stp);
};
#endif
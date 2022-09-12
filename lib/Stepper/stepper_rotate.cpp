#include "stepper_rotate.h"

#include <mbed.h>

#include "config.h"
#include "stepper.h"
rotate_stepper::rotate_stepper(PinName dir, PinName stp) : Stepper(dir, stp) {}
void rotate_stepper::rotate(float theta) {
  int abs_step = (int)(step_per_theta * (theta - theta_zero) / step_mode);
  step(freq_ini, abs_step - global_stp_counter);
}
float rotate_stepper::progress_cnt() {
  if (target_step == 0) {
    return 0;
  } else {
    if (stp_counter == 0) {
      return 1.0f;
    } else {
      return ((float)(stp_counter) / (float)(abs(target_step)));
    }
  }
}

void rotate_stepper::set_theta_config(float theta_0, float stp_per_theta) {
  step_per_theta = stp_per_theta;
  theta_zero = theta_0;
}
void rotate_stepper::set_step_mode(int num) { step_mode = num; }

void rotate_stepper::rotate_vel(int vel) {
  static int freq_diff_tmp;
  static float freq_ini_tmp;
  if (!is_vel_moving) {
    freq_ini_tmp = freq_ini;
    freq_diff_tmp = freq_diff;
  }
  set_config(0, freq_max, vel);
  if (vel > 0) {
    is_vel_moving = true;
    step(vel, INT_MAX);
  } else {
    is_vel_moving = true;
    step(-vel, -INT_MAX);
  }
  if (vel == 0) {
    is_vel_moving = false;
    stepcycle = 0;
    set_config(freq_diff_tmp, freq_max, freq_ini_tmp);
  }
}

void rotate_stepper::emergency() {
  stepcycle = 0;  // called when limit switch is on
}

void rotate_stepper::reset(int global_step) {
  stepcycle = 0;
  global_stp_counter = global_step;  // for theta
}

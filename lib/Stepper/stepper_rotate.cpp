#include "stepper_rotate.h"

#include <mbed.h>

#include "stepper.h"
rotate_stepper::rotate_stepper(PinName dir, PinName stp) : Stepper(dir, stp) {}
void rotate_stepper::rotate(int freq_ini, float theta) {
  int abs_step = (int)(step_per_theta * (theta - theta_zero) / step_mode);
  step(freq_ini, abs_step - global_stp_counter);
}
float rotate_stepper::progress_cnt() {
  if (target_step == 0) {
    return 0;
  } else {
    return (float)(stp_counter / abs(target_step));
  }
}
void rotate_stepper::set_theta_config(float theta_0, float stp_per_theta) {
  step_per_theta = stp_per_theta;
  theta_zero = theta_0;
}
void rotate_stepper::set_step_mode(int num) { step_mode = num; }

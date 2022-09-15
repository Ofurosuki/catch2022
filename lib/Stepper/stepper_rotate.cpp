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
/*
void rotate_stepper::rotate_vel(int vel) {
  if (!is_vel_moving) {
    freq_ini_tmp = freq_ini;
    freq_diff_tmp = freq_diff;
  }
  if (vel == 0) {
    is_vel_moving = false;
    stepcycle = 0;
    set_config(freq_diff_tmp, freq_max, freq_ini_tmp);
    return;
  }
  set_config(freq_diff, freq_max, freq_ini);
  if (vel > 0) {
    is_vel_moving = true;
    step(vel, INT_MAX);
  } else if (vel < 0) {
    is_vel_moving = true;
    step(-vel, -INT_MAX);
  }
}*/

void rotate_stepper::rotate_acc(int acc) {
  const float freq_diff_max = 200.0f;
  static int freq_diff_tmp;
  static float freq_ini_tmp;
  const float dead_zone = 20.0f;
  const float stop_acc = 180.0f;  // must be positive
  if (!is_vel_moving) {
    freq_ini_tmp = freq_ini;
    freq_diff_tmp = freq_diff;
  }
  if (freq_diff_max <= abs(acc)) {
    if (acc >= 0) {
      acc = freq_diff_max;
    } else {
      acc = -freq_diff_max;
    }  // acc　max の設定
  }
  freq_diff = acc;
  if (acc == 0) {
    if (freq < 0) {
      acc = -stop_acc;
    } else {
      acc = stop_acc;
    }
  }
  step(freq, INT_MAX);
  is_vel_moving = true;
  if (freq >= dead_zone || freq <= -dead_zone) {
    is_vel_moving = false;
    stepcycle = 0;
    freq_diff = freq_diff_tmp;
    freq_ini = freq_ini_tmp;
  }
}
/*
void rotate_stepper::rotate_vel(int acc) {
  static int freq_diff_tmp;
  static float freq_ini_tmp;
  const float freq_diff_max = 200.0f;
  // must be positive :max value of acceleration
  const float dead_zone = 20.0f;
  static float last_vel_freq = 0;
  if (!is_vel_moving) {
    freq_ini_tmp = freq_ini;
    freq_diff_tmp = freq_diff;
  }
  if (freq_diff_max <= abs(acc)) {
    if (acc >= 0) {
      acc = freq_diff_max;
    } else {
      acc = -freq_diff_max;
    }  // acc　max の設定
  }

  if (freq > dead_zone) {
    if (acc == 0) {
      freq_diff = -freq_diff_max;
      is_vel_moving = true;
      step((int)(last_vel_freq), INT_MAX);
      last_vel_freq = freq;
    } else {
      freq_diff = acc;
      is_vel_moving = true;
      step((int)(last_vel_freq), INT_MAX);
      last_vel_freq = freq;
    }

  } else if (freq < -dead_zone) {
    if (acc == 0) {
      acc = freq_diff_max;
    }

    step((int)(last_vel_freq), -INT_MAX);
    last_vel_freq = -freq;
  } else if (freq > 0) {
    if (acc == 0) {
      is_vel_moving = false;
      stepcycle = 0;
      freq_diff = freq_diff_tmp;
      freq_ini = freq_ini_tmp;
    } else {
      freq_diff = acc;
      step((int)(last_vel_freq), INT_MAX);
      is_vel_moving = true;
      last_vel_freq = freq;
    }
  } else {
    if (acc == 0) {
      is_vel_moving = false;
      stepcycle = 0;
      freq_diff = freq_diff_tmp;
      freq_ini = freq_ini_tmp;
    } else {
      freq_diff = acc;
      freq = -freq;
      step((int)(last_vel_freq), -INT_MAX);
      is_vel_moving = true;
      last_vel_freq = -freq;
    }
  }
}

void rotate_stepper::emergency() {
  stepcycle = 0;  // called when limit switch is on
}

void rotate_stepper::reset(int global_step) {
  stepcycle = 0;
  global_stp_counter = global_step;  // for theta
}

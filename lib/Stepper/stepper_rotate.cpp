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
  set_config(0, freq_max, vel);
  if (vel > 0) {
    is_vel_moving = true;
    step(vel, INT_MAX);
  } else if (vel < 0) {
    is_vel_moving = true;
    step(-vel, -INT_MAX);
  }
}*/

void rotate_stepper::rotate_vel(int velocity) {
  float vel = (float)(velocity);
  const float vel_coefficient = 1.0f;
  //インプットの何倍の周波数を取るか(freq=velocity_coefficient*velocity)
  vel = vel * vel_coefficient;
  if (!is_vel_moving) {
    freq_ini_tmp = freq_ini;
    freq_diff_tmp = freq_diff;
  }
  if (vel == 0 && freq == 0) {
    is_vel_moving = false;
    set_config(freq_diff_tmp, freq_max, freq_ini_tmp);
    stepcycle = 0;
    last_vel = 0;
    return;
  }

  if (vel - last_vel >= 0) {
    if (vel - last_vel > max_vel_diff) {
      vel = last_vel + max_vel_diff;
    }
    if (last_vel >= 0) {
      is_vel_moving = true;
      step(vel, INT_MAX);
      is_plus = 1;
    } else if (vel <= 0) {
      is_vel_moving = true;
      step(-vel, -INT_MAX);
      is_plus = 0;
    } else {
      is_vel_moving = true;
      step(vel, INT_MAX);
      is_plus = 1;
    }
  } else if (vel - last_vel < 0) {
    if (last_vel - vel > max_vel_diff) {
      vel = last_vel - max_vel_diff;
    }
    if (vel >= 0) {
      is_vel_moving = true;
      step(vel, INT_MAX);
      is_plus = 1;
    } else if (vel < 0) {
      is_vel_moving = true;
      step(-vel, -INT_MAX);
      is_plus = 0;
    }
  }
  last_vel = vel;

  /*
    if (vel > 0) {
      if (abs(last_vel - vel) > max_vel_diff) {
        if (vel - last_vel > 0) {
          vel = last_vel + max_vel_diff;
        } else if (vel - last_vel < 0) {
          vel = last_vel - max_vel_diff;
        }
      }
      set_config(0, freq_max, vel);
      is_vel_moving = true;
      step((int)(vel), INT_MAX);
      is_plus = 1;
    } else if (vel < 0) {
      if (abs(last_vel - vel) > max_vel_diff) {
        if (vel - last_vel > 0) {
          vel = last_vel + max_vel_diff;
        } else if (vel - last_vel < 0) {
          vel = last_vel - max_vel_diff;
        }
      }
      set_config(0, freq_max, vel);
      is_vel_moving = true;
      step(-(int)(vel), -INT_MAX);
      is_plus = 0;
    } else if (vel == 0) {
      if(last_vel>=0){
         if (abs(last_vel - vel) > max_vel_diff) {
        if (vel - last_vel > 0) {
          vel = last_vel + max_vel_diff;
        } else if (vel - last_vel < 0) {
          vel = last_vel - max_vel_diff;
        }
      }
      set_config(0, freq_max, vel);
      is_vel_moving = true;
      step((int)(vel), INT_MAX);
      is_plus=1;
      }else{
         if (abs(last_vel - vel) > max_vel_diff) {
        if (vel - last_vel > 0) {
          vel = last_vel + max_vel_diff;
        } else if (vel - last_vel < 0) {
          vel = last_vel - max_vel_diff;
        }
      }
      set_config(0, freq_max, vel);
      is_vel_moving = true;
      step(-(int)(vel), -INT_MAX);
      is_plus=0;
      }
    }*/
}
void rotate_stepper::emergency() {
  stepcycle = 0;  // called when limit switch is on
}

void rotate_stepper::reset(int global_step) {
  stepcycle = 0;
  global_stp_counter = global_step;  // for theta
}

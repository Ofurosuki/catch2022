#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "mbed.h"

class Stepper {
  int cnt;
  volatile int stepcycle;

  bool can_velocity_triangle;
  int target_step;
  int stp_counter;  // local なステップ合計数 一動作終了につきリセット
  int global_stp_counter;  // global　なステップ合計数　試合開始からずっと保持　物理的位置と1対1対応

  float freq;
  int freq_min;
  int freq_max;
  int freq_diff;
  float freq_top;
  bool is_first;

  int triangle_area;

  DigitalOut _dir;
  DigitalOut _stp;

 public:
  Ticker ticker;
  bool is_cw();
  void step_ctl();
  int get_stp_counter();
  int get_cnt();
  int get_stepcycle();
  int get_freq();
  void step(int freq_ini, int target_step);
  void set_config(int d_step_cycle, int minimum_stepcycle);
  Stepper(PinName dir, PinName stp);
};

#endif
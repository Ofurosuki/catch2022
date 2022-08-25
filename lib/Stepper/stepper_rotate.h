#ifndef __STEPPER_ROTATE_H__
#define __STEPPER_ROTATE_H__
#include "mbed.h"
#include "stepper.h"
class rotate_stepper: public Stepper{
    float step_per_theta;
    float theta_zero;
    int step_mode=1;
    public:
    void rotate(int freq_ini,float theta);
    float progress_cnt();
    void set_theta_config(float theta_0,float theta_per_stp);
    void set_step_mode(int num);
    rotate_stepper(PinName dir,PinName stp);
};
#endif
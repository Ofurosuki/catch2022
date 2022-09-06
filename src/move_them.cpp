#include <math.h>
#include <mbed.h>

#include "field_data.h"
#include "main.h"

void move(position pos, bool is_common = 0) {
  float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  stepper_r.rotate(cal_theta(pos));
  motor.drivePosition(pos.x_1);  //何回転が何ミリか確認し、修正
  if (is_common) {
    servo.setPosition(90.0 - cal_theta(pos));
  } else {
    servo.setPosition(jaga_3_degree - cal_theta(pos));
  }
}

float cal_theta(position pos) {
  if (pos.x - pos.x_1 == 0) {
    if (pos.y >= 0) {
      return 3.14 / 2;  // M_PI がよまれない
    } else {
      return -3.14 / 2;
    }
  } else {
    return atan(pos.y / pos.x - pos.x_1);
  }
}
void catch_jaga(float z) { stepper_z.rotate(z); }
void release_jaga(bool sucker0 = 1, bool sucker1 = 1, bool sucker2 = 1) {
  if (sucker0) solenoid.driveSingle(0, 1, 1000);  // 1を吸引解除ということに
  if (sucker1) solenoid.driveSingle(1, 1, 1000);
  if (sucker2) solenoid.driveSingle(2, 1, 1000);
}

void take_down(float z) { stepper_z.rotate(z); }
void take_up(float z) { stepper_z.rotate(z); }
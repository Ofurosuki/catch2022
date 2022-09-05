#include <math.h>
#include <mbed.h>

#include "field_data.h"
#include "main.h"

void move(position pos) {
  float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  stepper_r.rotate(cal_theta(pos));
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

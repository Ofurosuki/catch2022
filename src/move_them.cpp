#include <math.h>
#include <mbed.h>

#include "field_data.h"

position jaga_a = {100, 200};
position shoot_a = {100, 223};

void move(position pos) {
  float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  stepper0.rotate(cal_theta(pos));
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

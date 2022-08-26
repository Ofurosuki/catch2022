#include <math.h>
#include <mbed.h>
typedef struct {
  float x;
  float y;
  float x_1;
} position;

position jaga_a = {1.1, 2.6};
//
//
//

void move(position pos) {
  float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  stepper0.rotate(cal_theta(pos));
}

float cal_theta(position pos) {
  if (pos.x - pos.x_1 == 0) {
    if (pos.y >= 0) {
      return M_PI / 2;
    } else {
      return -M_PI / 2;
    }
  } else {
    atan(pos.y / pos.x - pos.x_1);
  }
}

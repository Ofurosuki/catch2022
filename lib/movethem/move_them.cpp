#define M_PI 3.1415926
#include <math.h>
#include <mbed.h>

#include "field_data.h"
#include "main.h"

void move_x1(float x1) {
  const float revolution_per_x1 = 1.0f / 80.0f;
  if (x1 < x1_max && x1 > 0.0f) {
    motor.drivePosition(x1 * revolution_per_x1);
  } else if (x1 >= x1_max) {
    motor.drivePosition(x1_max);
  } else if (x1 <= 0) {
    motor.drivePosition(0);
  }
}
float cal_theta(position pos) {
  if (pos.x - pos.x_1 == 0) {
    if (pos.y >= 0) {
      return M_PI / 2;  // M_PI がよまれない
    } else {
      return -M_PI / 2;
    }
  } else {
    return atan(pos.y / (pos.x - pos.x_1));
  }
}

void move(position pos, bool is_common = 0) {
  float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  stepper_theta.rotate(cal_theta(pos));
  stepper_r.rotate(r);
  move_x1(pos.x_1);
  if (is_common) {
    servo.setPosition(90.0f - cal_theta(pos));
  } else {
    servo.setPosition(jaga_3_degree - cal_theta(pos));
  }
}

void catch_jaga(float z) { stepper_z.rotate(z); }
const int delta_time_to_resuck = 1000;
void release_jaga(bool sucker0 = 1, bool sucker1 = 1, bool sucker2 = 1) {
  if (sucker0)
    solenoid.driveSingle(0, 1,
                         delta_time_to_resuck);  // 1を吸引解除ということに
  if (sucker1) solenoid.driveSingle(1, 1, delta_time_to_resuck);
  if (sucker2) solenoid.driveSingle(2, 1, delta_time_to_resuck);
}

void take_down(float z) { stepper_z.rotate(z); }
void take_up() { stepper_z.rotate(z_height.z_up); }

float joyDeg0;
float joyDeg1;
int step_choice;  // switch文用。どのステッピングモーターを動かすか

void gamepad_input_to_command() {
  while (gamepad.getButton(1) == 0) {
    const float DCVelocity = 10;
    int StepVel1 = (gamepad.getAxis(1)) * 5;
    int StepVel2 = (gamepad.getAxis(2)) * 5;
    int StepVel3 = (gamepad.getAxis(3)) * 5;
    if (abs(gamepad.getAxis(0)) >= 10) {
      // Dead zone
      // stepXを動かす
      motor.driveVelocity(DCVelocity);
      if (abs(joyDeg0) <= M_PI / 12 || abs(joyDeg0) >= (M_PI / 12) * 11) {
        // stepxを動かす
        motor.driveVelocity(DCVelocity);
      }
    } else if (abs(gamepad.getAxis(1)) >= 10) {
      // Dead zone
      // stepシータを動かす
      stepper_theta.rotate_vel(StepVel1);
      if (abs(joyDeg0) >= (M_PI / 12) * 5 && abs(joyDeg0) <= (M_PI / 12) * 7) {
        // stepθを+に動かす
        // gamepad.Axis(1)の値はマイナスなので正負入れ替えたほうがいいかも
        stepper_theta.rotate_vel(StepVel1);
      }
    } else {
      // stepper止める
      stepper_theta.rotate_vel(0);
    }

    //サーボモーターを動かす
    const float ServoVelocity = 5;
    while (gamepad.getButton(4) == 1) {
      //反時計回り
      //サーボの角度＝サーボの角度+5°
      //みたいな感じで単発押しと長押しで角度調整できるようにする。
      servo.setVelocity(-ServoVelocity);
    }

    while (gamepad.getButton(5) == 1) {
      //時計回り
      servo.setVelocity(ServoVelocity);
    }
  }
}

void getDegree(int xAxis0, int yAxis0, int xAxis1, int yAxis1) {
  xAxis0 = gamepad.getAxis(0);
  yAxis0 = gamepad.getAxis(1);
  xAxis1 = gamepad.getAxis(2);
  yAxis1 = gamepad.getAxis(3);
  joyDeg0 = atan2(xAxis0, yAxis0);
  joyDeg1 = atan2(xAxis1, yAxis1);
}

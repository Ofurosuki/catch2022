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
    motor.drivePosition(x1_max * revolution_per_x1);
  } else if (x1 <= 0) {
    motor.drivePosition(0);
  }
}
float cal_theta(position pos) {
  if (pos.x - pos.x_1 == 0) {
    if (pos.y >= 0) {
      return 90.0f;
    } else {
      return -90.0f;
    }
  } else {
    return atan(pos.y / (pos.x - pos.x_1)) * M_PI / 180.0f;
  }
}

void move(position pos, float phi = 45.0f) {
  if (pos != here) {
    float r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
    stepper_theta.rotate(cal_theta(pos));
    stepper_r.rotate(r);
    move_x1(pos.x_1);
  }
  servo.setPosition(phi - cal_theta(pos));
}

void catch_jaga(float z) { stepper_z.rotate(z); }
const int delta_time_to_resuck = 1000;
void release_jaga(bool sucker0 = 1, bool sucker1 = 1, bool sucker2 = 1) {
  if (sucker0) solenoid.driveSingle(0, 1, delta_time_to_resuck);
  if (sucker1) solenoid.driveSingle(1, 1, delta_time_to_resuck);
  if (sucker2) solenoid.driveSingle(2, 1, delta_time_to_resuck);
}  // 1は吸引解除

void take_down(float z) { stepper_z.rotate(z); }
void take_up() { stepper_z.rotate(z_height.z_up); }

float joyDeg0;
float joyDeg1;
int step_choice;  // switch文用。どのステッピングモーターを動かすか

void getDegree() {
  int xAxis0 = gamepad.getAxis(0);
  int yAxis0 = gamepad.getAxis(1);
  int xAxis1 = gamepad.getAxis(2);
  int yAxis1 = gamepad.getAxis(3);
  joyDeg0 = atan2(yAxis0, xAxis0);
  joyDeg1 = atan2(yAxis1, xAxis1);
}

void gamepad_input_to_command() {
  while (gamepad.getButton(1) == 0) {
    getDegree();
    const float DCVelocity = (float)gamepad.getAxis(0) / 100;
    int StepVel1 = (gamepad.getAxis(1)) * 2;
    int StepVel2 = (gamepad.getAxis(2)) * 2;
    int StepVel3 = (gamepad.getAxis(3));

    // stepXを動かす
    if ((abs(joyDeg0) <= M_PI / 12 || abs(joyDeg0) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(0)) >= 10) {
      // stepxを動かす
      motor.driveVoltage(DCVelocity);
      stepper_theta.rotate_vel(0);
    }
    // stepシータを動かす
    else if ((abs(joyDeg0) >= (M_PI / 12) * 5 &&
              abs(joyDeg0) <= (M_PI / 12) * 7) &&
             abs(gamepad.getAxis(1)) >= 10) {
      stepper_theta.rotate_vel(StepVel1);
      motor.driveVoltage(0);
    } else {
      // stepper, DC止める
      stepper_theta.rotate_vel(0);
      motor.driveVoltage(0);
    }

    // stepRを動かす
    if ((abs(joyDeg1) <= M_PI / 12 || abs(joyDeg1) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(2)) >= 10) {
      // stepRを動かす
      stepper_r.rotate_vel(StepVel2);
      stepper_z.rotate_vel(0);
    }
    // step上下を動かす
    else if ((abs(joyDeg1) >= (M_PI / 12) * 5 &&
              abs(joyDeg1) <= (M_PI / 12) * 7) &&
             (abs(gamepad.getAxis(3)) >= 10)) {
      // step上下に動かす
      stepper_z.rotate_vel(StepVel3);
      stepper_r.rotate_vel(0);
    } else {
      // stepper止める
      stepper_r.rotate_vel(0);
      stepper_z.rotate_vel(0);
    }
    //サーボモーターを動かす
    const float ServoVelocity = 50;  // deg/s
    if (gamepad.getButton(4) == 1) {
      //反時計回り
      servo.setVelocity(-ServoVelocity);
    } else if (gamepad.getButton(5) == 1) {
      //時計回り
      servo.setVelocity(ServoVelocity);
    } else {
      servo.setVelocity(0);
    }
  }
}

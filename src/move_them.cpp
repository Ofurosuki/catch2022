#define M_PI 3.1415926
#include <math.h>
#include <mbed.h>

#include "Gui.h"
#include "field_data.h"
#include "main.h"

float move_x1(float x1) {
  const float revolution_per_x1 =
      (5.5495f / 312.0f + 6.403f / 362.0f + 6.2075f / 350.0f) / 3.0f;
  return (x1 * revolution_per_x1);
}
float cal_theta(position pos) {
  if (pos.x - pos.x_1 == 0) {
    if (pos.y >= 0) {
      return 90.0f;
    } else {
      return -90.0f;
    }
  } else {
    if ((pos.x - pos.x_1) <= 0 && pos.y <= 0) {
      return atan2(pos.y, (pos.x - pos.x_1)) / M_PI * 180.0f + 360.0f;
    } else {
      return atan2(pos.y, (pos.x - pos.x_1)) / M_PI * 180.0f;
    }
  }
}

void move(position pos, float phi = 45.0f) {
  float r;
  float x1;
  if (10 <= cal_theta(pos) - phi && cal_theta(pos) - phi <= 190) {
    servo.setPosition(cal_theta(pos) - phi - 10);
  } else {
    printf("servo modified\n");
    servo.setPosition(cal_theta(pos) - phi + 170);
  }
  if (pos.x_1 <= x1_min) pos.x_1 = x1_min;
  r = sqrt((pos.x - pos.x_1) * (pos.x - pos.x_1) + pos.y * pos.y);
  if (r >= r_max) r = r_max;
  stepper_theta.rotate(cal_theta(pos));
  stepper_r.rotate(r);
  x1 = move_x1(pos.x_1);
  ThisThread::sleep_for(1s);
  while (stepper_r.progress_cnt() < 1.0f ||
         stepper_theta.progress_cnt() < 1.0f ||
         motor.getCurrentPosition() - x1 > 0.05f) {
    printf("progress:(r,theta,x1)=(%f,%f,%f)\n", stepper_r.progress_cnt() * 100,
           stepper_theta.progress_cnt() * 100, motor.getPositionProgress());
    motor.drivePosition(x1);
    ThisThread::sleep_for(100ms);
  }
  printf("move to (%f,%f,) completed.\n", pos.x, pos.y);
}
// const int delta_time_to_resuck = 1000;
void catch_jaga() {
  if (!gui.checkNewSucker()) printf("waiting sucker input...\n");
  while (!gui.checkNewSucker()) {
    ThisThread::sleep_for(100ms);
  }
  if (!gui.getCommand().enableSuckers[0]) {
    solenoid.driveSingle(0, 1, 0);
  } else {
    solenoid.driveSingle(0, 0, 0);
  }
  if (!gui.getCommand().enableSuckers[1]) {
    solenoid.driveSingle(1, 1, 0);
  } else {
    solenoid.driveSingle(1, 0, 0);
  }
  if (!gui.getCommand().enableSuckers[2]) {
    solenoid.driveSingle(2, 1, 0);
  } else {
    solenoid.driveSingle(2, 0, 0);
  }
}

void release_jaga() {
  if (!gui.getCommand().enableSuckers[0]) solenoid.driveSingle(0, 1, 0);
  if (!gui.getCommand().enableSuckers[1]) solenoid.driveSingle(1, 1, 0);
  if (!gui.getCommand().enableSuckers[2]) solenoid.driveSingle(2, 1, 0);
}  // 1は吸引解除

void take_down(float z) {
  stepper_z.rotate(z);
  while (stepper_z.progress_cnt() < 1.0f) {
    ThisThread::sleep_for(100ms);
  }
}
void take_up() {
  stepper_z.rotate_vel(15);
  while (!sensor.getState(4)) {
    ThisThread::sleep_for(100ms);
  }
}

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
  printf("Please input gamepad\n");
  while (gamepad.getButton(1) == 0) {
    // printf("%d,%d,%d,%d\n", gamepad.getAxis(0), gamepad.getAxis(1),
    //        gamepad.getAxis(2), gamepad.getAxis(3));
    printf("theta: %d, r: %d, z: %d\n", stepper_theta.get_freq(),
           stepper_r.get_freq(), stepper_z.get_freq());
    getDegree();
    const float DCVelocity = -(float)gamepad.getAxis(0) / 200;
    int StepVel1 = -(gamepad.getAxis(1)) * 2;
    int StepVel2 = -(gamepad.getAxis(2)) / 2;
    int StepVel3 = -(gamepad.getAxis(3)) * 2;

    // stepXを動かす
    if ((abs(joyDeg0) <= M_PI / 12 || abs(joyDeg0) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(0)) >= 10) {
      // stepxを動かす
      motor.driveVoltage(DCVelocity);
      stepper_r.rotate_vel(0);
    }
    // stepシータを動かす
    else if ((abs(joyDeg0) >= (M_PI / 12) * 5 &&
              abs(joyDeg0) <= (M_PI / 12) * 7) &&
             abs(gamepad.getAxis(1)) >= 10) {
      stepper_r.rotate_vel(StepVel1);
      motor.driveVoltage(0);
    } else {
      // stepper, DC止める
      stepper_r.rotate_vel(0);
      motor.driveVoltage(0);
    }

    // stepRを動かす
    if ((abs(joyDeg1) <= M_PI / 12 || abs(joyDeg1) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(2)) >= 10) {
      // stepRを動かす
      stepper_theta.rotate_vel(StepVel2);
      stepper_z.rotate_vel(0);
    }
    // step上下を動かす
    else if ((abs(joyDeg1) >= (M_PI / 12) * 5 &&
              abs(joyDeg1) <= (M_PI / 12) * 7) &&
             (abs(gamepad.getAxis(3)) >= 10)) {
      // step上下に動かす
      stepper_z.rotate_vel(StepVel3);
      stepper_theta.rotate_vel(0);
    } else {
      // stepper止める
      stepper_theta.rotate_vel(0);
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

#include <CanManager.h>
#include <Gamepad.h>
#include <Motor.h>
#include <PC.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <config.h>
#include <math.h>
#include <mbed.h>
#include <stepper.h>
#include <stepper_rotate.h>

asm(".global _printf_float");  // float出力用

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

UnbufferedSerial pc(USBTX, USBRX);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Motor motor(0x01, manager);
Servo servo(0x02, manager);
Solenoid solenoid(0x03, manager);
Sensor sensor(0x04, manager);

PC pcConnector;
Gamepad gamepad;

DigitalOut led(LED1);
DigitalIn button(BUTTON1);

// Keita
//  int step_choice=0;
//  int deci=0;
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

int main() {
  manager.begin();
  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  rotate_stepper stepper0(DIR0, STP0);
  rotate_stepper stepper1(DIR1, STP1);
  rotate_stepper stepper2(DIR2, STP2);

  while (true) {
    // printf("progresscnt:%f\n", stepper0.progress_cnt());
    printf("%d, %d, %d, %d", gamepad.getAxis(0), gamepad.getAxis(1),
           gamepad.getAxis(2), gamepad.getAxis(3));
    printf(":%d, %d", gamepad.getHat(0), gamepad.getHat(1));
    printf(":%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", gamepad.getButton(0),
           gamepad.getButton(1), gamepad.getButton(2), gamepad.getButton(3),
           gamepad.getButton(4), gamepad.getButton(5), gamepad.getButton(6),
           gamepad.getButton(7), gamepad.getButton(8), gamepad.getButton(9),
           gamepad.getButton(10), gamepad.getButton(11), gamepad.getButton(12),
           gamepad.getButton(13), gamepad.getButton(14), gamepad.getButton(15),
           gamepad.getButton(16), gamepad.getButton(17));
    ThisThread::sleep_for(50ms);

    /*
    [0]  [1]
     θ   上下
     ↑    ↑
    ← →x ←  →R
     ↓    ↓
    */
    getDegree();
    //以下でステッパーを動かすためのswitch文の条件を決める
    const float DCVelocity = (float)gamepad.getAxis(0) / 100;
    int StepVel1 = (gamepad.getAxis(1)) * 2;
    int StepVel2 = (gamepad.getAxis(2)) * 2;
    int StepVel3 = (gamepad.getAxis(3));
    // Dead zone
    // stepXを動かす
    if ((abs(joyDeg0) <= M_PI / 12 || abs(joyDeg0) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(0)) >= 10) {
      // stepxを動かす
      printf("dc");
      motor.driveVoltage(DCVelocity);
      stepper0.rotate_vel(0);
    }
    // Dead zone
    // stepシータを動かす
    else if ((abs(joyDeg0) >= (M_PI / 12) * 5 &&
              abs(joyDeg0) <= (M_PI / 12) * 7) &&
             abs(gamepad.getAxis(1)) >= 10) {
      // stepθを+に動かす
      // gamepad.Axis(1)の値はマイナスなので正負入れ替えたほうがいいかも
      stepper0.rotate_vel(StepVel1);
      motor.driveVoltage(0);
      printf("theta\n");
    } else {
      // stepper止める
      stepper0.rotate_vel(0);
      motor.driveVoltage(0);
    }

    // Dead zone
    // stepRを動かす
    if ((abs(joyDeg1) <= M_PI / 12 || abs(joyDeg1) >= (M_PI / 12) * 11) &&
        abs(gamepad.getAxis(2)) >= 10) {
      // stepRを動かす
      stepper1.rotate_vel(StepVel2);
      stepper2.rotate_vel(0);
      printf("R\n");
    }
    // Dead zone
    // step上下を動かす
    else if ((abs(joyDeg1) >= (M_PI / 12) * 5 &&
              abs(joyDeg1) <= (M_PI / 12) * 7) &&
             (abs(gamepad.getAxis(3)) >= 10)) {
      // step上下に動かす
      stepper2.rotate_vel(StepVel3);
      stepper1.rotate_vel(0);
      printf("updown\n");
    } else {
      // stepper止める
      stepper1.rotate_vel(0);
      stepper2.rotate_vel(0);
    }

    /*
      while (true) {
        printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
               motor.getPositionProgress() * 100);
      } */
    //サーボモーターを動かす
    const float ServoVelocity = 50;
    if (gamepad.getButton(4) == 1) {
      //反時計回り
      //サーボの角度＝サーボの角度+5°
      //みたいな感じで単発押しと長押しで角度調整できるようにする。
      printf("servo CCW\n");
      servo.setVelocity(-ServoVelocity);
    } else if (gamepad.getButton(5) == 1) {
      //時計回り
      printf("servo CW\n");
      servo.setVelocity(ServoVelocity);
    } else {
      servo.setVelocity(0);
    }
  }
}
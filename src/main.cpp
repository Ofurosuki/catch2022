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

asm(".global _printf_float");  // float出力用

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

void getDegree(int xAxis0, int yAxis0, int xAxis1, int yAxis1) {
  xAxis0 = gamepad.getAxis(0);
  yAxis0 = gamepad.getAxis(1);
  xAxis1 = gamepad.getAxis(2);
  yAxis1 = gamepad.getAxis(3);
  joyDeg0 = atan2(xAxis0, yAxis0);
  joyDeg1 = atan2(xAxis1, yAxis1);
}

int main() {
  manager.begin();
  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  while (true) {
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
    //以下でステッパーを動かすためのswitch文の条件を決める

    const float DCVelocity = 10;
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
      if (abs(joyDeg0) >= (M_PI / 12) * 5 && abs(joyDeg0) <= (M_PI / 12) * 7) {
        // stepθを+に動かす
        // gamepad.Axis(1)の値はマイナスなので正負入れ替えたほうがいいかも
      }
    } else {
      // stepper止める
    }

    if (abs(gamepad.getAxis(2)) >= 10) {
      // Dead zone
      // stepRを動かす
      if (abs(joyDeg1) <= M_PI / 12 || abs(joyDeg1) >= (M_PI / 12) * 11) {
        // stepRを動かす
      }
    } else if (abs(gamepad.getAxis(3)) >= 10) {
      // Dead zone
      // step上下を動かす
      if (abs(joyDeg1) >= (M_PI / 12) * 5 && abs(joyDeg1) <= (M_PI / 12) * 7) {
        // step上下に動かす
      }
    } else {
      // stepper止める
    }
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
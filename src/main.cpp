#include <CanManager.h>
#include <Gamepad.h>
#include <Motor.h>
#include <PC.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <config.h>
#include <mbed.h>
#include <stepper.h>
#include <stepper_rotate.h>

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

int main() {
  manager.begin();
  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  rotate_stepper stepper0(DIR0, STP0);

  stepper0.set_config(50, 1000, 100);  // set acceleration and  max velocity
  // stepper0.step(20, -1000);  // rotate stepper (initial frequency and target
  //  step )回すだけの関数もある

  stepper0.set_theta_config(0, 0.557);
  //起動時の角度を指定、1ステップあたりの角度を指定
  // while(button){}
  // stepper0.rotate(2000);
  while (button) {
  }
  // stepper0.step(100,-2000);
  stepper0.rotate_vel(600);

  // stepper0.rotate_vel(0);

  while (true) {
    printf("progresscnt:%f\n", stepper0.progress_cnt());
  }

  /*
    while (true) {
      printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
             motor.getPositionProgress() * 100);
    } */
}

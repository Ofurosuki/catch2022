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
  stepper0.set_config(3, 50, 20);  // set acceleration and  max velocity
  stepper0.step(20, -1000);  // rotate stepper (initial frequency and target
                             // step )回すだけの関数もある

  stepper0.set_theta_config(0, 0.5);
  //起動時の角度を指定、1ステップあたりの角度を指定

  stepper0.rotate(50.2);  // rotate stepper by angle

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
  }
}

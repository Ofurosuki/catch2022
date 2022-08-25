#include <CanManager.h>
#include <Motor.h>
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
Motor motor(0x08, manager);

DigitalIn button(BUTTON1);

int main() {
  manager.begin();
  printf("start\n");
  motor.setMode(Motor::Mode::Position);
  motor.drivePosition(4);

  rotate_stepper stepper0(DIR0, STP0);
  stepper0.set_config(3, 50);  // set acceleration and  max velocity  
  stepper0.step(20,-1000);  // rotate stepper (initial frequency and target step )　　回すだけの関数もある

  stepper0.set_theta_config(0,0.5); //起動時の角度を指定、1ステップあたりの角度を指定

  stepper0.rotate(20,50.0);   //rotate stepper by angle (initial frequency and angle)

  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  }
}
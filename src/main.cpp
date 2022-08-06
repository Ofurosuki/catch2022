#include "main.h"

#include <CanManager.h>
#include <Motor.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <Stepper.h>
#include <mbed.h>

asm(".global _printf_float");  // float出力用

UnbufferedSerial pc(USBTX, USBRX);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Motor motor(0x01, manager);
Servo servo(0x02, manager);
Solenoid solenoid(0x03, manager);
Sensor sensor(0x04, manager);
Stepper thetaStepper(PB_12, PB_13, 200, 10, 10);
Stepper rStepper(PB_14, PB_15, 200, 10, 10);
Stepper upDownStepper(PC_6, PC_7, 200, 10, 10);

AreaColor areaColor = AreaColor::Red;

int main() {
  manager.begin();
  printf("start\n");
  motor.setMode(Motor::Mode::Position);
  motor.drivePosition(4);

  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  }
}
#include <CanManager.h>
#include <Motor.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <mbed.h>

asm(".global _printf_float");  // float出力用

UnbufferedSerial pc(USBTX, USBRX);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Motor motor(0x08, manager);

DigitalIn button(BUTTON1);

int main() {
  manager.begin();
  printf("start\n");

  while (true) {
    motor.setMode(Motor::Mode::Voltage);
    motor.driveVoltage(1);
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  }
}
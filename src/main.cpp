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
  float counter = 0;
  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
    motor.setMode(Motor::Mode::Voltage);
    motor.driveVoltage(counter);
    counter += 0.1;
    if (counter >= 1) counter = -1;
    ThisThread::sleep_for(100ms);
  }
}
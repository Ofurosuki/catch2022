#include <CanManager.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <mbed.h>

UnbufferedSerial pc(USBTX, USBRX);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Sensor sensor(0x11, manager);

DigitalIn button(BUTTON1);

int main() {
  manager.begin();
  printf("start\n");

  sensor.registerCallback(0, [](uint8_t sensorNum, bool state) {
    char buf[64];
    sprintf(buf, "sensor %d: %d\n", sensorNum, state);
    pc.write(buf, 64);
  });
  uint8_t counter = 0;
  while (1) {
    // put your main code here, to run repeatedly:
    printf("state: %d, %d\n", sensor.getStates(), counter++);
    ThisThread::sleep_for(100ms);
  }
}
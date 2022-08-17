#include <CanManager.h>
#include <Motor.h>
#include <PC.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <mbed.h>

asm(".global _printf_float");  // float出力用

UnbufferedSerial pc(USBTX, USBRX, 115200);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Motor motor(0x08, manager);
PC pcConnector;

DigitalOut led(LED1);
DigitalIn button(BUTTON1);

int main() {
  manager.begin();
  pcConnector.registerCallback(0x01, [&](uint8_t* data, size_t size) {
    led = !led;
    asm("nop");
  });
  while (true) {
  }
}

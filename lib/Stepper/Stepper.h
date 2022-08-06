#pragma once

#include <CanManager.h>
#include <mbed.h>

class Stepper {
 public:
  Stepper(PinName dir, PinName pulse, uint16_t pulsePerRev, uint16_t maxSpeed,
          uint16_t maxAccel)
      : dir(dir),
        pulse(pulse),
        ppr(ppr),
        maxSpeed(maxSpeed),
        maxAccel(maxAccel) {}

  void setTargetPosition(float pos);
  float getCurrentPosition();

 private:
  DigitalOut dir;
  DigitalOut pulse;
  uint16_t ppr;
  uint16_t maxSpeed;
  uint16_t maxAccel;
};

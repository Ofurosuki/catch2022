#pragma once

#include <CanManager.h>
#include <mbed.h>

class Servo {
 public:
  Servo(uint32_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {}

  void setPosition(float degree,
                   float velocity = INFINITY);  // velocity - deg/s
  void setVelocity(float velocity);             // velocity - deg/s

 private:
  uint32_t hardId;
  CanManager& manager;
};
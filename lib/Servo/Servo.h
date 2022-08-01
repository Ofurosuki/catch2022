#pragma once

#include <CanManager.h>
#include <mbed.h>

class Servo {
 public:
  Servo(uint32_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {}

  // positionは角度指定
  void setPosition(uint8_t num, float position);

 private:
  uint32_t hardId;
  CanManager& manager;
};
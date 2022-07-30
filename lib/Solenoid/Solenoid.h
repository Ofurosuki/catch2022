#pragma once

#include <CanManager.h>
#include <mbed.h>

class Solenoid {
 public:
  Solenoid(uint32_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {}
  void driveSingle(uint8_t num, bool state, uint32_t duration = 0);
  void drivePair(uint8_t num, bool state, uint32_t duration = 0);

 private:
  uint32_t hardId;
  CanManager& manager;
};

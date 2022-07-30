#pragma once

#include <CanManager.h>
#include <mbed.h>

class Sensor {
 public:
  Sensor(uint16_t hardId, CanManager& manager);

  void registerCallback(uint8_t sensorNum,
                        Callback<void(uint8_t, bool)> callback);
  void managerCallback(CanManager::Message msg);
  bool getState(uint8_t sensorNum);
  uint8_t getStates() { return state; }

 private:
  uint16_t hardId;
  CanManager& manager;

  Callback<void(uint8_t, bool)> callbacks[8];
  uint8_t state = 0;
};
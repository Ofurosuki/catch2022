#include "Sensor.h"

Sensor::Sensor(uint16_t hardId, CanManager& manager)
    : hardId(hardId), manager(manager) {
  manager.registerCallback(callback(this, &Sensor::managerCallback), hardId);
}

void Sensor::registerCallback(uint8_t sensorNum,
                              Callback<void(uint8_t, bool)> callback) {
  callbacks[sensorNum] = callback;
}

void Sensor::managerCallback(CanManager::Message msg) {
  if (msg.hardId != hardId) return;

  uint8_t sensorNum;

  switch (msg.cmdId) {
    case 3:
      sensorNum = (uint8_t) * (float*)(msg.data + 4);
      if (sensorNum >= 8) return;
      state |= 1 << sensorNum;
      if (!callbacks[sensorNum]) break;
      callbacks[sensorNum](sensorNum, 1);
      break;

    case 6:
      uint8_t r = *(float*)(msg.data + 4);
      state = (uint8_t) * (float*)(msg.data + 4);
      asm("nop");
      break;
  }
}

bool Sensor::getState(uint8_t sensorNum) { return state & (1 << sensorNum); }
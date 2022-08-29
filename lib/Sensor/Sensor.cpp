#include "Sensor.h"

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
      state = (uint8_t) * (float*)(msg.data + 4);
      break;
  }
}

bool Sensor::getState(uint8_t sensorNum) { return state & (1 << sensorNum); }
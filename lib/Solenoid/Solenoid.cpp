#include "Solenoid.h"

void Solenoid::driveSingle(uint8_t num, bool state, uint32_t duration) {
  CanManager::Message msg;
  msg.hardId = hardId;
  msg.cmdId = 1;
  msg.data[0] = num;
  msg.data[1] = state;
  *(uint32_t*)&msg.data[2] = duration;
  manager.send(msg);
}

void Solenoid::drivePair(uint8_t num, bool state, uint32_t duration) {
  CanManager::Message msg;
  msg.hardId = hardId;
  msg.cmdId = 2;
  msg.data[0] = num;
  msg.data[1] = state;
  *(uint32_t*)&msg.data[2] = duration;
  manager.send(msg);
}
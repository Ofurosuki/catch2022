#include "Servo.h"

void Servo::setPosition(uint8_t num, float position) {
  if (num >= 3) return;
  CanManager::Message msg;
  msg.hardId = hardId;
  msg.cmdId = num + 1;
  *(float*)&msg.data[0] = position;
  manager.send(msg);
}
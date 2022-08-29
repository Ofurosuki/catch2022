#include "Servo.h"

void Servo::setPosition(float position, float velocity) {
  CanManager::Message msg;
  msg.hardId = hardId;
  msg.cmdId = 0x01;
  *(float*)&msg.data[0] = position;
  *(float*)&msg.data[4] = velocity;
  manager.send(msg);
}

void Servo::setVelocity(float velocity) {
  CanManager::Message msg;
  msg.hardId = hardId;
  msg.cmdId = 0x02;
  *(float*)&msg.data[0] = velocity;
  manager.send(msg);
}
#include "Motor.h"

void Motor::receive(CanManager::Message msg) {
  if (msg.hardId != hardId) return;
  switch (msg.cmdId) {
    case 0x10:
      currentPosition = *(float*)msg.data;
      break;
  }
}

void Motor::setMode(Mode mode) {
  uint8_t data[8] = {0};
  switch (mode) {
    case Mode::Position:
      data[0] = 0x00;
      break;
    case Mode::Velocity:
      data[0] = 0x01;
      break;
    case Mode::Voltage:
      data[0] = 0x03;
      break;
    case Mode::Stop:
      data[0] = 0x04;
      break;
  }
  CanManager::Message msg(hardId, 0x02, data);
  manager.send(msg);
}

void Motor::drivePosition(float position) {
  uint8_t data[8] = {0};
  *(float*)data = position;
  manager.send(CanManager::Message(hardId, 0x03, data));
  currentCommandPosition = -position;
  startPosition = currentPosition;
}

void Motor::driveVelocity(float velocity) {
  uint8_t data[8] = {0};
  *(float*)data = velocity;
  manager.send(CanManager::Message(hardId, 0x04, data));
}

void Motor::driveVoltage(float voltage) {
  uint8_t data[8] = {0};
  *(float*)data = voltage;
  manager.send(CanManager::Message(hardId, 0x06, data));
}

void Motor::reset() {
  uint8_t data[8] = {0};
  manager.send(CanManager::Message(hardId, 0x01, data));
}

void Motor::resetPosition(float defaultPosition) {
  uint8_t data[8] = {0};
  *(float*)data = -defaultPosition;
  manager.send(CanManager::Message(hardId, 0x09, data));
}

float Motor::getPositionProgress() {
  if (currentCommandPosition - startPosition == 0) return 0;
  return 1 - (currentCommandPosition - currentPosition) /
                 (currentCommandPosition - startPosition);
}
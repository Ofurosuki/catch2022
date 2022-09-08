#pragma once

#include <CanManager.h>
#include <mbed.h>

class Motor {
 public:
  enum class Mode { Stop, Position, Velocity, Voltage };

  Motor(uint16_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {
    // manager.registerCallback(callback(this, &Motor::receive), hardId);
  }

  void receive(CanManager::Message msg);

  void setMode(Mode mode);
  void drivePosition(float position);
  void driveVelocity(float velocity);
  void driveVoltage(float voltage);

  void reset();
  void resetPosition(float defaultPosition = 0);

  float getPositionProgress();
  float getCurrentPosition() { return currentPosition; }

 private:
  uint16_t hardId;
  CanManager& manager;

  float startPosition = 0;
  float currentCommandPosition = 0;
  float currentPosition = 0;
};
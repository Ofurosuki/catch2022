#pragma once

#include <CanManager.h>
#include <mbed.h>

class Motor {
 public:
  enum class Mode { Stop, Position, Velocity, Voltage };

  Motor(uint16_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {
    manager.registerCallback(hardId, callback(this, &Motor::receive));
  }

  // 動作モード指定。指定したモードと違うdrive命令は無視される
  void setMode(Mode mode);

  // 位置指定
  void drivePosition(float position);
  // 速度指定
  void driveVelocity(float velocity);
  // 電圧指定
  void driveVoltage(float voltage);

  // MD基板をリセットする
  void reset();
  // エンコーダーの値をリセットする
  void resetPosition(float defaultPosition = 0);

  // 位置指定で動作中に、スタートからゴールまで何%動いたかを返す
  float getPositionProgress();
  // 現在位置を返す
  float getCurrentPosition() { return currentPosition; }

  // CanManagerから呼び出されるメソッド。呼び出さないこと
  void receive(CanManager::Message msg);

 private:
  uint16_t hardId;
  CanManager& manager;

  float startPosition = 0;
  float currentCommandPosition = 0;
  float currentPosition = 0;
};
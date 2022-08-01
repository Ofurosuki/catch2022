#pragma once

#include <CanManager.h>
#include <mbed.h>

class Sensor {
 public:
  Sensor(uint16_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {
    manager.registerCallback(hardId, callback(this, &Sensor::managerCallback));
  }

  // Sensor基板の各スイッチごとに、押されたときのコールバック関数を登録する
  void registerCallback(uint8_t sensorNum,
                        Callback<void(uint8_t, bool)> callback);

  // スイッチ番号を入れるとその状態を返す
  bool getState(uint8_t switchNum);
  // すべての状態を返す
  uint8_t getStates() { return state; }

  // CanManagerから呼び出されるメソッド。呼び出さないこと
  void managerCallback(CanManager::Message msg);

 private:
  uint16_t hardId;
  CanManager& manager;

  Callback<void(uint8_t, bool)> callbacks[8];
  uint8_t state = 0;
};
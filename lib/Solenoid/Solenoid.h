#pragma once

#include <CanManager.h>
#include <mbed.h>

class Solenoid {
 public:
  Solenoid(uint32_t hardId, CanManager& manager)
      : hardId(hardId), manager(manager) {}

  // 指定した番号のソレノイド1つをstateに指定した状態にする
  // durationを指定すると、そのミリ秒後に元の状態に戻る
  void driveSingle(uint8_t num, bool state, uint32_t duration = 0);

 private:
  uint32_t hardId;
  CanManager& manager;
};

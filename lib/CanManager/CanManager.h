#pragma once

#include <mbed.h>

#include <map>

class CanManager {
 public:
  struct Message {
    Message(uint16_t hardId, uint16_t cmdId, uint8_t* _data, size_t len = 8)
        : hardId(hardId), cmdId(cmdId) {
      memcpy(data, _data, len);
    }
    Message() : hardId(0), cmdId(0) {}
    uint16_t hardId;
    uint16_t cmdId;
    uint8_t data[8] = {0};
  };

  CanManager(RawCAN& can) : can(can) {
    can.frequency(500E3);
    can.attach(callback(this, &CanManager::receive), RawCAN::RxIrq);
  }

  void receive();
  void send(Message msg);
  void registerCallback(Callback<void(Message)> callback, uint32_t hardId);

 private:
  RawCAN& can;
  std::map<uint32_t, Callback<void(Message)>> callbacks;
};
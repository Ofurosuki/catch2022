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

  CanManager(RawCAN& can);

  // main関数の最初で呼び出すこと
  void begin();

  // CANバスに値を送る
  void send(Message msg);
  // 値を受信したときに呼び出すコールバック関数を登録する
  void registerCallback(uint32_t hardId, Callback<void(Message)> callback);

  // RawCANから呼び出されるメソッド。呼び出さないこと
  void receive();

 private:
  RawCAN& can;
  std::map<uint32_t, Callback<void(Message)>> callbacks;
};
#include "CanManager.h"

void CanManager::receive() {
  CANMessage msg;
  while (can.read(msg)) {
    uint32_t hardId = msg.id >> 5;
    auto callback = callbacks.find(hardId);
    if (callback == callbacks.end()) continue;
    callback->second(
        Message(msg.id >> 6, msg.id & 0b0111111, (uint8_t*)msg.data));
  }
}

void CanManager::send(Message msg) {
  CANMessage canMsg;
  canMsg.id = msg.hardId << 6 | msg.cmdId;
  memcpy(canMsg.data, msg.data, 8);
  can.write(canMsg);
}

void CanManager::registerCallback(Callback<void(Message)> callback,
                                  uint32_t hardId) {
  callbacks[hardId] = callback;
}
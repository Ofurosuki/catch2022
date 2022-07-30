#include "CanManager.h"

CanManager::CanManager(RawCAN& can) : can(can) {}

void CanManager::begin() {
  can.frequency(500E3);
  can.attach(callback(this, &CanManager::receive));
}

void CanManager::receive() {
  CANMessage msg;
  if (can.read(msg)) {
    uint32_t hardId = msg.id >> 6;
    auto callback = callbacks.find(hardId);
    if (callback == callbacks.end()) return;
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
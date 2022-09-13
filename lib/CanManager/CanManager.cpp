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
  CANMessage canMsg(msg.hardId << 6 | msg.cmdId, (char*)msg.data);
  char counter = 0;
  while (can.write(canMsg) != 1 && counter < 3) {
    counter++;
    wait_us(200);
  }
}

void CanManager::registerCallback(uint32_t hardId,
                                  Callback<void(Message)> callback) {
  callbacks[hardId] = callback;
}
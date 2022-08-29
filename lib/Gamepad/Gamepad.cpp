#include "Gamepad.h"  //PC.cpp or PC.hを経由してゲームパッドから受信

int Gamepad::getAxis(uint8_t num) {
  if (num < 4) return axis[num];
  return 0;
}

bool Gamepad::getButton(uint8_t num) {
  if (num < buttonNum) {
    return button[num];
  }
  return false;
}

int Gamepad::getHat(uint8_t num) {
  if (num < 2) return hat[num];
  return 0;
}

void Gamepad::registerButtonCallback(uint8_t num, ButtonMode mode,
                                     Callback<void()> callback) {
  buttonCallbacks[num][mode] = callback;
}

void Gamepad::pcCallback(uint8_t* data, size_t size) {
  buttonNum = data[5];
  size_t expectedSize =
      6 + (buttonNum + 8 - 1) / 8;  // buttonNumを8で割って切り上げしている
  if (size != expectedSize) return;
  memcpy(axis, (char*)data, 4);

  uint8_t hatState = data[4];
  hat[0] = (hatState & 0x01) + (hatState >> 1 & 0x01) * -1;
  hat[1] = (hatState >> 2 & 0x01) + (hatState >> 3 & 0x01) * -1;

  for (size_t i = 0; i < buttonNum; i++) {
    bool prevButton = button[i];
    button[i] = data[6 + i / 8] & (1 << (i % 8));
    if (button[i] != prevButton) {
      if (buttonCallbacks[i][Both]) buttonCallbacks[i][Both]();
      if (button[i]) {
        if (buttonCallbacks[i][Down]) buttonCallbacks[i][Down]();
      } else {
        if (buttonCallbacks[i][Up]) buttonCallbacks[i][Up]();
      }
    }
  }
}
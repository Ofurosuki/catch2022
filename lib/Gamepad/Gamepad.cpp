#include "Gamepad.h"

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

void Gamepad::pcCallback(uint8_t* data, size_t size) {
  buttonNum = data[4];
  size_t expectedSize =
      5 + (buttonNum + 8 - 1) / 8;  // buttonNumを8で割って切り上げしている
  if (size != expectedSize) return;
  memcpy(axis, (char*)data, 4);
  for (size_t i = 0; i < buttonNum; i++) {
    button[i] = data[5 + i / 8] & (1 << (i % 8));
  }
}
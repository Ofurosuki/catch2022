#pragma once

#include <mbed.h>

#include <map>

class Gamepad {
 public:
  Gamepad() {}
  int getAxis(uint8_t num);  // -100 ~ 99
  bool getButton(uint8_t num);
  int getHat(uint8_t num);  // -1 ~ 1
  void registerButtonCallback(uint8_t num, Callback<void()> callback);

  void pcCallback(uint8_t* data, size_t size);

 private:
  signed char axis[4];  // signedをつけないとマイナスにならない
  volatile bool button[20];
  signed char hat[2];
  volatile size_t buttonNum = 0;
  Callback<void()> buttonCallbacks[12];
};
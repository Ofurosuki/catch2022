#pragma once

#include <mbed.h>

#include <map>

class Gamepad {
 public:
  enum ButtonMode { Down = 0, Up, Both };

  Gamepad() {}
  int getAxis(uint8_t num);  // -100 ~ 99
  bool getButton(uint8_t num);
  int getHat(uint8_t num);  // -1 ~ 1
  void registerButtonCallback(uint8_t num, ButtonMode mode,
                              Callback<void()> callback);

  void pcCallback(uint8_t* data, size_t size);

 private:
  signed char axis[4];  // signedをつけないとマイナスにならない
  volatile bool button[20];
  signed char hat[2];
  volatile size_t buttonNum = 0;
  std::map<uint8_t, Callback<void()>[3]> buttonCallbacks;
};
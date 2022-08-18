#pragma once

#include <mbed.h>

class Gamepad {
 public:
  Gamepad() {}
  int getAxis(uint8_t num);  // -100 ~ 100
  bool getButton(uint8_t num);

  void pcCallback(uint8_t* data, size_t size);

 private:
  signed char axis[4];  // signedをつけないとマイナスにならない
  volatile bool button[20];
  volatile size_t buttonNum = 0;
};
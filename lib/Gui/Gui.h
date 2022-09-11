#pragma once

#include <mbed.h>

class Gui {
 public:
  enum CommandMode {
    ownArea = 0,
    commonArea = 1,
    shootingBox = 2,
  };
  struct Command {
    CommandMode mode;
    uint8_t destination0;
    uint8_t destination1;
    bool enableSuckers[3];
    bool isHigher;
  };

  Gui() {}

  void pcVectorCallback(uint8_t* data, size_t size);
  void pcSuckerCallback(uint8_t* data, size_t size);

  Command getCommand() { return command; }

  bool checkNewVector() {
    if (!newVector) return false;
    newVector = false;
    return true;
  }
  bool checkNewSucker() {
    if (!newSucker) return false;
    newSucker = false;
    return true;
  }

 private:
  Command command;
  bool newVector = false;
  bool newSucker = false;
};
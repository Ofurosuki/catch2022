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
    uint8_t destination;
    bool enableSuckers[3];
    bool isHigher;
  };

  Gui() {}
  void registerCallback(Callback<void(Command)> func) { callback = func; }
  void removeCallback() { callback = nullptr; }
  void pcCallback(uint8_t* data, size_t size);

  Command getCommand() { return command; }

  bool checkNewConfig() {
    if (!newConfig) return false;
    newConfig = false;
    return true;
  }

 private:
  Callback<void(Command)> callback;
  Command command;
  bool newConfig = false;
};
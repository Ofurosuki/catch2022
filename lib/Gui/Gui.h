#pragma once

#include <mbed.h>

class Gui {
 public:
  struct GuiConfig {
    uint8_t getFrom;
    uint8_t moveTo;
    bool enableSuckers[3];
    bool isHigher;
  };

  Gui() {}
  void registerCallback(Callback<void(GuiConfig)> func) { callback = func; }
  void removeCallback() { callback = nullptr; }
  void pcCallback(uint8_t* data, size_t size);

  GuiConfig getConfig() { return config; }

  bool checkNewConfig() {
    if (!newConfig) return false;
    newConfig = false;
    return true;
  }

 private:
  Callback<void(GuiConfig)> callback;
  GuiConfig config;
  bool newConfig = false;
};
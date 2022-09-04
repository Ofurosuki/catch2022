#include "Gui.h"

void Gui::pcCallback(uint8_t* data, size_t size) {
  config.getFrom = data[0];
  config.moveTo = data[1];
  config.enableSuckers[2] = data[2] & 0b0001;
  config.enableSuckers[2] = data[2] & 0b0010;
  config.enableSuckers[2] = data[2] & 0b0100;
  config.isHigher = data[2] & 0b1000;

  newConfig = true;
  if (callback == nullptr) return;
  callback(config);
}
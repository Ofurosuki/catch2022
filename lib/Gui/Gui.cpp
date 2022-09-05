#include "Gui.h"

void Gui::pcCallback(uint8_t* data, size_t size) {
  if (size == 0 || size > 6) return;
  if (data[0] > 2) return;
  config.mode = (CommandMode)data[0];
  config.destination = data[1];
  config.isHigher = data[2] & 0b1000;
  config.enableSuckers[0] = data[3];
  config.enableSuckers[1] = data[4];
  config.enableSuckers[2] = data[5];

  newConfig = true;
  if (callback == nullptr) return;
  callback(config);
}
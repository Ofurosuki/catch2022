#include "Gui.h"

void Gui::pcCallback(uint8_t* data, size_t size) {
  if (size == 0 || size > 6) return;
  if (data[0] > 2) return;
  command.mode = (CommandMode)data[0];
  command.destination = data[1];
  command.isHigher = data[2] & 0b1000;
  command.enableSuckers[0] = data[3];
  command.enableSuckers[1] = data[4];
  command.enableSuckers[2] = data[5];

  newConfig = true;
  if (callback == nullptr) return;
  callback(command);
}
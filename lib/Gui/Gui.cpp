#include "Gui.h"

void Gui::pcCallback(uint8_t* data, size_t size) {
  if (size == 0 || size > 7) return;
  if (data[0] > 2) return;
  command.mode = (CommandMode)data[0];
  command.destination0 = data[1];
  command.destination1 = data[2];
  command.isHigher = data[3] & 0b1000;
  command.enableSuckers[0] = data[4];
  command.enableSuckers[1] = data[5];
  command.enableSuckers[2] = data[6];

  newConfig = true;
  if (callback == nullptr) return;
  callback(command);
}
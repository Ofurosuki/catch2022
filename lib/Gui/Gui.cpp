#include "Gui.h"

void Gui::pcVectorCallback(uint8_t* data, size_t size) {
  if (size == 0 || size > 4) return;
  if (data[0] > 2) return;
  command.mode = (CommandMode)data[0];
  command.destination0 = data[1];
  command.destination1 = data[2];
  command.isHigher = data[3] & 0b1000;

  newVector = true;
}

void Gui::pcSuckerCallback(uint8_t* data, size_t size) {
  if (size == 0 || size > 3) return;
  command.enableSuckers[0] = data[0];
  command.enableSuckers[1] = data[1];
  command.enableSuckers[2] = data[2];
  newSucker = true;
}
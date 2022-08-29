#include "PC.h"

PC::PC() : pc(USBTX, USBRX, 115200) {
  pc.attach(callback(this, &PC::serialCallback));
}

void PC::registerCallback(uint8_t id,
                          Callback<void(uint8_t*, size_t)> callback) {
  callbacks[id] = callback;
}

void PC::parse(uint8_t* data, size_t size) {
  uint8_t nextZero = data[0];
  for (size_t i = 1; i < size; i++) {
    nextZero--;
    if (nextZero == 0) {
      nextZero = data[i];
      data[i] = 0;
    }
  }
  uint8_t id = data[1];
  if (callbacks.find(id) != callbacks.end()) {
    callbacks[id](data + 2, size - 3);
  }
}

void PC::serialCallback() {
  while (pc.readable()) {
    uint8_t c;
    pc.read(&c, 1);
    receiveBuf[receiveCounter] = c;
    receiveCounter++;
    if (receiveCounter >= receiveBufSize) {
      receiveCounter = 0;
    }

    if (c != 0) return;
    parse(receiveBuf, receiveCounter);
    receiveCounter = 0;
  }
}

void PC::send(uint8_t id, uint8_t* data, size_t size) {
  constexpr size_t dataSizeMax = 16;
  constexpr size_t bufSize = dataSizeMax + 3;
  uint8_t buf[bufSize] = {0};
  if (size > dataSizeMax) {
    return;
  }
  buf[1] = id;
  memcpy(buf + 2, data, size);
  uint8_t zeroCounter = 0;
  for (int i = size + 2 - 1; i >= 0; i--) {
    if (buf[i] != 0) {
      zeroCounter++;
      continue;
    }
    buf[i] = zeroCounter + 1;
    zeroCounter = 0;
  }
  pc.write(buf, size + 3);
}

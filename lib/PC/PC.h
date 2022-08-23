#include <mbed.h>

#include <map>

constexpr size_t receiveBufSize = 256;

class PC {
 public:
  PC();
  void registerCallback(uint8_t id, Callback<void(uint8_t*, size_t)> callback);
  void send(uint8_t id, uint8_t* data, size_t size);
  void serialCallback();

 private:
  std::map<uint8_t, Callback<void(uint8_t*, size_t)>> callbacks;
  UnbufferedSerial pc;

  uint8_t receiveBuf[receiveBufSize] = {0};
  size_t receiveCounter;

  void parse(uint8_t* data, size_t size);
};
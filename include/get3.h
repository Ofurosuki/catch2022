#pragma once

#include <mbed.h>

#include <tinyfsm.hpp>

#include "main.h"

struct CallGet3 : tinyfsm::Event {
  uint8_t positionNum;
};

class Get3 : public tinyfsm::Fsm<Get3> {
 public:
  void react(tinyfsm::Event const &) {}

  virtual void react(CallGet3 const &);

  virtual void entry(void){};
  void exit(void){};
};
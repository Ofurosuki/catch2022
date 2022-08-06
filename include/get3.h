#pragma once

#include <mbed.h>

#include <tinyfsm.hpp>

#include "main.h"

struct CallGet3 : tinyfsm::Event {
  uint8_t positionNum;
};

class Get3 : public tinyfsm::Fsm<Get3> {
 public:
  enum Target { target0, target1, target2, target3, None };

  void react(tinyfsm::Event const &) {}

  virtual void react(CallGet3 const &);

  virtual void entry(void){};
  void exit(void){};

  static void setTarget(Target target);

 protected:
  static Target target;
};
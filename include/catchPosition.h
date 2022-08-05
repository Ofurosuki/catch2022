#pragma once

#include <tinyfsm.hpp>

struct CallCatchPosition : tinyfsm::Event {};

class CatchPosition : public tinyfsm::Fsm<CatchPosition> {
 public:
  void react(tinyfsm::Event const &) {}
  virtual void react(CallCatchPosition const &);

  virtual void entry(void){};
  void exit(void){};
};
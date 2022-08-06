#include "catchPosition.h"

#include "get3.h"

class ready : public CatchPosition {
 public:
  void entry() override {}
  void react(CallCatchPosition const& e) override {}
};

class position0 : public CatchPosition {
 public:
  void entry() override {
    Get3::setTarget(Get3::Target::target0);
    Get3::set_initial_state();
  }
  void react(CallCatchPosition const& e) override {
    Get3::dispatch(CallGet3());
  }
};

FSM_INITIAL_STATE(CatchPosition, position0);
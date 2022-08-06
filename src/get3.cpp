#include "get3.h"

Get3::Target target = Get3::Target::None;

class step0 : public Get3 {
 public:
  void entry() override {
    motor.drivePosition(tablePos[target]);
    rStepper.setTargetPosition(rPos[target]);
    thetaStepper.setTargetPosition(thetaPos[target]);
    upDownStepper.setTargetPosition(0);
  }
  void react(CallGet3 const& e) override {
    if (motor.getPositionProgress() > 0.8) {
      transit<step1>();
    }
  }

 private:
  float tablePos[4] = {0, 0, 0, 0};
  float rPos[4] = {0, 0, 0, 0};
  float thetaPos[4] = {0, 0, 0, 0};
};

class step1 : public Get3 {
 public:
  void entry() override {}
  void react(CallGet3 const& e) override {}
};

FSM_INITIAL_STATE(Get3, step0);
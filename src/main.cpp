#include <CanManager.h>
#include <Gamepad.h>
#include <Gui.h>
#include <Motor.h>
#include <PC.h>
#include <Sensor.h>
#include <Servo.h>
#include <Solenoid.h>
#include <config.h>
#include <field_data.h>
#include <mbed.h>
#include <stepper.h>
#include <stepper_rotate.h>

asm(".global _printf_float");  // float出力用

UnbufferedSerial pc(USBTX, USBRX);

RawCAN can(PA_11, PA_12, 500E3);
CanManager manager(can);
Motor motor(0x01, manager);
Servo servo(0x02, manager);
Solenoid solenoid(0x03, manager);
Sensor sensor(0x04, manager);

rotate_stepper stepper_z(DIR_z, STP_z);
rotate_stepper stepper_theta(DIR_theta, STP_z);
rotate_stepper stepper_r(DIR_r, STP_r);

PC pcConnector;
Gamepad gamepad;
Gui gui;

DigitalOut led(LED1);
DigitalIn button(BUTTON1);

void move(position pos);

void init(Team team) {
  manager.begin();
  const int stepper_vel_for_init = 10;
  const float motor_voltage_for_init = 1.0;
  const float revolution_num_rightside = 100.1;
  //右端についたときの回転数
  const int step_num_maxium = 104;
  // r最大値

  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  pcConnector.registerCallback(0x02, callback(&gui, &Gui::pcCallback));

  sensor.registerCallback(0, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(0);
  });
  // DC left limit

  sensor.registerCallback(1, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(revolution_num_rightside);
  });  // DC Right limit
  sensor.registerCallback(2, [=](uint8_t, bool) { stepper_r.reset(0); });
  // Stepper for r min_lim
  sensor.registerCallback(3, [=](uint8_t, bool) {
    stepper_r.reset(step_num_maxium);
  });  // Stepper for r max_lim
  sensor.registerCallback(
      4, [=](uint8_t, bool) { stepper_z.reset(0); });  // Stepper for r z_max
  sensor.registerCallback(
      5, [=](uint8_t, bool) { stepper_r.reset(0); });  // Stepper for theta

  // reset servo
  if (team) {
    motor.driveVoltage(-motor_voltage_for_init);
    stepper_r.rotate_vel(stepper_vel_for_init);
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootBlue[i];
    }
    // sw0 に向かって押す　(blue)

    // theta=0が基準点
  } else {
    motor.driveVoltage(motor_voltage_for_init);
    stepper_r.rotate_vel(stepper_vel_for_init);
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootRed[i];
    }
    // sw1　にむかって押す(red)
    // theta=0 へ向かったあと、theta=180に向かう
  }
}

int main() {
  init(Red);
  // stepper0.set_config(50, 1000, 100);   set acceleration and  max velocity
  //  stepper0.step(20, -1000);  // rotate stepper (initial frequency and target
  //   step )回すだけの関数もある

  // stepper0.set_theta_config(0, 100);
  //起動時の角度を指定、1ステップあたりの角度を指定
  //  while(button){}
  //  stepper0.rotate(2000);
  while (button) {
  }
  // stepper0.step(300, -20000);
  //  stepper0.rotate(360);

  enum jaga_group {
    jaga_a = 1,
    jaga_b,
    jaga_c,
    jaga_d,
    jaga_e,
    jaga_f  //これらの値がｐｃから送られてくる
  };
  enum shoot_group { shoot_a = 1, shoot_b, shoot_c, shoot_d, shoot_e, shoot_f };

  void catch_jaga(float z);
  void move(position pos);
  bool is_field_red = true;
  bool is_catch_successful = false;
  bool is_waiting_for_input = true;
  bool is_finished = true;

  while (true) {
    // scanf的な何かでselecting_placeに値を入れる
    bool is_waiting_for_input = true;
    while (!gui.checkNewConfig()) {
    }
    is_waiting_for_input = false;

    switch (gui.getConfig().moveTo) {
      case jaga_a:
        printf("moving to jaga_a\n");
        move(jaga[1]);
        break;
      case jaga_b:
        printf("moving to jaga_b\n");
        move(jaga[2]);
        break;
      case jaga_c:
        printf("moving to jaga_c\n");
        move(jaga[3]);
        break;
      case jaga_d:
        printf("moving to jaga_d\n");
        move(jaga[4]);
        break;
      case jaga_e:
        printf("moving to jaga_e\n");
        move(jaga[5]);
        break;
      case jaga_f:
        printf("moving to jaga_f\n");
        move(jaga[6]);
        break;
      default:
        break;
    }
    is_waiting_for_input = false;
    while (!is_finished) {  //作業完了をまつ
      ThisThread::sleep_for(100ms);
    }
    while (!is_catch_successful) {
      is_waiting_for_input = true;
      while (!gamepad.getButton(1)) {
        // Kei-chan's code (controller)
      }
      is_waiting_for_input = false;
      catch_jaga();
      while (gamepad.getButton(1) == 0 && gamepad.getButton(2) == 0) {
        if (gamepad.getButton(1) == 1) {
          is_catch_successful = true;
        } else {
          is_catch_successful = false;
        }
      }
    }
    is_waiting_for_input = true;
    while (!gui.checkNewConfig()) {
    }
    is_waiting_for_input = false;

    switch (gui.getConfig().moveTo) {
      case shoot_a:
        move(shoot[0]);
        break;
      case shoot_b:
        move(shoot[3]);
        break;
      case shoot_c:
        move(shoot[6]);
        break;
      default:
        break;
    }
    is_waiting_for_input = false;
    while (!is_finished) {
      ThisThread::sleep_for(100ms);
    }
  }

  //以下修正版

  is_waiting_for_input = true;
  while (!gui.checkNewConfig()) {
  }
  is_waiting_for_input = false;

  switch (gui.getCommand().mode) {
    case gui.CommandMode::ownArea:
      break;
    case gui.CommandMode::commonArea:
      break;
    case gui.CommandMode::shootingBox:
      if (gui.getCommand().enableSuckers[0] &&
          gui.getCommand().enableSuckers[1] &&
          gui.getCommand().enableSuckers[2]) {
        move(shoot[gui.getCommand().destination]);
      }
      break;
    default:
      break;
  }
  is_waiting_for_input = true;
  while (gamepad.getButton(1) == 0) {
    // keichan's code here
  }
  is_waiting_for_input = true;
  if (gui.getCommand().isHigher) {
  }
}

/*
  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  } */

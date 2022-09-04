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

enum Team { Red = 0, Blue };

void move(position pos);

void init(Team team) {
  manager.begin();
  const int stepper_vel_for_init = 10;
  const float motor_voltage_for_init = 1.0;

  // reset servo
  if (team) {
    motor.driveVoltage(-motor_voltage_for_init);
    stepper_r.rotate_vel(stepper_vel_for_init);
    // sw0 に向かって押す　(blue)

    // theta=0が基準点
  } else {
    motor.driveVoltage(motor_voltage_for_init);
    stepper_r.rotate_vel(stepper_vel_for_init);
    // sw1　にむかって押す
    // theta=0 へ向かったあと、theta=180に向かう
  }
}

int main() {
  init(Red);
  const float revolution_num_rightside = 100.1;
  //右端についたときの回転数
  const int step_num_maxium = 104;
  // r最大値

  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  pcConnector.registerCallback(0x02, callback(&gui, &Gui::pcCallback));

  sensor.registerCallback(1, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(0);
  });
  // DC left limit

  sensor.registerCallback(1, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(revolution_num_rightside);
  });                            // DC Right limit
  sensor.registerCallback(2, );  // Stepper for r min_lim
  sensor.registerCallback(3);    // Stepper for r max_lim
  sensor.registerCallback(4);    // Stepper for r z_max
  sensor.registerCallback(5);    // Stepper for theta

  stepper0.set_config(50, 1000, 100);  // set acceleration and  max velocity
  // stepper0.step(20, -1000);  // rotate stepper (initial frequency and target
  //  step )回すだけの関数もある

  stepper0.set_theta_config(0, 100);
  //起動時の角度を指定、1ステップあたりの角度を指定
  // while(button){}
  // stepper0.rotate(2000);
  while (button) {
  }
  stepper0.step(300, -20000);
  // stepper0.rotate(360);

  while (true) {
    printf("progresscnt:%f\n", stepper0.progress_cnt());
  }

  enum jaga_group {
    jaga_a = 0,
    jaga_b = 1,
    jaga_c = 2  //これらの値がｐｃから送られてくる
  };
  enum shoot_group { shoot_a = 0, shoot_b = 1, shoot_c = 2 };
  void catch_jaga();
  void move();
  bool is_field_red = true;
  bool is_catch_successful = false;
  bool is_waiting_for_input = true;
  bool is_finished = true;

  while (true) {
    // scanf的な何かでselecting_placeに値を入れる
    int selecting_place = 1;
    bool is_waiting_for_input = true;

    bool x_button = gamepad.getButton(1);
    bool o_button = gamepad.getButton(2);

    switch (selecting_place) {
      case jaga_a:
        printf("moving to jaga_a\n");
        move(jaga_a);
        break;
      case jaga_b:
        printf("moving to jaga_b\n");
        move(jaga_b);
        break;
      case jaga_c:
        printf("moving to jaga_c\n");
        move(jaga_c);
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
      while (!x_button) {
        // Kei-chan's code (controller)
      }
      is_waiting_for_input = false;
      catch_jaga();
      while (x_button == 0 && o_button == 0) {
        if (x_button == 1) {
          is_catch_successful = true;
        } else {
          is_catch_successful = false;
        }
      }
    }
    // scanf的な何かでselecting_placeに入れる
    is_waiting_for_input = true;
    switch (selecting_place) {
      case shoot_a:
        move(shoot_a_position);
        break;
      case shoot_b:
        move(shoot_b_position);
        break;
      case shoot_c:
        move(shoot_c_position);
        break;
      default:
        break;
    }
    is_waiting_for_input = false;
    while (!is_finished) {
      ThisThread::sleep_for(100ms);
    }
  }
}

/*
  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  } */

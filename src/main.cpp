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
#include <move_them.h>
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

static bool is_waiting_for_input = true;

void ini() {
  sensor.registerCallback(0, [=](uint8_t, bool) {
    initialize(Blue);
    printf("team: we're blue team.\n");
    is_waiting_for_input = false;
  });
  sensor.registerCallback(1, [=](uint8_t, bool) {
    initialize(Red);
    printf("team: we're red team.\n");
    is_waiting_for_input = false;
  });
}
void initialize(Team team) {
  manager.begin();
  const int stepper_vel_for_init = 10;
  const float motor_voltage_for_init = 1.0;
  const float revolution_num_rightside = 100.1;
  //右端についたときの回転数
  const int step_num_maxium = 500;
  // r最大値

  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  pcConnector.registerCallback(0x02, callback(&gui, &Gui::pcCallback));

  sensor.registerCallback(0, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(0);
    printf("x1 :left limit detected\n");
  });

  sensor.registerCallback(1, [=](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(revolution_num_rightside);
    printf("x1 :right limit detected\n");
  });
  sensor.registerCallback(2, [=](uint8_t, bool) {
    stepper_r.reset(0);
    printf("r :minimum limit detected\n");
  });

  sensor.registerCallback(3, [=](uint8_t, bool) {
    stepper_r.reset(step_num_maxium);
    printf("r :maximum limit detected\n");
  });
  sensor.registerCallback(4, [=](uint8_t, bool) {
    stepper_z.reset(0);
    printf("z :maximum limit detected\n");
  });
  sensor.registerCallback(5, [=](uint8_t, bool) {
    stepper_theta.reset(0);
    printf("theta :zero point adjustment detected\n");
  });

  servo.setPosition(0);  // reset servo
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
  ini();
  while (true) {
    is_waiting_for_input = true;
    while (!gui.checkNewConfig()) {
    }
    is_waiting_for_input = false;

    switch (gui.getCommand().mode) {
      case gui.CommandMode::ownArea:
        move(jaga[gui.getCommand().destination]);
        move(sharejaga[gui.getCommand().destination]);
        //目的地到着後（シュート）
        is_waiting_for_input = true;
        gamepad_input_to_command();
        is_waiting_for_input = false;
        take_down(z_height.z_down);
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(取るときはいるのか)
        is_waiting_for_input = false;
        take_down(z_height.z_down_take);
        // if finished
        take_up();
        break;
      case gui.CommandMode::commonArea:
        move(sharejaga[gui.getCommand().destination], true);
        //目的地到着後（シュート）
        is_waiting_for_input = true;
        gamepad_input_to_command();
        is_waiting_for_input = false;
        take_down(z_height.z_down_common);
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(取るときはいるのか)
        is_waiting_for_input = false;
        take_down(z_height.z_down_common_take);
        // if finished
        take_up();
        break;

      case gui.CommandMode::shootingBox:
        if (gui.getCommand().enableSuckers[0] &&
            gui.getCommand().enableSuckers[1] &&
            gui.getCommand().enableSuckers[2]) {
          move(shoot[gui.getCommand().destination]);  // ooo
        } else if (gui.getCommand().enableSuckers[1]) {
          if (!(gui.getCommand().enableSuckers[0] ||
                gui.getCommand().enableSuckers[2])) {
            move(shoot[gui.getCommand().destination]);  // xox
          } else {
            move(shoot[gui.getCommand().destination]);  // xoo
          }
        } else {
          if (gui.getCommand().enableSuckers[0] &&
              gui.getCommand().enableSuckers[2]) {
            move(shoot[gui.getCommand().destination]);  // oxo
          } else if (gui.getCommand().enableSuckers[0] ||
                     gui.getCommand().enableSuckers[2]) {
            move(shoot[gui.getCommand().destination]);  // xxo
          } else {
            continue;  // xxx シュートへ行く意味ないのでもう一度選択させる
          }
        }

        //目的地到着後（シュート）
        is_waiting_for_input = true;
        gamepad_input_to_command();
        is_waiting_for_input = false;
        if (gui.getCommand().isHigher) {
          take_down(z_height.z_down_2nd_release);
        } else {
          take_down(z_height.z_down_release);
        }
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(落とすときはいるのかな)
        is_waiting_for_input = false;
        release_jaga();
        // if(finished)
        take_up();
        break;
      default:
        break;
    }
  }
}

/*
  while (true) {
    printf("pos: %f, %f%%\n", motor.getCurrentPosition(),
           motor.getPositionProgress() * 100);
  } */

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
#include <main.h>
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
rotate_stepper stepper_theta(DIR_theta, STP_theta);
rotate_stepper stepper_r(DIR_r, STP_r);

PC pcConnector;
Gamepad gamepad;
Gui gui;

DigitalOut led(LED1);
DigitalIn button(BUTTON1);

static bool is_waiting_for_input = true;
static bool is_Red;

void initialize(Team team) {
  stepper_r.set_theta_config(240.0f, 820.0f / 480.0f);
  stepper_r.set_config(15, 200, 5);
  stepper_theta.set_theta_config(0, 797.0f / 180.0f);
  stepper_theta.set_config(7, 100, 5);
  stepper_theta.set_max_vel_diff(0.2);
  stepper_z.set_theta_config(0, 481.0f / 154.0f);
  stepper_z.set_config(40, 300, 10);

  const int stepper_vel_for_init = 10;
  const float motor_voltage_for_init = 0.15;
  const float revolution_num_rightside = -9.2445f;
  //右端についたときの回転数
  // r最大値

  char serialBuf[64] = "";
  volatile bool is_initialized[6] = {0};

  sensor.registerCallback(0, [&](uint8_t, bool) {
    motor.reset();
    wait_us(50000);
    motor.resetPosition(revolution_num_rightside);
    is_initialized[0] = true;
    sprintf(serialBuf, "x1 :left limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(1, [&](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(0);
    is_initialized[1] = true;
    sprintf(serialBuf, "x1 :right limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(2, [&](uint8_t, bool) {
    stepper_r.rotate_vel(0);
    stepper_r.reset(0);
    is_initialized[2] = true;
    sprintf(serialBuf, "r :minimum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });

  sensor.registerCallback(3, [&](uint8_t, bool) {
    stepper_r.rotate_vel(0);
    stepper_r.reset(r_max);
    is_initialized[3] = true;
    sprintf(serialBuf, "r :maximum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(4, [&](uint8_t, bool) {
    stepper_z.rotate_vel(0);
    stepper_z.reset(0);
    stepper_z.set_max_vel_diff(1);
    is_initialized[4] = true;
    sprintf(serialBuf, "z :maximum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(5, [&](uint8_t, bool) {
    stepper_theta.rotate_vel(0);
    stepper_theta.reset(0);
    stepper_theta.set_max_vel_diff(0.2);
    is_initialized[5] = true;
    sprintf(serialBuf, "theta :zero point adjustment detected\n");
    pc.write(serialBuf, strlen(serialBuf));
    sensor.registerCallback(5, nullptr);
  });

  servo.setPosition(0);  // reset servo
  if (team == Blue) {
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootBlue[i];
    }
    for (int i = 0; i < 16; i++) {
      jaga[i] = jagaB[i];
    }
    // sw0 に向かって押す　(blue)

    // theta=0が基準点
  } else {
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootRed[i];
    }
    for (int i = 0; i < 16; i++) {
      jaga[i] = jagaR[i];
    }
    // sw1　にむかって押す(red)
    // theta=0 へ向かったあと、theta=180に向かう
  }
  ThisThread::sleep_for(10ms);
  if (!sensor.getState(5)) {
    stepper_theta.set_max_vel_diff(8);
    stepper_theta.rotate_vel(8);
  } else {
    is_initialized[5] = true;
    sensor.registerCallback(5, nullptr);
  }
  if (!sensor.getState(4)) {
    stepper_z.set_max_vel_diff(stepper_vel_for_init * 3);
    stepper_z.rotate_vel(stepper_vel_for_init * 3);
  } else {
    is_initialized[4] = true;
  }
  if (!sensor.getState(3)) {
    stepper_r.rotate_vel(stepper_vel_for_init * 2);
  } else {
    is_initialized[3] = true;
    stepper_r.reset(r_max);
  }
  motor.reset();
  ThisThread::sleep_for(10ms);

  while (!((is_initialized[0] || is_initialized[1]) && is_initialized[3] &&
           is_initialized[4] && is_initialized[5])) {
    if (team == Blue) {
      if (!sensor.getState(0)) {
        motor.driveVoltage(motor_voltage_for_init);
      } else {
        is_initialized[1] = true;
      }
    } else {
      if (!sensor.getState(1)) {
        motor.driveVoltage(-motor_voltage_for_init);
      } else {
        is_initialized[1] = true;
      }
    }
    ThisThread::sleep_for(1ms);
  }
  if (team == Red) {
    stepper_theta.rotate(-180);
    ThisThread::sleep_for(1s);
    while (stepper_theta.progress_cnt() < 0.99) {
      ThisThread::sleep_for(100ms);
    }
    stepper_theta.reset(797.0f / 180.0f * 180);
  }
}

void ini() {
  printf("Select initialize mode\n");
  while (!sensor.getState(0) && !sensor.getState(1)) {
    ThisThread::sleep_for(100ms);
  }
  if (sensor.getState(0)) {
    initialize(Blue);
    printf("team: we're blue team.\n");
    is_waiting_for_input = false;
    is_Red = false;
  } else {
    initialize(Red);
    printf("team: we're red team.\n");
    is_waiting_for_input = false;
    is_Red = true;
  }
}
int destinationBwall;  //青の壁側の座標
int pickedvac0;        // 1st picked vacuum
int pickedvac1;        // 2nd picked vacuum
int sharedir;

int main() {
  manager.begin();
  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  pcConnector.registerCallback(0x02, callback(&gui, &Gui::pcVectorCallback));
  pcConnector.registerCallback(0x03, callback(&gui, &Gui::pcSuckerCallback));
  // stepper_r.set_theta_config(240.0f, 820.0f / 480.0f);
  // stepper_r.set_config(5, 200, 5);
  // stepper_theta.set_theta_config(0, 794.0f / 180.0f);
  // stepper_theta.set_config(5, 100, 5);
  // stepper_theta.set_max_vel_diff(0.2);
  // stepper_z.set_theta_config(0, 481.0f / 154.0f);
  ini();
  // gamepad_input_to_command();
  while (true) {
    is_waiting_for_input = true;
    printf("waiting for vector input...\n");
    while (!gui.checkNewVector()) {
      ThisThread::sleep_for(100ms);
    }
    is_waiting_for_input = false;
    pickedvac0 = gui.getCommand().destination0;
    pickedvac1 = gui.getCommand().destination1;
    sharedir = gui.getCommand().destination1;

    switch (gui.getCommand().mode) {
      case gui.CommandMode::ownArea:
        //場所と方向をGUIで指定して取るとき 青赤共通
        if (pickedvac0 > pickedvac1) {
          if (((pickedvac1) % 2 == 0 &&
               (pickedvac0) % 2 == 0) ||                       // 0=even&1=odd
              ((pickedvac1) % 2 != 0 && (pickedvac0 != 0))) {  // 0=odd&1=even
            move(jaga[pickedvac1], -45.0f);
          } else {
            move(jaga[pickedvac1], 45.0f);
          }
        } else if (pickedvac0 < pickedvac1) {  // destination 0<1
          if (((pickedvac0) % 2 == 0 && (pickedvac1) % 2 == 0) ||
              ((pickedvac0) % 2 != 0 && (pickedvac1) % 2 != 0)) {
            move(jaga[pickedvac1], 135.0f);
          } else {
            move(jaga[pickedvac1], -135.0f);
          }
        } else {
          continue;
        }
        //場所方向指定ここまで
        // move(sharejaga[gui.getCommand().destination1]);
        //目的地到着後（シュート）
        is_waiting_for_input = true;
        gamepad_input_to_command();
        is_waiting_for_input = false;
        take_down(z_height.z_down);
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(取るときはいるのか)
        is_waiting_for_input = false;
        catch_jaga();
        take_down(z_height.z_down_take);

        ThisThread::sleep_for(2000ms);
        take_up();

        break;

      case gui.CommandMode::commonArea:
        if (is_Red == false) {
          if (sharedir == 9) {
            move(sharejagaB[pickedvac0], 90.0f);
          } else if (sharedir == 10) {
            move(sharejagaB[pickedvac0], -90.0f);
          } else {
            move(sharejagaB[pickedvac0], 90.0f);
          }
        } else if (is_Red == true) {
          if (sharedir == 10) {
            move(sharejagaR[pickedvac0], 90.0f);
          } else if (sharedir == 9) {
            move(sharejagaR[pickedvac0], -90.0f);
          } else {
            move(sharejagaR[pickedvac0], 90.0f);
          }
        }
        //目的地到着後
        is_waiting_for_input = true;
        gamepad_input_to_command();  //上で微調整
        is_waiting_for_input = false;
        take_down(z_height.z_down_common);  //下す（まだ少し浮いてる）
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(取るときはいるのか)
        is_waiting_for_input = false;
        catch_jaga();
        take_down(z_height.z_down_common_take);  //吸う
        take_up();
        break;

      case gui.CommandMode::shootingBox:

        // ShootingBox BLUE までの移動とサーボの角度
        // when shoot in to the blue
        if (is_Red == false) {      // shooting box
          if ((pickedvac0 == 11 &&  // starting position and Degree set for
                                    // 4 center rows.
               pickedvac1 == 13) ||
              (pickedvac0 == 8 && pickedvac1 == 10) ||
              (pickedvac0 == 5 && pickedvac1 == 7) ||
              (pickedvac0 == 2 && pickedvac1 == 4) ||
              (pickedvac0 == 14 && pickedvac1 == 16) ||
              (pickedvac0 == 1 && pickedvac1 == 3)) {
            move(shoot[pickedvac1], 45.0f);
          } else if ((pickedvac0 == 6 && pickedvac1 == 4) ||
                     (pickedvac0 == 9 && pickedvac1 == 7) ||
                     (pickedvac0 == 12 && pickedvac1 == 10) ||
                     (pickedvac0 == 15 && pickedvac1 == 13) ||
                     (pickedvac0 == 3 && pickedvac1 == 1) ||
                     (pickedvac0 == 16 && pickedvac1 == 14)) {
            move(shoot[pickedvac1], -135.0f);
          } else if ((pickedvac0 == pickedvac1) &&
                     !(pickedvac0 == 4 || pickedvac0 == 7 || pickedvac0 == 10 ||
                       pickedvac0 == 13)) {
            //中心はシューティングボックスの外側の座標に合わせて移動（field_data.hに追加済み）
            switch (pickedvac0) {
                //選んだ場所によってサーボの角度調節（このスイッチ文大丈夫？）
              case 5:
              case 8:
              case 11:
              case 14:
              case 17:
                move(shootBwall[pickedvac1], -135.0f);
                break;
              case 0:
              case 1:
              case 2:
              case 3:
              case 6:
              case 9:
              case 12:
              case 15:
                move(shootBwall[pickedvac1], -45.0f);
                break;
              //以下例外、手動調整必要
              case 16:
                move(shootBwall[12], -45.0f);
                break;
              default:
                break;
            }
          } else {
            //何も動かさない、最初から選択
            continue;
          }
        }

        // ShootinBox REDまでの移動、サーボの角度
        if (is_Red == true) {
          if ((pickedvac0 == 1 &&  // starting position and Degree
                                   // set for 4 center rows.
               pickedvac1 == 5) ||
              (pickedvac0 == 0 && pickedvac1 == 4) ||
              (pickedvac0 == 3 && pickedvac1 == 7) ||
              (pickedvac0 == 6 && pickedvac1 == 10) ||
              (pickedvac0 == 9 && pickedvac1 == 13) ||
              (pickedvac0 == 12 && pickedvac1 == 16)) {
            move(shoot[pickedvac1], 135.0f);  // Position and Degree is ready
          } else if ((pickedvac0 == 5 &&      // starting position and Degree
                                              // set for 4 center rows.
                      pickedvac1 == 1) ||
                     (pickedvac0 == 8 && pickedvac1 == 4) ||
                     (pickedvac0 == 11 && pickedvac1 == 7) ||
                     (pickedvac0 == 14 && pickedvac1 == 10) ||
                     (pickedvac0 == 17 && pickedvac1 == 13) ||
                     (pickedvac0 == 16 && pickedvac1 == 12)) {
            move(shoot[pickedvac1], -45.0f);
          } else if ((pickedvac0 == pickedvac1) &&
                     !(pickedvac0 == 4 || pickedvac0 == 7 || pickedvac0 == 10 ||
                       pickedvac0 == 13)) {
            //中心はシューティングボックスの外側の座標に合わせて移動（field_data.hに追加済み）
            switch (pickedvac0) {
              case 2:
              case 5:
              case 8:
              case 11:
              case 14:
              case 16:
              case 17:
                move(shootRwall[pickedvac1], 135.0f);
                break;
              case 0:
              case 3:
              case 6:
              case 9:
                move(shootRwall[pickedvac1], 45.0f);
                break;
              //ここから例外、手動操作必要
              case 1:
                move(shoot[5], 135.0f);
                break;
              case 12:
              case 15:
                move(shootRwall[pickedvac1], 135.0f);
                break;
              default:
                break;
            }
          } else {
            //何も動かさない、最初から選択
            continue;
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

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
  stepper_r.set_theta_config(240.0f, 545.0f / 814.0f);
  stepper_theta.set_theta_config(0, 180.0f / 794.0f);
  const int stepper_vel_for_init = 10;
  const float motor_voltage_for_init = 0.15;
  const float revolution_num_rightside = 0;
  //右端についたときの回転数
  const int step_num_maxium = 500;
  // r最大値

  volatile bool theta_initialized = false;

  char serialBuf[64] = "";

  sensor.registerCallback(0, [&](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(0);
    sprintf(serialBuf, "x1 :left limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(1, [&](uint8_t, bool) {
    motor.reset();
    motor.resetPosition(revolution_num_rightside);
    sprintf(serialBuf, "x1 :right limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(2, [&](uint8_t, bool) {
    stepper_r.rotate_vel(0);
    stepper_r.reset(0);
    sprintf(serialBuf, "r :minimum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });

  sensor.registerCallback(3, [&](uint8_t, bool) {
    stepper_r.rotate_vel(0);
    stepper_r.reset(step_num_maxium);
    sprintf(serialBuf, "r :maximum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(4, [&](uint8_t, bool) {
    stepper_z.rotate_vel(0);
    stepper_z.reset(0);
    sprintf(serialBuf, "z :maximum limit detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });
  sensor.registerCallback(5, [&](uint8_t, bool) {
    stepper_theta.rotate_vel(0);
    stepper_theta.reset(0);
    theta_initialized = true;
    sprintf(serialBuf, "theta :zero point adjustment detected\n");
    pc.write(serialBuf, strlen(serialBuf));
  });

  servo.setPosition(0);  // reset servo
  if (team == Blue) {
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootBlue[i];
    }
    // sw0 に向かって押す　(blue)

    // theta=0が基準点
  } else {
    for (int i = 0; i < 18; i++) {
      shoot[i] = shootRed[i];
    }
    // sw1　にむかって押す(red)
    // theta=0 へ向かったあと、theta=180に向かう
  }
  ThisThread::sleep_for(10ms);
  if (!sensor.getState(5)) stepper_theta.rotate_vel(-stepper_vel_for_init);
  if (!sensor.getState(4)) stepper_z.rotate_vel(stepper_vel_for_init);
  if (!sensor.getState(3)) stepper_r.rotate_vel(stepper_vel_for_init);
  motor.reset();
  ThisThread::sleep_for(10ms);

  while (!((sensor.getState(0) || sensor.getState(1)) && sensor.getState(3) &&
           sensor.getState(4) && theta_initialized)) {
    if (team == Blue) {
      if (!sensor.getState(1)) motor.driveVoltage(-motor_voltage_for_init);
    } else {
      if (!sensor.getState(1)) motor.driveVoltage(-motor_voltage_for_init);
    }
    ThisThread::sleep_for(1ms);
  }
  if (team == Red) {
    stepper_theta.rotate(-180);
    while (stepper_theta.progress_cnt() < 0.99) {
    }
    stepper_theta.reset(180);
  }
}

void ini() {
  manager.begin();
  pcConnector.registerCallback(0x01, callback(&gamepad, &Gamepad::pcCallback));
  pcConnector.registerCallback(0x02, callback(&gui, &Gui::pcVectorCallback));
  pcConnector.registerCallback(0x03, callback(&gui, &Gui::pcSuckerCallback));
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
  ini();
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
            move(jaga[pickedvac1], 315.0f);
          } else {
            move(jaga[pickedvac1], 45.0f);
          }
        } else {  // destination 0<1
          if (((pickedvac0) % 2 == 0 && (pickedvac1) % 2 == 0) ||
              ((pickedvac0) % 2 != 0 && (pickedvac1) % 2 != 0)) {
            move(jaga[pickedvac1], 135.0f);
          } else {
            move(jaga[pickedvac1], 225.0f);
          }
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
        take_down(z_height.z_down_take);
        catch_jaga();
        ThisThread::sleep_for(2000ms);
        take_up();

        break;

      case gui.CommandMode::commonArea:
        if (is_Red == false) {
          if (sharedir == 9) {
            move(sharejagaB[pickedvac0], 90.0f);
          } else if (pickedvac1 == 10) {
            move(sharejagaB[pickedvac0], 270.0f);
          } else {
            move(sharejagaB[pickedvac0], 90.0f);
          }
        } else if (is_Red == true) {
          if (sharedir == 10) {
            move(sharejagaR[pickedvac0], 90.0f);
          } else if (sharedir == 9) {
            move(sharejagaR[pickedvac0], 270.0f);
          } else {
            move(sharejagaR[pickedvac0], 90.0f);
          }
        }
        //目的地到着後（シュート）
        is_waiting_for_input = true;
        gamepad_input_to_command();  //上で微調整
        is_waiting_for_input = false;
        take_down(z_height.z_down_common);  //下す（まだ少し浮いてる）
        is_waiting_for_input = true;
        gamepad_input_to_command();  //下した後の微調節、いるか要検討(取るときはいるのか)
        is_waiting_for_input = false;
        take_down(z_height.z_down_common_take);  //吸う
        catch_jaga();
        ThisThread::sleep_for(2000ms);
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
          } else if ((pickedvac0 == pickedvac1) &&
                     !(pickedvac0 == 4 || pickedvac0 == 7 || pickedvac0 == 10 ||
                       pickedvac0 == 13)) {
            //中心はシューティングボックスの外側の座標に合わせて移動（field_data.hに追加済み）
            switch (pickedvac0) {
                //選んだ場所によってサーボの角度調節（このスイッチ文大丈夫？）
              case 0:
              case 1:
              case 2:
              case 3:
              case 6:
              case 9:
              case 12:
              case 15:
                move(shootBwall[pickedvac1], 45.0f);
                break;
              case 16:
              case 17:
                move(shootBwall[pickedvac1], 45.0f);
                break;
              case 5:
              case 8:
              case 11:
              case 14:
                move(shootBwall[pickedvac1], 135.0f);
                break;
              default:
                break;
            }
          } else {
            //何も動かさない、最初から選択
          }
        }

        // ShootinBox REDまでの移動、サーボの角度
        if (is_Red == true) {
          if ((pickedvac0 == 3 &&  // starting position and Degree
                                   // set for 4 center rows.
               pickedvac1 == 1) ||
              (pickedvac0 == 6 && pickedvac1 == 4) ||
              (pickedvac0 == 9 && pickedvac1 == 7) ||
              (pickedvac0 == 12 && pickedvac1 == 10) ||
              (pickedvac0 == 15 && pickedvac1 == 13) ||
              (pickedvac0 == 16 && pickedvac1 == 14)) {
            move(shoot[pickedvac1], 225.0f);  // Position and Degree is ready
          } else if ((pickedvac0 == pickedvac1) &&
                     !(pickedvac0 == 4 || pickedvac0 == 7 || pickedvac0 == 10 ||
                       pickedvac0 == 13)) {
            //中心はシューティングボックスの外側の座標に合わせて移動（field_data.hに追加済み）
            switch (pickedvac0) {
              case 0:
              case 1:
              case 2:
              case 5:
              case 8:
              case 11:
              case 14:
              case 17:
                move(shootRwall[pickedvac1], 225.0f);
                break;
              case 15:
              case 16:
                move(shootRwall[pickedvac1], 135.0f);
                break;
              case 3:
              case 6:
              case 9:
              case 12:
                move(shootRwall[pickedvac1], 315.0f);
                break;
              default:
                break;
            }
          } else {
            //何も動かさない、最初から選択
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
        ThisThread::sleep_for(2000ms);
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

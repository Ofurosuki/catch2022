#include <mbed.h>
#include "math.h"
#include "config.h"
#include "stepper.h"

Stepper::Stepper(PinName dir,PinName stp): _dir(dir), _stp(stp){
    _dir=0; //0で正転
    _stp=0;
}

int Stepper::get_stp_counter(){
  return stp_counter;
}
int Stepper::get_cnt(){
  return cnt;
}
int Stepper::get_stepcycle(){
  return stepcycle;
}
int Stepper::get_freq(){
  return freq;
}

bool Stepper::is_cw(){
  if(target_step>=0){
    return true;
  }else{
    return false;
  }
}
void Stepper::step(int freq_ini,int target_stp){
    target_step=target_stp;
    if(is_cw()){
      _dir=0;
      target_step=target_stp;
    }else{
      _dir=1;
      target_step=-target_stp;
    }
    stp_counter=0; //reset local step counter
    freq_top=sqrt(freq_ini*freq_ini+freq_diff*target_step);
    is_first=true;
    if(freq_max>freq_top){
      can_velocity_triangle=true;
    }else{
      can_velocity_triangle=false;
    }
    stepcycle=1/(float)(freq_ini)*10e5;
    freq=freq_ini;
    asm("nop");
    
}

void Stepper::step_ctl(){ //割り込み関数  //call_interval micro second ごとに（関数の呼び出しごとに）周期を変更  
  if(stepcycle!=0){           //stepcycle==0で割り込まれても何もしない
        //freq+=(float)(freq_diff)*call_interval*10e-6;  

        // frequency(velocity)-time グラフの形を決定
        if(can_velocity_triangle){
          if(target_step/2>stp_counter){
            freq+=(float)(freq_diff)*10e-5;
          }else{
            freq-=(float)(freq_diff)*10e-5;
          }
        }else{ 
          if((target_step-triangle_area)<stp_counter){
            freq-=(float)(freq_diff)*10e-5;
          }else if(freq<=freq_max){
            freq+=(float)(freq_diff)*10e-5;
          }else if(freq>=freq_max&&is_first){
            triangle_area=stp_counter;
            is_first=false;
          }
        }
             
         stepcycle=1.0f/freq*10e5;  
         if(cnt>=stepcycle){   //閾値を超えると
            cnt=0;             //カウンタをリセット
            stp_counter++;   // ステップを１つしたカウント
            if(is_cw()){          //正転の場合グローバルカウンタを1足す、反転の場合1引く
              global_stp_counter++;
            }else{
              global_stp_counter--;
            }
        }else{
            cnt+=call_interval;       //100usごとに呼び出し、cntは時間と同じスケールで加算される
        }
        if(target_step<=stp_counter){
          stepcycle=0;
          stp_counter=0;
          freq=0;
        }
        _stp=(stepcycle!=0&&cnt<=H_time_length);
  }
}

void Stepper::set_config(int freq_difference,int freq_maxium){
  freq_diff=freq_difference;
  freq_max=freq_maxium;
}
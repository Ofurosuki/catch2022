#include <config.h>
#include <stepper.h>
#include <stepper_rotate.h>


asm(".global _printf_float");  // float出力用

UnbufferedSerial pc(USBTX, USBRX);

AnalogIn x(PA_0);
rotate_stepper stepper(PB_1, LED1);

int main() {

  stepper.set_config(2, 10, 2);
  printf("ini\n");
  // stepper.step(10, 50);
  while (1) {
    float vol = x.read();
    stepper.rotate_vel((int)(vol * 100) - 50);
    
    ThisThread::sleep_for(50ms);
    printf("input:%d,", (int)(vol * 100) - 50);
    printf("freq:%d,", stepper.get_freq());
    printf("is_plus:%d\n",stepper.is_plus);
    // printf("progress:%f\n",stepper.progress_cnt());
  }
}

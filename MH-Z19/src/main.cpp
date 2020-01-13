#include <mbed.h>

DigitalIn co2(D13);
Serial send(PB_6, PB_7, 115200);

volatile unsigned long  _millis;

void millisStart(void) {
    SysTick_Config(SystemCoreClock / 1000);
}
 
extern "C" void SysTick_Handler(void) {
    _millis++;
}
 
unsigned long millis(void) {
    return _millis;
}
 
long th, tl, h, l;
uint16_t ppm = 1000;
int main() {
uint8_t dataByte[2];
uint8_t check_sum{};
int val_now, val_prev;
long tt;
  millisStart();
  while(1){
    tt = millis();
    val_now = co2.read();
    if(val_now){
      if(val_now != val_prev){
        h = tt;
        tl = h - l;
        val_prev = val_now;
      }
    }else{
      if(val_now != val_prev){
        l = tt;
        th = l - h;
        val_prev = val_now;
        ppm = 2000 * (th - 2e-3) / ((th + tl) - 4e-3);
      }
    }

    dataByte[0] = (ppm >> 8) & 0xFF;
    dataByte[1] = (ppm >> 0) & 0xFF;
    //send.putc(HEAD_BYTE);
    send.putc(0x02);
  
    /*for(uint8_t i = 0; i < 2; ++i){
      if((dataByte[i] == ESCAPE_BYTE) || (dataByte[i] == HEAD_BYTE)){
         send.putc(ESCAPE_BYTE);
         send.putc(dataByte[i] ^ ESCAPE_BYTE);
       }else{
        send.putc(dataByte[i]);
      }
    }*/
    for(int i = 0; i < 2; ++i){
      if((dataByte[i] == 0x7D) || (dataByte[i] == 0x02)){
        send.putc(0x7D);
        check_sum += 0x7D;
        send.putc(dataByte[i] ^ 0x20);
        check_sum += dataByte[i] ^ 0x20;
      }else{
        send.putc(dataByte[i]);
        check_sum += dataByte[i];
      }
    }
    send.putc(check_sum);
  }
}
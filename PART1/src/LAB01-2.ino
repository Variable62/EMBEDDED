#include <avr/io.h>
#define F_CPU 16000000UL
int main(void){
  DDRD = DDRD | (1<<PD4);//LED RED O/P
  DDRD = DDRD | (1<<PD5);//LED YELLOW O/P
  DDRD = DDRD | (1<<PD6);//LED GREEN O/P
  DDRD &= ~(1<<PD3);//SW1 0
  while(true){
    PORTD = PORTD | (1<<PD6);//LED GREEN ON

    if(!(PIND & (1<<PD3))){
      PORTD = PORTD & ~(1<<PD6);
      PORTD = PORTD | (1<<PD5);
       __builtin_avr_delay_cycles(48000000);//3s
      PORTD = PORTD & ~(1<<PD5);
      PORTD = PORTD | (1<<PD4);
       __builtin_avr_delay_cycles(160000000);//10s
      PORTD = PORTD & ~(1<<PD4); 
    }
  }
}
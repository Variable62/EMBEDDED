#include <avr/io.h>
int main(){
  DDRD = DDRD | (1<<PD4);//LED RED O/P
  DDRD = DDRD | (1<<PD5);//LED YELLOW O/P
  DDRD = DDRD | (1<<PD6);//LED GREEN O/P
  DDRD &= ~(1<<PD3);//SW1 0
  while(1){
    PORTD = PORTD | (1<<PD6);//LED GREEN ON

    if(!(PIND & (1<<PD3))){
      PORTD = PORTD & ~(1<<PD6);
      PORTD = PORTD | (1<<PD5);
       __builtin_avr_delay_cycles(47000000);//3s
      PORTD = PORTD & ~(1<<PD5);
      PORTD = PORTD | (1<<PD4);
       __builtin_avr_delay_cycles(155000000);//10s
      PORTD = PORTD & ~(1<<PD4); 
    }
  }
}
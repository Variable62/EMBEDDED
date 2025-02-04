#define INT0_vect _VECTOR(1)        /* External Interrupt Request 0 */
#define TIMER1_OVF_vect _VECTOR(13) /* Timer 1 Overflow */
#define F_CPU 16000000UL
#include  <avr/io.h>
#include  <util/delay.h>
#define Sw PD2
#define Led_Red PB0
#define Led_Yellow PB1
#define Led_Green PB2

void clear_led(){
   PORTB &= ~((1 << Led_Red) | (1<<Led_Yellow) | (1<<Led_Green));
}

ISR(INT0_vect) {
	 clear_led();
	 PORTB  |= (1<<Led_Green);
	 TCNT1  = 3036;// 65536-62500;
}

ISR(TIMER1_OVF_vect) {
   if (PORTB & (1 << Led_Green))
  {
    clear_led();
    PORTB |= (1<<Led_Yellow);
    TCNT1 = 49911;
  }
  else if(PORTB & (1<<Led_Yellow)){
    clear_led();
    PORTB |= (1<<Led_Red);
    TIMSK1 = 1; //disable timer interrupt 
}}

int main() 
{ 
  DDRD &= ~(1<<Sw);// Sw input
  DDRB  |= ((1 << Led_Red) | (1<<Led_Yellow) | (1<<Led_Green));// led out put
	 
  EICRA = EICRA | 0b00000010; // INT0 Falling Edge
  EIMSK = 1;                  // Enable INT0  
  
  TCCR1A = 0x00; // Nomal mode
  TCCR1B = 0x05; // 1024 Prescale 
 
  TCNT1  = 3036;       // 65536-62500;
  TIMSK1 = 0b00000001;// Overflow Interrupt
  TIFR1  = 0b00000001;; // Clear Overflow Status
  PORTB  |= (1<<Led_Red);//l  led red on
  // Turn on global interrupts 
  sei();
  while(true){
  }
}
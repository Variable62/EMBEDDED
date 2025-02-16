#include <avr/io.h>
#include <util/delay.h>

#define INT0_vect _VECTOR(1) //D2 Switch 1
#define INT1_vect _VECTOR(2) //D3 Switch 2
#define TIMER1_OVF_vect _VECTOR(13) /* Timer 1 Overflow */
#define F_CPU 16000000UL // assign frequency clock 

#define Led_Red PD4 //D4
#define Led_Yellow PD5 //D5
#define Led_Green PD6 //D6

bool dir = 0;//initial = 0
uint8_t speed = 0;
uint8_t color = 0;
uint16_t speed_use = 200;
char speed_text[30];
 
void clear_led(){
  PORTD &= ~((1 << Led_Red) | (1 << Led_Yellow) | (1 << Led_Green)); // LED_All = Low
}

void Serial_begin(){
  UBRR0 = 51; //19200
  UCSR0A = 0b00000000;
  UCSR0B = 0b10011000;
  UCSR0C = 0b00000110;
}

void Serial_putc(char data){
  char busy;
  do { 
    busy = UCSR0A & 0b00100000;
  } while (busy == 0);
    UDR0 = data;
}
char Serial_getc(){
  char busy;
  do{
    busy = UCSR0A &0b10000000;
  }while (busy == 0);
  return (UDR0);
}
void Serial_puts(char *data){
  while (*data){
    Serial_putc(*data++);
  }
}
void Serial_display(){
  switch(dir)
  {
  case 0 :
    Serial_puts("Red > Yellow > Green : ");
    break;
  case 1 :
    Serial_puts("Green > Yellow > Red : ");
    break;
  default :
    break;
  }
  sprintf(speed_text, "%d", speed_use);
  strcat(speed_text, "ms");
  strcat(speed_text, "\r\n");
  Serial_puts(speed_text);
}

ISR(INT0_vect) //
{
  dir = dir ^= 1;// XOR
  Serial_display();
}

ISR(INT1_vect)
{
  if(speed < 4){
    speed++;
  }
  else
    speed = 0;
  speed_use = (speed + 1) * 200; //*200ms return for speed_use
  Serial_display();
}

ISR(TIMER1_OVF_vect) {
  float time_sec = (F_CPU / 1024.0) * (speed_use / 1000.0);
  TCNT1 = 65535 - time_sec;
  clear_led();
  switch (dir)
  {
  case 0: // Red -> Yellow -> Green
    switch (color)
    {
    case 0:
      PORTD |= (1 << Led_Red);
      color += 1;
      break;
    case 1:
      PORTD |= (1 << Led_Yellow);
      color += 1;
      break;
    case 2:
      PORTD |= (1 << Led_Green);
      color = 0;
      break;
    }
    break;
  case 1: // Green -> Yellow -> Red
    switch (color)
    {
    case 0:
      PORTD |= (1 << Led_Green);
      color += 1;
      break;
    case 1:
      PORTD |= (1 << Led_Yellow);
      color += 1;
      break;
    case 2:
      PORTD |= (1 << Led_Red);
      color = 0;
      break;
    }
    break;
  }
  
}
int main() {
  Serial_begin();//9600
  DDRD |= ((1 << Led_Red) | (1 << Led_Yellow) | (1 << Led_Green));  // led out put

  EICRA = EICRA | 0b00001010;  //Falling Edge
  EIMSK |= (1 << INT0 | 1 << INT1);// Enable INT0 INT1

  TCCR1A = 0b00000000;  // Nomal mode
  TCCR1B = 0b00000101;  // 1024 Prescale
  TIMSK1 = 0b00000001;
  TCNT1 = 62410;//200ms
  sei();
  while(true){

  }
}
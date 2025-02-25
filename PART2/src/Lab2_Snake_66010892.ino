#include <avr/io.h>

#define INT0_vect _VECTOR(1)  //Sw1
#define INT1_vect _VECTOR(2)  //Sw2
#define TIMER1_OVF_vect _VECTOR(13)

int x[4] = { 4, 5, 6, 7 }, y[4] = { 7, 7, 7, 7 };
//fixed axis initial
int horizontal = 1, vertical = 0;  // ---->

void setting_interrupt() {
  EICRA = EICRA | 0b00001010;          // failling edge
  EIMSK |= (1 << INT1) | (1 << INT0);  // enable INT1 , INT0
  TCCR1A = 0b00000000;
  TCCR1B = 0b00000101;  // prescale 1024
  TIMSK1 = 0b00000001;
  TCNT1 = 57723;  // 500 ms
}

void spi_init(void) {
  DDRB = 0b00101110;  // Set MOSI, SCK, Reset output
  SPSR = 0b00000001;  // Set prescaler
  SPCR = 0b01010000;  // Enable SPI, Set as Mase,u Clock.
}
void spi_putc(char data) {
  char busy;
  // Load data into the buffer
  SPDR = data;
  //Wait until transmission complete
  do { busy = SPSR & 0b10000000; } while (busy == 0);
}
void max7219_wr(char addr, char data) {
  PORTB &= 0b111111011;  // SS = 0
  spi_putc(addr);
  spi_putc(data);
  PORTB |= 0b000000100;  // SS = 1
}

void max7219_config() {
  max7219_wr(0x0C, 0x00);  // Shutdown
  max7219_wr(0x09, 0x00);  // No Decode mode
  max7219_wr(0x0A, 0x15);  // Intensity
  max7219_wr(0x0B, 0x07);  // Scan limit
  max7219_wr(0x0C, 0x01);  // Turn on
}
void display() {
  byte set_display[8] = { 0 };
  for (int i = 0; i < 4; i++) {
    set_display[y[i]] |= (1 << x[i]);
  }
  for (int row = 0; row < 8; row++) {
    max7219_wr(row + 1, set_display[row]);
  }
}
void move() {
  display();
  // update x,y 4 dot
  for (int position = 3; position > 0; position--) {
    x[position] = x[position - 1];
    y[position] = y[position - 1];
  }
  x[0] -= horizontal;
  y[0] -= vertical;
  if (x[0] < 0)  // horizontal
  {
    x[0] = 7;  // reset
  }
  if (y[0] < 0)  // vertical
  {
    y[0] = 7;  // reset3
  }
}

ISR(INT0_vect) {
  // shift right (default)
  horizontal = 1, vertical = 0;
}

ISR(INT1_vect) {
  // shift up
  horizontal = 0, vertical = 1;
}

ISR(TIMER1_OVF_vect) {
  move();
  TCNT1 = 57723;  // 500 ms
}
void Clear_display() {
  for (int i = 1; i <= 8; i++) {
    max7219_wr(i, 0x00);
  }
}
int main() {
  setting_interrupt();
  spi_init();
  max7219_config();
  Clear_display();
  sei();
  while (1) {
  }
}
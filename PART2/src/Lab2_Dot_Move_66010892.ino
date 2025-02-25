#include <avr/io.h>
#include <util/delay.h>

#define INT0_vect _VECTOR(1)  // SW 1
#define INT1_vect _VECTOR(2)  // SW 2

uint8_t x = 0, y = 0, row = 0;
char clear = 0x00;
void setting_interrupt() {
  EICRA |= (1 << ISC01) | (1 << ISC11);  // Falling edge trigger INT0, INT1
  EIMSK |= (1 << INT0) | (1 << INT1);    // Enable INT0, INT1
}

void spi_init() {
  DDRB = 0b00101110;  // Set MOSI, SCK, Reset output
  SPSR = 0b00000001;  // Set prescaler
  SPCR = 0b01010000;  // Enable SPI, Set as Master, Clock.
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
  max7219_wr(0x0C, 0x00);  // Shutdown mode
  max7219_wr(0x09, 0x00);  // Decode mode
  max7219_wr(0x0A, 0x0F);  // Intensity max
  max7219_wr(0x0B, 0x07);  // Scan limit
  max7219_wr(0x0C, 0x01);  // Turn on
}
void Dot_move() {
  for (row = 0; row < 8; row++) {
    max7219_wr(row + 1, clear);
  }
  max7219_wr(y + 1, (1 << x));
}
ISR(INT0_vect) {
  x += 1;
  if (x > 7) {
    x = 0;  // reset
      _delay_ms(200);
  }
  Dot_move();
}
ISR(INT1_vect) {
  y += 1;
  if (y > 7) {
    y = 0;  //reset
    _delay_ms(200);
  }
  Dot_move();
}

int main() {
  setting_interrupt();
  spi_init();
  max7219_config();
  sei();
  Dot_move();
  while (1) {
  }
}
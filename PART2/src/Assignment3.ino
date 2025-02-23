#include <avr/io.h>
#include <util/delay.h>

const uint8_t NUM_6[8] = {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00};
const uint8_t NUM_0[8] = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00};
const uint8_t NUM_1[8] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00};
const uint8_t NUM_8[8] = {0x3C, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00};
const uint8_t NUM_9[8] = {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00};
const uint8_t NUM_2[8] = {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00};

const uint8_t A[8] = {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00};
const uint8_t D[8] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00};
const uint8_t I[8] = {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00};
const uint8_t S[8] = {0x3C, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x3C, 0x00};
const uint8_t O[8] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00};
const uint8_t R[8] = {0x78, 0x6C, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00};
const uint8_t N[8] = {0x83, 0xE3, 0xF3, 0xFF, 0xFF, 0xCF, 0xC7, 0xC1};

void spi_init(void) {
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

void max7219_Config() {
  max7219_wr(0x0C, 0);     // Shutdown
  max7219_wr(0x09, 0x00);  // NO decode 
  max7219_wr(0x0A, 0x00);  // light intensity
  max7219_wr(0x0B, 0x07);  // scan limit
  max7219_wr(0x0C, 1);     // turn ON
}

void displayChar(const uint8_t character[8]) {
  for (char i = 0; i < 8; i++) {
    max7219_wr(i + 1, character[i]);  
  }
}

void display_messsage() {
  const uint8_t* message[] = {NUM_6, NUM_6, NUM_0, NUM_1, NUM_0, NUM_8, NUM_9, NUM_2, A, D, I, S, O, R, N};
  const int length = sizeof(message) / sizeof(message[0]);
  
  for (int i = 0; i < length; i++) {
    displayChar(message[i]);
    _delay_ms(800);  
    Clear_display();
    _delay_ms(200);  
  }
}

void Clear_display() {
  for (char i = 1; i <= 8; i++) {
    max7219_wr(i, 0x00);
  }
}

int main() {
  spi_init();
  max7219_Config();
  Clear_display();
  while (1){
    display_messsage();
  }
}

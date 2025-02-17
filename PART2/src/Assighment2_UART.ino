#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define UART_RX_vect _VECTOR(18)
char choose;  // default Celsius
int Temp;
float cel, fah;
char choice, text[5];
void ADC_init() {
  ADMUX = 0b01000000;   //0 1 : AVcc
  ADCSRA = 0b10000111;  // ADC : Enable , Prescale : 128
}

void serial_begin() {
  UBRR0 = 25;  // Baudrate 38400 -> UBRR = 16000000 / (16 * 38400) - 1 ? 25
  UCSR0A = 0b00000000;
  UCSR0B = 0b10011000;
  UCSR0C = 0b00000110;
}
char serial_getc() {
  char busy;
  do { busy = UCSR0A & 0b10000000; } while (busy == 0);
  return (UDR0);
}

void serial_putc(char data) {
  char busy;
  do {
    // Check data register empty?
    busy = UCSR0A & 0b00100000;
  } while (busy == 0);
  UDR0 = data;
}

void serial_puts(char* data) {
  while (*data) {
    serial_putc(*data++);
  }
}

int ADC_read(char ch) {
  char busy;
  ADMUX = ADMUX & 0b11110000;
  ADMUX = ADMUX | ch;
  ADCSRA = ADCSRA | 0b01000000;
  do {
    busy = ADCSRA & 0b01000000;
  } while (busy != 0);
  return (ADC);
}

float convert_fah_to_cel() {
  fah = (((9 * cel) / 5) + 32);
  return (fah);
}

void Condition() {
  int choice_to_int = choice - '0';
  switch (choice_to_int) {
    case 1:
      dtostrf(cel, 8, 2, text);
      serial_puts("Temperature is ");
      serial_puts(text);
      serial_puts(" C\r\n");
      break;
    case 2:
      fah = convert_fah_to_cel();
      dtostrf(fah, 8, 2, text);
      serial_puts("Temperature is ");
      serial_puts(text);
      serial_puts(" F\r\n");
      break;
    default:
      serial_puts("Please type only 1 or 2\r\n");
      break;
  }
  serial_puts("Read Temperature in Celsius (1) or Fahrenheit (2): ");
}
ISR(UART_RX_vect) {
  Temp = ADC_read(3);  // ADC 3
  cel = ((float)Temp * 5.0 / 1024.0) * 100.0;
  choice = serial_getc();
  serial_putc(choice);
  serial_puts("\r\n");
  Condition();
}
int main(void) {
  serial_begin();
  ADC_init();
  sei();  // Enable global interrupts
  serial_puts("Read Temperature in Celsius (1) or Fahrenheit (2) : ");
  while (1) {
  }
}

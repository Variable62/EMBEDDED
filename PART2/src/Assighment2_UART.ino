#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define INT1_vect _VECTOR(2)
#define UART_RX_vect _VECTOR(18)

float Temp = 0.0;
char choose = "1";  // default Celsius
char ans = 0;
char received_char = 0;
uint8_t data_received = 0;

void serial_begin() {
  UBRR0 = 25;  // Baudrate 38400 -> UBRR = 16000000 / (16 * 38400) - 1 ≈ 25
  UCSR0A = 0b00000000;
  UCSR0B = 0b00011000;
  UCSR0C = 0b00000110;
}

void serial_putc(char data) {
  char busy;
  do { busy = UCSR0A & 0b00100000; } while (busy == 0);
  UDR0 = data;
}

char serial_getc() {
  char busy;
  do { busy = UCSR0A & 0b10000000; } while (busy == 0);
  return (UDR0);
}

void serial_puts(char *data) {
  while (*data) {
    serial_putc(*data++);
  }
}

uint16_t Adc_read(char ch) {
  ADMUX = (0b01000000) | (ch & 0b00001111);
  ADCSRA = 0b10000110;    //
  ADCSRA |= (1 << ADSC);  // initial
  while (ADCSRA & (1 << ADSC));
  return ADC;
}

float Get_Temperature() {
  uint16_t adc_value = Adc_read(0);  //ADC 0
  float voltage = adc_value * (5.0 / 1024.0);
  float temperature = voltage * 100.0;

  if (choose == '2') {
    temperature = (temperature * 9.0 / 5.0) + 32.0;  //Convert Fahrenheit
  }
  return temperature;
}
void Set_Interrupts() {
  EICRA = EICRA | 0b00001010;  //Falling Edge
  EIMSK |= (1 << INT0);        // Enable INT0 INT1
}

void Set_Timer() {
  TCCR1A = 0b00000000;  // Nomal mode
  TCCR1B = 0b00000101;  // 1024 Prescale
  TIMSK1 = 0b00000001;
}

ISR(INT0_vect) {
  char temp_str[10];
  Temp = Get_Temperature();
  serial_puts("Temperature is ");
  dtostrf(Temp, 5, 2, temp_str);
  serial_puts(temp_str);

  if (choose == '1') {
    serial_puts(" C\r\n");
  } else {
    serial_puts(" F\r\n");
  }
}
ISR(UART_RX_vect) {
  received_char = UDR0;  // อ่านค่าที่ได้รับ
  data_received = 1;     // ตั้งค่า flag ว่ามีข้อมูลเข้า
}
int main(void) {
  serial_begin();
  DDRB = 0b00000001;  // set PB0 output
  Set_Interrupts();
  Set_Timer();
  sei();  // Enable global interrupts
  serial_puts("Read Temperature in Celsius (1) or Fahrenheit (2) : ");

  while (1) {
    if (data_received) {
      serial_putc(received_char);
      serial_puts("\r\n");

      if (received_char == '1' || received_char == '2') {
        choose = received_char;
      } else {
        serial_puts("Please try again\r\n");
      }
      serial_puts("Read Temperature in Celsius (1) or Fahrenheit (2) : ");
    }
  }
}
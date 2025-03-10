#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define UART_RX_vect _VECTOR(18)
#define UART_TX_vect _VECTOR(17)

float temperature = 0;
uint8_t D1, D2;

// ---------------------------------- UART ----------------------------------------------

void serial_begin()
{
  UBRR0 = 52; // 19200 baud rate
  UCSR0A = 0b00000000;
  UCSR0B = 0b10011000;
  UCSR0C = 0b00000110;
}

void serial_putc(char data)
{
  char busy;
  do
  {
    busy = UCSR0A & 0b00100000;
  } while (busy == 0);
  UDR0 = data;
}

void serial_puts(char *data)
{
  while (*data)
  {
    serial_putc(*data++);
  }
}

// ----------------------------- I2C -------------------------------------

void i2c_init()
{
  TWSR = 0b00000000; // set prescaler bits to zero
  TWBR = 32;         // SCL frequency is 200K for 16Mhz
  TWCR = 0b00000100; // enable TWI module
  // Reset
  PORTB = PORTB & 0b11111101;
  _delay_ms(100);
  PORTB = PORTB | 0b00000010;
}

void i2c_start()
{
  char busy;
  TWCR = 0b10100100; // TWINT,TWSTA,TWEN;
  do
  {
    busy = TWCR & 0b10000000;
  } while (busy == 0);
}

void i2c_stop(void)
{
  TWCR = 0b10010100; // TWINT,TWSTO,TWEN;
  _delay_ms(1);      // wait for a short time
}

void i2c_putc(char data)
{
  char busy;
  TWDR = data;
  TWCR = 0b10000100; // TWINT,TWEN;
  do
  {
    busy = TWCR & 0b10000000;
  } while (busy == 0);
}

char i2c_getc(char ack)
{
  char busy;
  if (ack == 1)
    TWCR = 0b11000100; // TWINT,TWEN,TWEA=1
  else
    TWCR = 0b10000100; // TWINT,TWEN,TWEA=0
  do
  {
    busy = TWCR & 0b10000000;
  } while (busy == 0);
  return TWDR;
}

// ----------------------------- MCP9803 -------------------------------------

void mcp9803_wr(char addr, char data)
{
  i2c_start();
  i2c_putc(0b10010000); // Device Address (Write mode)
  i2c_putc(addr);
  i2c_putc(data);
  i2c_stop();
}

float mcp9803_rd()
{
  i2c_start();
  i2c_putc(0b10010000); // Device Address (Write mode)
  i2c_putc(0x00);       
  i2c_stop();

  i2c_start();
  i2c_putc(0b10010001); // Device Address (Read mode)
  D1 = i2c_getc(1);     
  D2 = i2c_getc(0);    
  i2c_stop();

  return (float)D1 + (D2 / 256.0); 
}

// ----------------------------- MAIN -------------------------------------

int main()
{
  char buffer[10];
  i2c_init();
  serial_begin();
  mcp9803_wr(0x01, 0b01100000); // Configuration register, 12-bit resolution
  while (1)
  {
    temperature = mcp9803_rd();
    dtostrf(temperature, 5, 3, buffer);
    serial_puts("Temp: ");
    serial_puts(buffer);
    serial_puts(" C\r\n");
    _delay_ms(1000);
  }
}

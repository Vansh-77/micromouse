#include "main.h"
#include "uart.h"
#include <stdio.h>
#include <stdarg.h>

void uart_write_char(char c){

	while( !(USART2->SR & (1U << 7)) );

	USART2->DR = c;

}

void uart_write(const char *str){
	while(*str){
		uart_write_char(*str);
		str++;
	}
}

void uart_write_uint(uint32_t i)
{
    char buffer[10];
    int j = 0;

    if (i == 0)
    {
        uart_write_char('0');
        return;
    }

    while (i > 0)
    {
        buffer[j++] = '0' + (i % 10);
        i /= 10;
    }

    while (j > 0)
    {
        uart_write_char(buffer[--j]);
    }
}
void uart_write_int(int32_t value)
{
    if (value < 0)
    {
        uart_write_char('-');
        value = -value;
    }

    uart_write_uint((uint32_t)value);
}


void uart_printf(const char *format, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    uart_write(buffer);
}




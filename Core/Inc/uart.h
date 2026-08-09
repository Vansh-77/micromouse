#ifndef UART_H
#define UART_H

#include<stdint.h>

void uart_write_char(char c);
void uart_write(const char *str);
void uart_write_uint(uint32_t i);
void uart_write_int(int32_t value);
void uart_printf(const char *format, ...);


#endif

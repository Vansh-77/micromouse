#include "main.h"
#include "bt.h"
#include <stdio.h>
#include <stdarg.h>


void bt_write_char(char c){

	while( !(USART1->SR & (1U << 7)) );

	USART1->DR = c;

}

void bt_write(const char *str){
	while(*str){
		bt_write_char(*str);
		str++;
	}
}


void bt_printf(const char *format, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    bt_write(buffer);
}

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

uint8_t bt_read_char(char *c){

	if (!(USART1->SR & (1U << 5))) return 0;
	else {
		*c = (char)USART1->DR;
		return 1;
	}
}

uint8_t bt_read(char *buffer, uint8_t max_len){
	static uint8_t index = 0;
	char c;

	while (bt_read_char(&c))
	{
	if (c == '\n')
	{
	   buffer[index] = '\0';
	   index = 0;
	   return 1;
	}
	if (c == '\r') continue;

	if (index < max_len - 1){
	            buffer[index++] = c;
	 }
	 else{
	  // Buffer overflow
		 index = 0;
	  }
	}
	return 0;
}









#include "main.h"
#include "bt.h"
#include <stdio.h>
#include <stdarg.h>


#define BT_RX_BUFFER_SIZE 64

static volatile char rx_buffer[BT_RX_BUFFER_SIZE];
static volatile uint8_t rx_index = 0;
static volatile uint8_t message_ready = 0;


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



void bt_init(void)
{
    /*
     * Enable USART1 RXNE interrupt
     */
    USART1->CR1 |= USART_CR1_RXNEIE;

    /*
     * Enable USART1 interrupt in NVIC
     */
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        char c = (char)USART1->DR;
        /*
         * End of message
         */
        if (c == '\n')
        {
            rx_buffer[rx_index] = '\0';
            message_ready = 1;
            rx_index = 0;
        }
        /*
         * Ignore carriage return
         */
        else if (c == '\r')
        {
        	// Ignore
        	}
                /*
                 * Store character
                 */
         else{
        	 if (rx_index < BT_RX_BUFFER_SIZE - 1)
        	 {
        		 rx_buffer[rx_index++] = c;
              }
              else
              {
               /*
               * Buffer overflow.
               * Reset the current message.
               */
               rx_index = 0;
              }
          }
    }
}

uint8_t bt_message_available(void)
{
    return message_ready;
}

void bt_get_message(char *buffer)
{
    uint8_t i;

    /*
     * Copy message from ISR buffer
     * into application buffer.
     */
    for (i = 0; i < BT_RX_BUFFER_SIZE; i++)
    {
        buffer[i] = rx_buffer[i];

        if (rx_buffer[i] == '\0')
            break;
    }

    /*
     * Message has been consumed.
     */
    message_ready = 0;
}


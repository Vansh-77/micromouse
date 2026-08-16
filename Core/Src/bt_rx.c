#include "main.h"
#include "bt_rx.h"

#define RX_BUFFER_SIZE 128

static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];

static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;


void bt_rx_init(void)
{
    /* Enable USART1 RX interrupt */
    USART1->CR1 |= USART_CR1_RXNEIE;

    /* Enable USART1 interrupt in NVIC */
    NVIC_EnableIRQ(USART1_IRQn);
}


void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        uint8_t c = (uint8_t)USART1->DR;

        uint16_t next =
            (rx_head + 1) % RX_BUFFER_SIZE;

        /*
         * Only store the byte if the buffer
         * isn't full.
         */
        if (next != rx_tail)
        {
            rx_buffer[rx_head] = c;
            rx_head = next;
        }
    }
}


uint8_t bt_rx_available(void)
{
    return (rx_head != rx_tail);
}


uint8_t bt_rx_read(uint8_t *data)
{
    if (rx_head == rx_tail)
        return 0;

    *data = rx_buffer[rx_tail];

    rx_tail =
        (rx_tail + 1) % RX_BUFFER_SIZE;

    return 1;
}



#include "led.h"
#include "main.h"

void led_on(void)
{
    GPIOC->BSRR = (1U << 13);
}

void led_off(void)
{
    GPIOC->BSRR = (1U << (13 + 16));
}

void led_toggle(void)
{
    if(GPIOC->ODR & (1U << 13))
        led_off();
    else
        led_on();
}

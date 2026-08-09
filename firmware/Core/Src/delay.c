#include "delay.h"
#include "main.h"


void delay_ms(uint32_t ms)
{
    while(ms--)
    {
    	TIM2->CNT = 0;
    	while (TIM2->CNT < 1000);
    }
}

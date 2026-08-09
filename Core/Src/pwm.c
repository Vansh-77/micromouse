#include "pwm.h"
#include "main.h"

void pwm_set(uint8_t ch , uint16_t duty)
{
    if(duty > 100)
        duty = 100;

    switch (ch){
    case 1:
    	TIM3->CCR1 = ((TIM3->ARR +1)*duty)/100;
    	break;
    case 2:
    	TIM3->CCR2 = ((TIM3->ARR +1)*duty)/100;
    	break;
    case 3:
    	TIM3->CCR3 = ((TIM3->ARR +1)*duty)/100;
    	break;
    case 4:
    	TIM3->CCR4 = ((TIM3->ARR +1)*duty)/100;
    	break;
    default:
    }

}

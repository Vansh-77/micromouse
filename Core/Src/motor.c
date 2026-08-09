#include "motor.h"
#include "main.h"
#include "pwm.h"

void motor_left(int16_t speed){

//	if(speed >100) speed = 100;
//	else if (speed < -100)speed = -100; already present in pwm_set()

	if(speed >= 0){
		IN2_GPIO_Port->BSRR =  (uint32_t)IN2_Pin << 16;
		pwm_set(IN1_CHANNEL,speed);
	}
	else if(speed < 0){
		IN1_GPIO_Port->BSRR = (uint32_t)IN1_Pin << 16;
		pwm_set(IN2_CHANNEL,-speed);
	}
}

void motor_right(int16_t speed){

//	if(speed >100) speed = 100;
//	else if (speed < -100)speed = -100; already present in pwm_set()

	if(speed >= 0){
		IN4_GPIO_Port->BSRR = (uint32_t)IN4_Pin << 16;
		pwm_set(IN3_CHANNEL,speed);
	}
	else if(speed < 0){
		IN3_GPIO_Port->BSRR = (uint32_t)IN3_Pin << 16;
		pwm_set(IN4_CHANNEL,-speed);
	}
}




#include "i2c.h"
#include "main.h"

uint8_t i2c_read_reg(uint8_t addr , uint8_t reg){
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB));
	I2C1->DR = (addr << 1);
	while (!(I2C1->SR1 & I2C_SR1_ADDR));
	// clear ADDR
	volatile uint32_t temp;
	temp = I2C1->SR1;
	temp = I2C1->SR2;
	(void)temp;
	while (!(I2C1->SR1 & I2C_SR1_TXE));
	I2C1->DR = reg;
	while (!(I2C1->SR1 & I2C_SR1_BTF));
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB));
	I2C1->DR = (addr << 1) | 1;
	while (!(I2C1->SR1 & I2C_SR1_ADDR));
	temp = I2C1->SR1;
	temp = I2C1->SR2;
	(void)temp;
	I2C1->CR1 &= ~I2C_CR1_ACK;
	I2C1->CR1 |= I2C_CR1_STOP;
	while (!(I2C1->SR1 & I2C_SR1_RXNE));
	uint8_t data = I2C1->DR;
	I2C1->CR1 |= I2C_CR1_ACK;

	return data;
}

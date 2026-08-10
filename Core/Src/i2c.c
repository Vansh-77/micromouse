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

void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data){
	// start
	I2C1->CR1 |= I2C_CR1_START;
	// wait for start bit
	while (!(I2C1->SR1 & I2C_SR1_SB));
	// write device address with write bit 0
	I2C1->DR = (addr << 1);
	// wait for addr write
	while (!(I2C1->SR1 & I2C_SR1_ADDR));
	// clear addr reg
	volatile uint32_t temp;
	temp = I2C1->SR1;
	temp = I2C1->SR2;
	(void)temp;
	//wait until transmit data reg is empty
	while (!(I2C1->SR1 & I2C_SR1_TXE));
	// send reg	addr we want to write to
	I2C1->DR = reg;
	while (!(I2C1->SR1 & I2C_SR1_TXE));
	//write data
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_BTF));
	// stop
	I2C1->CR1 |= I2C_CR1_STOP;

}

void i2c_read_regs(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    volatile uint32_t temp;
    // START
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
    // Device address + WRITE
    I2C1->DR = (addr << 1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    // Clear ADDR
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;
    // Send register address
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = reg;
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    // Repeated START
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
    // Device address + READ
    I2C1->DR = (addr << 1) | 1;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    /*
     * Clear ADDR while keeping ACK enabled.
     * For a multi-byte read we ACK every byte except
     * the final byte.
     */
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            // Last byte: send NACK
            I2C1->CR1 &= ~I2C_CR1_ACK;

            // Generate STOP
            I2C1->CR1 |= I2C_CR1_STOP;
        }
        else
        {
            // ACK this byte so the slave sends the next one
            I2C1->CR1 |= I2C_CR1_ACK;
        }

        // Wait until received byte is ready
        while (!(I2C1->SR1 & I2C_SR1_RXNE));

        data[i] = I2C1->DR;
    }

    // Restore ACK for future transactions
    I2C1->CR1 |= I2C_CR1_ACK;
}











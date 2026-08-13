#include "i2c.h"
#include "main.h"
#define I2C_TIMEOUT 1000000U


static uint8_t i2c_wait_flag(uint32_t flag){
    uint32_t timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & flag)){
        if (--timeout == 0)return 0;
    }
    return 1;
}

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




uint8_t i2c_read_regs(uint8_t addr,
                      uint8_t reg,
                      uint8_t *data,
                      uint8_t len)
{
    volatile uint32_t temp;
    uint8_t i = 0;

    if (len == 0)
        return 0;

    /* Normal receive configuration */
    I2C1->CR1 &= ~I2C_CR1_POS;
    I2C1->CR1 |= I2C_CR1_ACK;

    /* =========================
       START
       ========================= */

    I2C1->CR1 |= I2C_CR1_START;

    if (!i2c_wait_flag(I2C_SR1_SB))
        goto error;


    /* =========================
       DEVICE ADDRESS + WRITE
       ========================= */

    I2C1->DR = (addr << 1);

    if (!i2c_wait_flag(I2C_SR1_ADDR))
        goto error;

    /* Clear ADDR */
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;


    /* =========================
       REGISTER ADDRESS
       ========================= */

    if (!i2c_wait_flag(I2C_SR1_TXE))
        goto error;

    I2C1->DR = reg;

    if (!i2c_wait_flag(I2C_SR1_BTF))
        goto error;


    /* =========================
       REPEATED START
       ========================= */

    I2C1->CR1 |= I2C_CR1_START;

    if (!i2c_wait_flag(I2C_SR1_SB))
        goto error;


    /* =========================
       DEVICE ADDRESS + READ
       ========================= */

    I2C1->DR = (addr << 1) | 1;

    if (!i2c_wait_flag(I2C_SR1_ADDR))
        goto error;


    /* Clear ADDR */
    temp = I2C1->SR1;
    temp = I2C1->SR2;
    (void)temp;


    /* =========================
       RECEIVE
       ========================= */

    if (len == 1)
    {
        /*
         * One-byte receive
         */

        I2C1->CR1 &= ~I2C_CR1_ACK;

        I2C1->CR1 |= I2C_CR1_STOP;

        if (!i2c_wait_flag(I2C_SR1_RXNE))
            goto error;

        data[0] = I2C1->DR;
    }

    else if (len == 2)
    {
        /*
         * Two-byte receive
         */

        I2C1->CR1 &= ~I2C_CR1_ACK;
        I2C1->CR1 |= I2C_CR1_POS;

        if (!i2c_wait_flag(I2C_SR1_BTF))
            goto error;

        I2C1->CR1 |= I2C_CR1_STOP;

        data[0] = I2C1->DR;
        data[1] = I2C1->DR;

        I2C1->CR1 &= ~I2C_CR1_POS;
    }

    else
    {
        /*
         * 3+ byte receive
         */

        while (len > 3)
        {
            if (!i2c_wait_flag(I2C_SR1_BTF))
                goto error;

            data[i++] = I2C1->DR;

            len--;
        }

        /*
         * Three bytes remain.
         */

        if (!i2c_wait_flag(I2C_SR1_BTF))
            goto error;

        /*
         * Stop ACKing.
         */
        I2C1->CR1 &= ~I2C_CR1_ACK;

        /*
         * Read first of final three bytes.
         */
        data[i++] = I2C1->DR;

        /*
         * Generate STOP.
         */
        I2C1->CR1 |= I2C_CR1_STOP;

        /*
         * Read second-last byte.
         */
        data[i++] = I2C1->DR;

        /*
         * Wait for final byte.
         */
        if (!i2c_wait_flag(I2C_SR1_RXNE))
            goto error;

        /*
         * Read final byte.
         */
        data[i++] = I2C1->DR;
    }

    /* Restore normal configuration */
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 &= ~I2C_CR1_POS;

    return 1;


error:

    /*
     * Something went wrong.
     * Make sure the bus is released.
     */
    I2C1->CR1 |= I2C_CR1_STOP;

    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 &= ~I2C_CR1_POS;

    return 0;
}









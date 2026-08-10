#ifndef  I2C_H
#define I2C_H
#include <stdint.h>

uint8_t i2c_read_reg(uint8_t addr, uint8_t reg);
void i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data);
void i2c_read_regs(uint8_t addr,
                   uint8_t reg,
                   uint8_t *data,
                   uint8_t len);

#endif

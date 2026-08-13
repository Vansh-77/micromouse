#include "main.h"
#include "mpu6500.h"
#include "i2c.h"

void mpu6500_init(void){
	i2c_write_reg(MPU6500_ADDR, MPU6500_PWR_MGMT1, 0x00);
}

uint8_t mpu6500_whoami(void){
	return i2c_read_reg(MPU6500_ADDR, MPU6500_WHO_AM_I)	;
}

uint8_t mpu6500_read(MPU6500_Data *data){
	uint8_t buffer[14];

	if (!i2c_read_regs(MPU6500_ADDR,
	                   0x3B,
	                   buffer,
	                   14))
	{
	    return 0;
	}

	data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
	data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
	data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);
	data->temperature = (int16_t)((buffer[6] << 8) | buffer[7]);
	data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
	data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
	data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);

}

#include "main.h"
#include "mpu6500.h"
#include "i2c.h"

void mpu6500_init(void){
	/* Wake MPU6500 */
    i2c_write_reg(MPU6500_ADDR,
                  MPU6500_PWR_MGMT1,
                  0x00);

    /* DLPF configuration */
    i2c_write_reg(MPU6500_ADDR,
                  MPU6500_CONFIG,
                  0x03);

    /* Gyroscope ±250 °/s */
    i2c_write_reg(MPU6500_ADDR,
                  MPU6500_GYRO_CONFIG,
                  0x00);

    /* Accelerometer ±2g */
    i2c_write_reg(MPU6500_ADDR,
                  MPU6500_ACCEL_CONFIG,
                  0x00);

    /* Sample rate */
    i2c_write_reg(MPU6500_ADDR,
                  MPU6500_SMPLRT_DIV,
                  9);
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

	data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]) / 16384.0f;
	data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3])/ 16384.0f;
	data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5])/ 16384.0f;
	data->temperature =(float)((int16_t)((buffer[6] << 8) | buffer[7]))/ 333.87f + 21.0f;
	data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]) / 131.0f;
	data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]) / 131.0f;
	data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]) / 131.0f;
	return 1;

}

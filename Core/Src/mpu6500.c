#include "main.h"
#include "mpu6500.h"
#include "i2c.h"
#include "delay.h"
#include "uart.h"

static float gyro_bias_x = 0.0f;
static float gyro_bias_y = 0.0f;
static float gyro_bias_z = 0.0f;

static float heading = 0.0f;

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
	data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]) / 131.0f - gyro_bias_x;
	data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]) / 131.0f - gyro_bias_y;
	data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]) / 131.0f - gyro_bias_z;
	return 1;

}

void mpu6500_calibrate_gyro(void)
{
    const uint16_t samples = 500;

    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;

    MPU6500_Data data;

    for (uint16_t i = 0; i < samples; i++)
    {
        if (mpu6500_read(&data))
        {
            sum_x += data.gyro_x;
            sum_y += data.gyro_y;
            sum_z += data.gyro_z;
        }

        delay_ms(2);
    }

    gyro_bias_x = sum_x / samples;
    gyro_bias_y = sum_y / samples;
    gyro_bias_z = sum_z / samples;
    uart_printf("g_bias_x %.3f g_bias_y %.3f g_bias_z %.3f\r\n", gyro_bias_x , gyro_bias_y , gyro_bias_z);
}

void mpu6500_update_heading(float dt)
{
    MPU6500_Data imu;

    if (mpu6500_read(&imu))
    {
        heading += imu.gyro_z * dt;

        /* Keep heading between -180 and +180 */
        if (heading > 180.0f)
            heading -= 360.0f;

        if (heading < -180.0f)
            heading += 360.0f;
    }
}
float mpu6500_get_heading(void)
{
    return heading;
}

void mpu6500_reset_heading(void)
{
    heading = 0.0f;
}




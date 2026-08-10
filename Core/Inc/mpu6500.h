#ifndef MPU6500_H
#define MPU6500_H
#include <stdint.h>

#define MPU6500_ADDR       0x68
#define MPU6500_WHO_AM_I   0x75
#define MPU6500_PWR_MGMT1  0x6B

void mpu6500_init(void);
uint8_t mpu6500_whoami(void);

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t temperature;
} MPU6500_Data;

void mpu6500_read(MPU6500_Data *data);


#endif

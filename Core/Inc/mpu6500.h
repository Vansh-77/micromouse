#ifndef MPU6500_H
#define MPU6500_H
#include <stdint.h>

#define MPU6500_ADDR       0x68
#define MPU6500_WHO_AM_I   0x75
#define MPU6500_PWR_MGMT1  0x6B
#define MPU6500_CONFIG       0x1A
#define MPU6500_GYRO_CONFIG  0x1B
#define MPU6500_ACCEL_CONFIG 0x1C
#define MPU6500_SMPLRT_DIV   0x19

void mpu6500_init(void);
uint8_t mpu6500_whoami(void);

typedef struct
{
    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float temperature;
} MPU6500_Data;

uint8_t mpu6500_read(MPU6500_Data *data);
void mpu6500_calibrate_gyro(void);


#endif

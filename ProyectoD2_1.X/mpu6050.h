/* #include "i2c.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 12/05/2020
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 8000000

#include "I2C.h"

#define W_DATA         0xD0
#define R_DATA         0xD1
#define PWR_MGMT_1     0x6B
#define PWR_MGMT_2     0x6C
#define SMPRT_DIV      0x19
#define CONFIG_R       0x1A
#define GYRO_CONFIG    0x1B
#define ACCEL_CONFIG   0x1C
#define ACCEL_XOUT_H   0x3B
#define ACCEL_XOUT_L   0x3C
#define ACCEL_YOUT_H   0x3D
#define ACCEL_YOUT_L   0x3E
#define ACCEL_ZOUT_H   0x3F
#define ACCEL_ZOUT_L   0x40
#define TEMP_OUT_H     0x41
#define TEMP_OUT_L     0x42
#define GYRO_XOUT_H    0x43
#define GYRO_XOUT_L    0x44
#define GYRO_YOUT_H    0x45
#define GYRO_YOUT_L    0x46
#define GYRO_ZOUT_H    0x47
#define GYRO_ZOUT_L    0x48

void MPU6050_Init(void);
int MPU6050_Read(unsigned char add);
void MPU6050_Write(unsigned char add, unsigned char data);

float MPU6050_Read_Ax(void);
float MPU6050_Read_Ay(void);
float MPU6050_Read_Az(void);
float MPU6050_Read_Gx(void);
float MPU6050_Read_Gy(void);
float MPU6050_Read_Gz(void);
float MPU6050_Read_Temperature(void);
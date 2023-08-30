/*#include "i2c.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 12/05/2020
* Electronica y Circuitos
*
*/

#include "mpu6050.h"

void MPU6050_Write(unsigned char add, unsigned char data)
{
   I2C_Master_Start();
   I2C_Master_Write(W_DATA);
   I2C_Master_Write(add);
   I2C_Master_Write(data);
   I2C_Master_Stop();
}

void MPU6050_Init(void)
{
    MPU6050_Write(PWR_MGMT_1, 0x80);
    __delay_ms(10);
    MPU6050_Write(PWR_MGMT_1, 0x00);
    __delay_ms(10);
    MPU6050_Write(CONFIG_R, 0x01);
    __delay_ms(10);
    MPU6050_Write(GYRO_CONFIG, 0x00);
}

int MPU6050_Read(unsigned char add)
{
    int retval;
    I2C_Master_Start();
    I2C_Master_Write(W_DATA);
    I2C_Master_Write(add);
    I2C_Master_RepeatedStart();
    I2C_Master_Write(R_DATA);
    retval = I2C_Master_Read(0);
    I2C_Master_Stop();
    return retval;
}

float MPU6050_Read_Ax(void)
{
    return (float)(((int)(MPU6050_Read(ACCEL_XOUT_H) << 8 | MPU6050_Read(ACCEL_XOUT_L)))/(float)16384);
}

float MPU6050_Read_Ay(void)
{
    return (float)(((int)(MPU6050_Read(ACCEL_YOUT_H) << 8 | MPU6050_Read(ACCEL_YOUT_L)))/(float)16384);
}

float MPU6050_Read_Az(void)
{
    return (float)(((int)(MPU6050_Read(ACCEL_ZOUT_H) << 8 | MPU6050_Read(ACCEL_ZOUT_L)))/(float)16384);
}

float MPU6050_Read_Gx(void)
{
    return (float)(((int)(MPU6050_Read(GYRO_XOUT_H) << 8 | MPU6050_Read(GYRO_XOUT_L)))/(float)131);
}

float MPU6050_Read_Gy(void)
{
    return (float)(((int)(MPU6050_Read(GYRO_YOUT_H) << 8 | MPU6050_Read(GYRO_YOUT_L)))/(float)131);
}

float MPU6050_Read_Gz(void)
{
    return (float)(((int)(MPU6050_Read(GYRO_ZOUT_H) << 8 | MPU6050_Read(GYRO_ZOUT_L)))/(float)131);;
}

float MPU6050_Read_Temperature(void)
{
    return (float)(((int)(MPU6050_Read(TEMP_OUT_H) << 8 | MPU6050_Read(TEMP_OUT_L)))/(float)340+(float)36.53);
}
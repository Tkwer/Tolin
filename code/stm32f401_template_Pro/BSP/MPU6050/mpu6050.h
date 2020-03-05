#ifndef _MPU6050_H
#define _MPU6050_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "math.h"
#define IIC_SCL(n)		(n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))
#define IIC_SDA(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET))

#define SCL		!(!(GPIOA->IDR & (1 << 15))) 	// SCL 逻辑状态
#define SDA		!(!(GPIOB->IDR & (1 << 3)))     // SDA 逻辑状态

//---------------------------------------------------------------------------------------------------------------------------------------

#define DEV_ADDR	0xD0	// 6050 器件地址

//-----------------------------------------
// 定义MPU6050内部地址
//-----------------------------------------
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

/* 加速度相关寄存器地址 */
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

/* 温度相关寄存器地址 */
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

/* 陀螺仪相关寄存器地址 */
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

/* 传感器数据修正值（消除芯片固定误差，根据硬件进行调整） */
#define X_ACCEL_OFFSET -600 
#define Y_ACCEL_OFFSET -100 
#define Z_ACCEL_OFFSET 2900 
#define X_GYRO_OFFSET 32 
#define Y_GYRO_OFFSET -11 
#define Z_GYRO_OFFSET 1 

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取

typedef struct Angle
{
    double X_Angle;
    double Y_Angle;
    double Z_Angle;
    
} MPU6050_Angle;
void MPU6050_Init(void);
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData);
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData);
uint8_t MPU6050_Read_Reg(uint8_t regAddr);
int16_t MPU6050_Get_Data(uint8_t regAddr);
void MPU6050_Display(void);
void MPU6050_Get_Angle(MPU6050_Angle *data);

#endif 

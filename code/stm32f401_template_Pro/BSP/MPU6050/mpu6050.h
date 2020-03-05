#ifndef _MPU6050_H
#define _MPU6050_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "math.h"
#define IIC_SCL(n)		(n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))
#define IIC_SDA(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET))

#define SCL		!(!(GPIOA->IDR & (1 << 15))) 	// SCL �߼�״̬
#define SDA		!(!(GPIOB->IDR & (1 << 3)))     // SDA �߼�״̬

//---------------------------------------------------------------------------------------------------------------------------------------

#define DEV_ADDR	0xD0	// 6050 ������ַ

//-----------------------------------------
// ����MPU6050�ڲ���ַ
//-----------------------------------------
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

/* ���ٶ���ؼĴ�����ַ */
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

/* �¶���ؼĴ�����ַ */
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

/* ��������ؼĴ�����ַ */
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

/* ��������������ֵ������оƬ�̶�������Ӳ�����е����� */
#define X_ACCEL_OFFSET -600 
#define Y_ACCEL_OFFSET -100 
#define Z_ACCEL_OFFSET 2900 
#define X_GYRO_OFFSET 32 
#define Y_GYRO_OFFSET -11 
#define Z_GYRO_OFFSET 1 

#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	SlaveAddress	0xD0	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ

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

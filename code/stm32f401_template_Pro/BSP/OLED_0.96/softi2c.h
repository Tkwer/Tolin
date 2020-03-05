/**
  ******************************************************************************
  * 
  * ���ģ��i2cЭ��(֧��mpu6050��mpu9250��oled��i2c�豸)
  *
  *
  * ʹ��˵����
  *		1.������֧��stm32f4ϵ�е�Ƭ����
  *		2.�޸Ķ˿ڿ��ƣ���ο��޸�softi2c.h�ļ����ݣ�
  *		3.�����ԣ���ʹ����mpu6050��mpu9250��oled��i2c�豸�ϣ�
  *		4.ע�⣬���ļ����漰���ĵ�ַΪi2c��ַ���豸��ַ��Ҫ������λ����ſ�ʳ�ã�
  *		5.���ļ������ļ���ST�̼����⣬����delay.c/.h��sys.h��
  *
  * �汾�޶���
  *		�޸�ʱ��	�汾��	�޸�����
  *		2017-12-20	v1.0	����(֧��mpu6050��mpu9250)��
  *		2017-12-20	v2.0	���Ӷ�oled��֧�֣�
  *
  * ���������
  *		Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI
  * 	All rights reserved			
  *
  *
  *	Create  ��2017��12��20��
  * Update  ��2017��12��20��
  * Author  ��������(SERI.LJI)
  * Email	��liangzongnan0214@163.com
  * Blog    ��liangzongnan.com
  * QQ		��494089986
  * 
  ******************************************************************************
***/



#ifndef __SOFTWARE_I2C_H__
#define __SOFTWARE_I2C_H__



/*softi2c.h Needed Library Define*/
#include "stm32f4xx.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_gpio.h"
#include "sys.h"



/*softi2c.h Command Define*/
#define	SW_I2C_PORT									GPIOB
#define	SW_I2C_PIN_NUM_SCL					8								//����SCL��ʱ����
#define	SW_I2C_PIN_NUM_SDA					9								//����SDA��������

#define	SW_I2C_SCL_L()						PBout(SW_I2C_PIN_NUM_SCL) = 0	//����֧��sys.h�ļ���ʹ�ùٷ��̼���(stm32f4xx_gpio.h)�޸ģ�
#define	SW_I2C_SCL_H()						PBout(SW_I2C_PIN_NUM_SCL) = 1
#define	SW_I2C_SDA_L()						PBout(SW_I2C_PIN_NUM_SDA) = 0
#define	SW_I2C_SDA_H()						PBout(SW_I2C_PIN_NUM_SDA) = 1
#define	SW_I2C_SDA_READ()					PBin(SW_I2C_PIN_NUM_SDA)


//���������ʱ����ֱ�ӽ���Ӧ��ѡ��Ϊ�������ʹ�ã�����ʹ�ú궨�壡
#define SW_I2C_NACK							0
#define SW_I2C_ACK							1



/*softi2c.h Function Define*/
void SW_I2C_InitConfig(void);//��ʼ��i2c��io��				 

void SW_I2C_Send(uint8_t data);//����һ���ֽ�����
uint8_t SW_I2C_Read(uint8_t ack);//����һ���ֽ�����	
void SW_I2C_WrDat(uint8_t dat);
void SW_I2C_WrCmd(uint8_t cmd);
uint8_t SW_I2C_SendByte(uint8_t addr, uint8_t reg, uint8_t data);//i2c��Ŀ���ַ����һ���ֽ�
uint8_t SW_I2C_ReadByte(uint8_t addr, uint8_t reg, uint8_t *data);//i2c��Ŀ���ַ����һ���ֽ�

uint8_t SW_I2C_SendBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);//i2c��Ŀ���ַ����ָ�����ȵ�����
uint8_t SW_I2C_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);//i2c��Ŀ���ַ����ָ�����ȵ�����




#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










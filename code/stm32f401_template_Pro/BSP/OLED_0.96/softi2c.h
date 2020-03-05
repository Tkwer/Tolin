/**
  ******************************************************************************
  * 
  * 软件模拟i2c协议(支持mpu6050、mpu9250、oled等i2c设备)
  *
  *
  * 使用说明：
  *		1.本部分支持stm32f4系列单片机；
  *		2.修改端口控制，请参考修改softi2c.h文件内容；
  *		3.经测试，可使用在mpu6050、mpu9250、oled等i2c设备上；
  *		4.注意，本文件中涉及到的地址为i2c地址，设备地址需要经过移位运算才可食用！
  *		5.本文件依赖文件除ST固件库外，还需delay.c/.h、sys.h；
  *
  * 版本修订：
  *		修改时间	版本号	修改内容
  *		2017-12-20	v1.0	搭建框架(支持mpu6050、mpu9250)；
  *		2017-12-20	v2.0	增加对oled的支持；
  *
  * 程序出处：
  *		Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI
  * 	All rights reserved			
  *
  *
  *	Create  ：2017年12月20日
  * Update  ：2017年12月20日
  * Author  ：梁宗南(SERI.LJI)
  * Email	：liangzongnan0214@163.com
  * Blog    ：liangzongnan.com
  * QQ		：494089986
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
#define	SW_I2C_PIN_NUM_SCL					8								//接入SCL，时钟线
#define	SW_I2C_PIN_NUM_SDA					9								//接入SDA，数据线

#define	SW_I2C_SCL_L()						PBout(SW_I2C_PIN_NUM_SCL) = 0	//若不支持sys.h文件，使用官方固件库(stm32f4xx_gpio.h)修改！
#define	SW_I2C_SCL_H()						PBout(SW_I2C_PIN_NUM_SCL) = 1
#define	SW_I2C_SDA_L()						PBout(SW_I2C_PIN_NUM_SDA) = 0
#define	SW_I2C_SDA_H()						PBout(SW_I2C_PIN_NUM_SDA) = 1
#define	SW_I2C_SDA_READ()					PBin(SW_I2C_PIN_NUM_SDA)


//由于我设计时接收直接进行应答选择，为方便参数使用，建议使用宏定义！
#define SW_I2C_NACK							0
#define SW_I2C_ACK							1



/*softi2c.h Function Define*/
void SW_I2C_InitConfig(void);//初始化i2c的io口				 

void SW_I2C_Send(uint8_t data);//发送一个字节数据
uint8_t SW_I2C_Read(uint8_t ack);//接收一个字节数据	
void SW_I2C_WrDat(uint8_t dat);
void SW_I2C_WrCmd(uint8_t cmd);
uint8_t SW_I2C_SendByte(uint8_t addr, uint8_t reg, uint8_t data);//i2c向目标地址发送一个字节
uint8_t SW_I2C_ReadByte(uint8_t addr, uint8_t reg, uint8_t *data);//i2c从目标地址接收一个字节

uint8_t SW_I2C_SendBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);//i2c向目标地址发送指定长度的数据
uint8_t SW_I2C_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);//i2c从目标地址接收指定长度的数据




#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










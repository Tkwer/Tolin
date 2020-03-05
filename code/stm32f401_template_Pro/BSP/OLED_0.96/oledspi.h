/**
  ******************************************************************************
  * 
  * 软件模拟spi协议(仅支持oled)
  *
  *
  * 使用说明：
  *		1.本部分支持stm32f4系列单片机；
  *		2.修改端口控制，请参考修改softspi.h文件内容；
  *		3.经测试，可使用在oled等spi设备上；
  *		4.本文件依赖文件除ST固件库外，还需delay./c.h、sys.h；
  *
  *
  *	Create  ：2017年12月23日
  * Update  ：2017年12月23日
  * Author  ：梁宗南(SERI.LJI)
  * Email	：liangzongnan0214@163.com
  * Blog    ：liangzongnan.com
  * QQ		：494089986
  * 
  ******************************************************************************
***/



#ifndef __OLED_SPI_H__
#define __OLED_SPI_H__



/*oledspi.h Needed Library Define*/
#include "stm32f4xx.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_gpio.h"
#include "sys.h"



/*oledspi.h Command Define*/
#define	OLED_SPI_PORT						GPIOD
#define	OLED_SPI_PIN_NUM_CS					0								//接入CS， spi从机选定
#define	OLED_SPI_PIN_NUM_DC					1 			 					//接入DC， 命令|数据选择(0命令 1数据)
#define	OLED_SPI_PIN_NUM_RST				2								//接入RST，硬件复位(视情况省略！)
#define	OLED_SPI_PIN_NUM_D1					3								//接入D1， 数据
#define	OLED_SPI_PIN_NUM_D0					4								//接入D0， 时钟

#define	OLED_SPI_CS_L()						PDout(OLED_SPI_PIN_NUM_CS) = 0	//若不支持sys.h文件，使用官方固件库(stm32f4xx_gpio.h)修改！
#define	OLED_SPI_CS_H()						PDout(OLED_SPI_PIN_NUM_CS) = 1
#define	OLED_SPI_D1_L()						PDout(OLED_SPI_PIN_NUM_D1) = 0
#define	OLED_SPI_D1_H()						PDout(OLED_SPI_PIN_NUM_D1) = 1
#define	OLED_SPI_D0_L()						PDout(OLED_SPI_PIN_NUM_D0) = 0
#define	OLED_SPI_D0_H()						PDout(OLED_SPI_PIN_NUM_D0) = 1
#define	OLED_SPI_DC_L()						PDout(OLED_SPI_PIN_NUM_DC) = 0
#define	OLED_SPI_DC_H()						PDout(OLED_SPI_PIN_NUM_DC)  = 1
#define	OLED_SPI_RST_L()           	     	PDout(OLED_SPI_PIN_NUM_RST) = 0
#define	OLED_SPI_RST_H()					PDout(OLED_SPI_PIN_NUM_RST) = 1



/*oledspi.h Function Define*/
void OLED_SPI_InitConfig(void);//spi协议下oled初始化

void OLED_SPI_WriteCommand(uint8_t command);//发送命令给oled
void OLED_SPI_WriteData(uint8_t data);//发送数据给oled



#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










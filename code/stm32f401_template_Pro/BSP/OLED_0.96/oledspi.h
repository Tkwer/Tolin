/**
  ******************************************************************************
  * 
  * ���ģ��spiЭ��(��֧��oled)
  *
  *
  * ʹ��˵����
  *		1.������֧��stm32f4ϵ�е�Ƭ����
  *		2.�޸Ķ˿ڿ��ƣ���ο��޸�softspi.h�ļ����ݣ�
  *		3.�����ԣ���ʹ����oled��spi�豸�ϣ�
  *		4.���ļ������ļ���ST�̼����⣬����delay./c.h��sys.h��
  *
  *
  *	Create  ��2017��12��23��
  * Update  ��2017��12��23��
  * Author  ��������(SERI.LJI)
  * Email	��liangzongnan0214@163.com
  * Blog    ��liangzongnan.com
  * QQ		��494089986
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
#define	OLED_SPI_PIN_NUM_CS					0								//����CS�� spi�ӻ�ѡ��
#define	OLED_SPI_PIN_NUM_DC					1 			 					//����DC�� ����|����ѡ��(0���� 1����)
#define	OLED_SPI_PIN_NUM_RST				2								//����RST��Ӳ����λ(�����ʡ�ԣ�)
#define	OLED_SPI_PIN_NUM_D1					3								//����D1�� ����
#define	OLED_SPI_PIN_NUM_D0					4								//����D0�� ʱ��

#define	OLED_SPI_CS_L()						PDout(OLED_SPI_PIN_NUM_CS) = 0	//����֧��sys.h�ļ���ʹ�ùٷ��̼���(stm32f4xx_gpio.h)�޸ģ�
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
void OLED_SPI_InitConfig(void);//spiЭ����oled��ʼ��

void OLED_SPI_WriteCommand(uint8_t command);//���������oled
void OLED_SPI_WriteData(uint8_t data);//�������ݸ�oled



#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










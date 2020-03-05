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



#include "oledspi.h"
#include "delay.h"


/**
  * @brief	:��ʼ��oled��spi�ӿ�
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/23
  * @design :
  **/
void OLED_SPI_InitConfig(void)
{
	GPIO_InitTypeDef  gpio;
	
	RCC_AHB1PeriphClockCmd((1<<(((u32)OLED_SPI_PORT - AHB1PERIPH_BASE)>>10)), ENABLE);//port clock enable!

	gpio.GPIO_Pin   = (1<<OLED_SPI_PIN_NUM_DC)
					| (1<<OLED_SPI_PIN_NUM_CS)
					| (1<<OLED_SPI_PIN_NUM_D1)
					| (1<<OLED_SPI_PIN_NUM_D0)
					| (1<<OLED_SPI_PIN_NUM_RST);
	gpio.GPIO_Mode  = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd  = GPIO_PuPd_UP;	
	GPIO_Init(OLED_SPI_PORT, &gpio);

	OLED_SPI_CS_L();
	OLED_SPI_RST_L();
	delay_ms(100);
	OLED_SPI_RST_H();
	delay_ms(100);
}



/**
  * @brief	:����һ���ֽ�����
  * @note   :--��λ�ȷ���
  * @param	:data, �����͵�����
  * @return	:void
  *
  * @date   :2017/12/23
  * @design :
  **/
static void OLED_SPI_SendByte(uint8_t data)
{                        
	uint8_t i;   

	for(i=0; i<8; i++)
	{
		OLED_SPI_D0_L();
		if(data & 0x80)//��λ�ȷ��ͣ�
			OLED_SPI_D1_H();
		else 
			OLED_SPI_D1_L();
		OLED_SPI_D0_H();
		
		data <<= 1;
	}
} 



/**
  * @brief	:���������oled
  * @note   :--
  * @param	:command, �����͵���������
  * @return	:void
  *
  * @date   :2017/12/23
  * @design :
  **/
void OLED_SPI_WriteCommand(uint8_t command)
{
	OLED_SPI_DC_L();
	OLED_SPI_CS_L();
	
	OLED_SPI_SendByte(command);
	
	OLED_SPI_CS_H();
	OLED_SPI_DC_H();
}



/**
  * @brief	:�������ݸ�oled
  * @note   :--
  * @param	:data, �����͵�����
  * @return	:void
  *
  * @date   :2017/12/23
  * @design :
  **/
void OLED_SPI_WriteData(uint8_t data)
{
	OLED_SPI_DC_H();
	OLED_SPI_CS_L();
	
	OLED_SPI_SendByte(data);
	
	OLED_SPI_CS_H();
	OLED_SPI_DC_H();
}

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










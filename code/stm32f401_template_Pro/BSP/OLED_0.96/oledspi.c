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



#include "oledspi.h"
#include "delay.h"


/**
  * @brief	:初始化oled的spi接口
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
  * @brief	:发送一个字节数据
  * @note   :--高位先发！
  * @param	:data, 待发送的数据
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
		if(data & 0x80)//高位先发送！
			OLED_SPI_D1_H();
		else 
			OLED_SPI_D1_L();
		OLED_SPI_D0_H();
		
		data <<= 1;
	}
} 



/**
  * @brief	:发送命令给oled
  * @note   :--
  * @param	:command, 待发送的命令数据
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
  * @brief	:发送数据给oled
  * @note   :--
  * @param	:data, 待发送的数据
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










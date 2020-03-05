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



#include "softi2c.h"
#include "delay.h"



/* 以下函数不推荐在外部文件中使用，若需使用，建议通过extern声明！*/
uint8_t SW_I2C_Start(void);		//发送i2c起始信号
void SW_I2C_Stop(void);			//发送i2c停止信号
uint8_t SW_I2C_WaitAck(void);	//i2c等待ack信号
void SW_I2C_Ack(void);			//i2c发送ack信号
void SW_I2C_NAck(void);			//i2c发送nack信号


/**
  * @brief	:初始化i2c的io口
  * @note   :--根据标准i2c协议，这里设置成开漏模式，需外置上拉电阻！
			 --若无外界上拉电阻，可尝试修改本程序，但不建议！！
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_InitConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_Initure.Pin   = (1<<SW_I2C_PIN_NUM_SCL)
					| (1<<SW_I2C_PIN_NUM_SDA);
	GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_OD;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SW_I2C_PORT,&GPIO_Initure);
}



/**
  * @brief	:发送i2c起始信号
  * @note   :--scl高电平期间，sda下降沿表示起始！
			 --可能总线正在被占用，导致起始失败！
  * @param	:void
  * @return	:0, 成功
			 1, 失败
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_Start(void)
{	
	SW_I2C_SDA_H();//释放总线
	SW_I2C_SCL_H();
	delay_us(2);
	if(!SW_I2C_SDA_READ()) return 1;//回检一次电平状态！
	
	SW_I2C_SDA_L();//scl高电平期间，sda下降沿为起始信号
	delay_us(2);
	if(SW_I2C_SDA_READ()) return 1;//回检一次电平状态！

	delay_us(2);
	return 0;
}



/**
  * @brief	:发送i2c停止信号
  * @note   :--scl高电平期间，sda上升沿表示停止！
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Stop(void)
{
	SW_I2C_SCL_L();//占用总线！
	delay_us(2);
	
	SW_I2C_SDA_L();
	SW_I2C_SCL_H();
	delay_us(2);
	
	SW_I2C_SDA_H();//scl高电平期间，sda上升沿为停止信号
	delay_us(2);	
}



/**
  * @brief	:i2c等待ack信号
  * @note   :--释放sda和scl，等待从机操作总线！
  * @param	:void
  * @return	:1, 接收应答失败
			 0, 接收应答成功
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_WaitAck(void)
{
	uint8_t errDly=100;
		
	SW_I2C_SCL_L();//占用总线！
	delay_us(2);
	
	SW_I2C_SDA_H();//释放sda
	delay_us(1);
	SW_I2C_SCL_H();//释放scl，等待从机发送！
	delay_us(1);
			
	while(SW_I2C_SDA_READ())
	{
		errDly--;
		if(!errDly)//随便给定的数字，大概值！-- 2017.12.20 by seri_liang.
		{
			SW_I2C_Stop();
			return 1;
		}
	}	
	
	SW_I2C_SCL_L();//
	return 0;  
}



/**
  * @brief	:i2c发送ack信号
  * @note   :--sda低电平，scl下降沿发送，表示应答！
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Ack(void)
{	
	SW_I2C_SCL_L();//占用总线！
	delay_us(2);
		
	SW_I2C_SDA_L();//sda低电平表示应答！
	delay_us(2);
	
	SW_I2C_SCL_H();
	delay_us(2);	
	SW_I2C_SCL_L();
}



/**
  * @brief	:i2c发送nack信号
  * @note   :--sda高电平，scl下降沿发送，表示应答！
			 --可理解为：主机等待从机操作总线，直到从机拉低sda或等待时间超时，主机确认从机动作！
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_NAck(void)
{
	SW_I2C_SCL_L();//占用总线！
	delay_us(2);
		
	SW_I2C_SDA_H();//sda高电平表示无应答！
	delay_us(2);
	
	SW_I2C_SCL_H();
	delay_us(2);
	SW_I2C_SCL_L();
}			



/**
  * @brief	:发送一个字节数据
  * @note   :--最高位首先发送！
  * @param	:data, 待发送的数据
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Send(uint8_t data)
{                        
	uint8_t i=8;   
	
	while(i--)//高位优先发送
	{
		SW_I2C_SCL_L();
		delay_us(2);
		
		if(data&0x80)
			SW_I2C_SDA_H();
		else
			SW_I2C_SDA_L();
		
		data <<= 1;//sda数据维持！	
		delay_us(2);
		
		SW_I2C_SCL_H();//scl上升沿发送数据！
		delay_us(2);
	}

	SW_I2C_SCL_L();
	SW_I2C_WaitAck();
} 	    



/**
  * @brief	:接收一个字节数据
  * @note   :--最高位首先接收！
  * @param	:ack, (1,ack 0,nack)
  * @return	:uint8_t, 接收的数据
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_Read(uint8_t ack)
{
	uint8_t recData=0x00;
	uint8_t i=8;
		
	ack = ack;
	while(i--)
	{
        SW_I2C_SCL_L();//scl上升沿接收稳定数据！ 
        delay_us(2);
		SW_I2C_SCL_H();
        delay_us(2);	

        recData <<= 1;	
        if(SW_I2C_SDA_READ()) ++recData;   
	}
	
 /* 若外部进行应答，屏蔽掉以下4行！*/
	if(!ack)
		SW_I2C_NAck();//发送nACK
	else
		SW_I2C_Ack(); //发送ACK   
	
	return recData;
}



/**
  * @brief	:i2c向目标地址发送一个字节
  * @note   :--注意：当驱动oled时，需要关闭应答等待！oled不应答！
			 --使用时注意地址问题！i2c地址 or 期间地址！
  * @param	:addr, 目标地址 
 			 reg , 目标寄存器
			 data, 待发送的数据
  * @return	:0, 成功
			 1, 失败
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_SendByte(uint8_t addr, uint8_t reg, uint8_t data)
{
	if(SW_I2C_Start())	return 1;

	SW_I2C_Send(addr);//搜寻目标地址，并等待应答！
//	if(SW_I2C_WaitAck())
//	{
//		SW_I2C_Stop();
//		return 1;
//	}
	
	SW_I2C_Send(reg);//指定目标寄存器
//	SW_I2C_WaitAck();
	
	SW_I2C_Send(data);//发送数据
//	SW_I2C_WaitAck();
	
	SW_I2C_Stop();
	return 0;
}



/**
  * @brief	:i2c从目标地址接收一个字节
  * @note   :--
  * @param	:addr , 目标地址 
 			 reg  , 目标寄存器
			 *data, 接收缓冲区
  * @return	:0, 成功
			 1, 失败
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_ReadByte(uint8_t addr, uint8_t reg, uint8_t *data)
{
	if(SW_I2C_Start())	return 1;
	
	SW_I2C_Send(addr<<1);//搜寻目标地址，并等待应答！
	if(SW_I2C_WaitAck())
	{
		SW_I2C_Stop();
		return 1;
	}
	
	SW_I2C_Send(reg);//指定目标寄存器
	SW_I2C_WaitAck();
	
	SW_I2C_Start();//重新启动一次总线，开始读数据！
	SW_I2C_Send(addr<<1 | 0x01);
	SW_I2C_WaitAck();

	*data = SW_I2C_Read(SW_I2C_NACK);//接收数据
//	SW_I2C_NAck();
	
	SW_I2C_Stop();
	return 0;
}



/**
  * @brief	:i2c向目标地址发送指定长度的数据
  * @note   :--
  * @param	:addr , 目标地址 
 			 reg  , 目标寄存器
			 *buff, 待发送的数据
			 len  , 数据长度
  * @return	:0, 成功
			 1, 失败
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_SendBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	uint8_t i;
	
	if(SW_I2C_Start())	return 1;

	SW_I2C_Send(addr<<1);//搜寻目标地址，并等待应答！
	SW_I2C_WaitAck();
	
	SW_I2C_Send(reg);//指定目标寄存器
	SW_I2C_WaitAck();

	for(i=0; i<len; i++)
	{
		SW_I2C_Send(buff[i]);
		SW_I2C_WaitAck();
	}
	
	SW_I2C_Stop();
	return 0;
}



/**
  * @brief	:i2c从目标地址接收指定长度的数据
  * @note   :--
  * @param	:addr , 目标地址 
 			 reg  , 目标寄存器
			 *buff, 接收缓冲区
			 len  , 数据长度
  * @return	:0, 成功
			 1, 失败
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	if(SW_I2C_Start())	return 1;
	
	SW_I2C_Send(addr<<1);//搜寻目标地址，并等待应答！
	if(SW_I2C_WaitAck())
	{
		SW_I2C_Stop();
		return 1;
	}
	
	SW_I2C_Send(reg);//指定目标寄存器
	SW_I2C_WaitAck();
	
	SW_I2C_Start();//重新启动一次总线，开始读数据！
	SW_I2C_Send(addr<<1 | 0x01);
	SW_I2C_WaitAck();
	while(len)
	{
		if(len == 1)//若主动应答，则一直接收数据！
			*buff = SW_I2C_Read(SW_I2C_NACK);
		else
			*buff = SW_I2C_Read(SW_I2C_ACK);

		++buff;
		len--;
	}
	
	SW_I2C_Stop();
	return 0;
}


void SW_I2C_WrDat(uint8_t dat)
{
	SW_I2C_SendByte(0x78, 0x40, dat);
}

void SW_I2C_WrCmd(uint8_t cmd)
{
	SW_I2C_SendByte(0x78, 0x00, cmd);
}

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










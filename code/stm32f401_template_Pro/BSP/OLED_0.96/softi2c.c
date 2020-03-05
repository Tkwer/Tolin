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



#include "softi2c.h"
#include "delay.h"



/* ���º������Ƽ����ⲿ�ļ���ʹ�ã�����ʹ�ã�����ͨ��extern������*/
uint8_t SW_I2C_Start(void);		//����i2c��ʼ�ź�
void SW_I2C_Stop(void);			//����i2cֹͣ�ź�
uint8_t SW_I2C_WaitAck(void);	//i2c�ȴ�ack�ź�
void SW_I2C_Ack(void);			//i2c����ack�ź�
void SW_I2C_NAck(void);			//i2c����nack�ź�


/**
  * @brief	:��ʼ��i2c��io��
  * @note   :--���ݱ�׼i2cЭ�飬�������óɿ�©ģʽ���������������裡
			 --��������������裬�ɳ����޸ı����򣬵������飡��
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
  * @brief	:����i2c��ʼ�ź�
  * @note   :--scl�ߵ�ƽ�ڼ䣬sda�½��ر�ʾ��ʼ��
			 --�����������ڱ�ռ�ã�������ʼʧ�ܣ�
  * @param	:void
  * @return	:0, �ɹ�
			 1, ʧ��
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_Start(void)
{	
	SW_I2C_SDA_H();//�ͷ�����
	SW_I2C_SCL_H();
	delay_us(2);
	if(!SW_I2C_SDA_READ()) return 1;//�ؼ�һ�ε�ƽ״̬��
	
	SW_I2C_SDA_L();//scl�ߵ�ƽ�ڼ䣬sda�½���Ϊ��ʼ�ź�
	delay_us(2);
	if(SW_I2C_SDA_READ()) return 1;//�ؼ�һ�ε�ƽ״̬��

	delay_us(2);
	return 0;
}



/**
  * @brief	:����i2cֹͣ�ź�
  * @note   :--scl�ߵ�ƽ�ڼ䣬sda�����ر�ʾֹͣ��
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Stop(void)
{
	SW_I2C_SCL_L();//ռ�����ߣ�
	delay_us(2);
	
	SW_I2C_SDA_L();
	SW_I2C_SCL_H();
	delay_us(2);
	
	SW_I2C_SDA_H();//scl�ߵ�ƽ�ڼ䣬sda������Ϊֹͣ�ź�
	delay_us(2);	
}



/**
  * @brief	:i2c�ȴ�ack�ź�
  * @note   :--�ͷ�sda��scl���ȴ��ӻ��������ߣ�
  * @param	:void
  * @return	:1, ����Ӧ��ʧ��
			 0, ����Ӧ��ɹ�
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_WaitAck(void)
{
	uint8_t errDly=100;
		
	SW_I2C_SCL_L();//ռ�����ߣ�
	delay_us(2);
	
	SW_I2C_SDA_H();//�ͷ�sda
	delay_us(1);
	SW_I2C_SCL_H();//�ͷ�scl���ȴ��ӻ����ͣ�
	delay_us(1);
			
	while(SW_I2C_SDA_READ())
	{
		errDly--;
		if(!errDly)//�����������֣����ֵ��-- 2017.12.20 by seri_liang.
		{
			SW_I2C_Stop();
			return 1;
		}
	}	
	
	SW_I2C_SCL_L();//
	return 0;  
}



/**
  * @brief	:i2c����ack�ź�
  * @note   :--sda�͵�ƽ��scl�½��ط��ͣ���ʾӦ��
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Ack(void)
{	
	SW_I2C_SCL_L();//ռ�����ߣ�
	delay_us(2);
		
	SW_I2C_SDA_L();//sda�͵�ƽ��ʾӦ��
	delay_us(2);
	
	SW_I2C_SCL_H();
	delay_us(2);	
	SW_I2C_SCL_L();
}



/**
  * @brief	:i2c����nack�ź�
  * @note   :--sda�ߵ�ƽ��scl�½��ط��ͣ���ʾӦ��
			 --�����Ϊ�������ȴ��ӻ��������ߣ�ֱ���ӻ�����sda��ȴ�ʱ�䳬ʱ������ȷ�ϴӻ�������
  * @param	:void
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_NAck(void)
{
	SW_I2C_SCL_L();//ռ�����ߣ�
	delay_us(2);
		
	SW_I2C_SDA_H();//sda�ߵ�ƽ��ʾ��Ӧ��
	delay_us(2);
	
	SW_I2C_SCL_H();
	delay_us(2);
	SW_I2C_SCL_L();
}			



/**
  * @brief	:����һ���ֽ�����
  * @note   :--���λ���ȷ��ͣ�
  * @param	:data, �����͵�����
  * @return	:void
  *
  * @date   :2017/12/20
  * @design :
  **/
void SW_I2C_Send(uint8_t data)
{                        
	uint8_t i=8;   
	
	while(i--)//��λ���ȷ���
	{
		SW_I2C_SCL_L();
		delay_us(2);
		
		if(data&0x80)
			SW_I2C_SDA_H();
		else
			SW_I2C_SDA_L();
		
		data <<= 1;//sda����ά�֣�	
		delay_us(2);
		
		SW_I2C_SCL_H();//scl�����ط������ݣ�
		delay_us(2);
	}

	SW_I2C_SCL_L();
	SW_I2C_WaitAck();
} 	    



/**
  * @brief	:����һ���ֽ�����
  * @note   :--���λ���Ƚ��գ�
  * @param	:ack, (1,ack 0,nack)
  * @return	:uint8_t, ���յ�����
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
        SW_I2C_SCL_L();//scl�����ؽ����ȶ����ݣ� 
        delay_us(2);
		SW_I2C_SCL_H();
        delay_us(2);	

        recData <<= 1;	
        if(SW_I2C_SDA_READ()) ++recData;   
	}
	
 /* ���ⲿ����Ӧ�����ε�����4�У�*/
	if(!ack)
		SW_I2C_NAck();//����nACK
	else
		SW_I2C_Ack(); //����ACK   
	
	return recData;
}



/**
  * @brief	:i2c��Ŀ���ַ����һ���ֽ�
  * @note   :--ע�⣺������oledʱ����Ҫ�ر�Ӧ��ȴ���oled��Ӧ��
			 --ʹ��ʱע���ַ���⣡i2c��ַ or �ڼ��ַ��
  * @param	:addr, Ŀ���ַ 
 			 reg , Ŀ��Ĵ���
			 data, �����͵�����
  * @return	:0, �ɹ�
			 1, ʧ��
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_SendByte(uint8_t addr, uint8_t reg, uint8_t data)
{
	if(SW_I2C_Start())	return 1;

	SW_I2C_Send(addr);//��ѰĿ���ַ�����ȴ�Ӧ��
//	if(SW_I2C_WaitAck())
//	{
//		SW_I2C_Stop();
//		return 1;
//	}
	
	SW_I2C_Send(reg);//ָ��Ŀ��Ĵ���
//	SW_I2C_WaitAck();
	
	SW_I2C_Send(data);//��������
//	SW_I2C_WaitAck();
	
	SW_I2C_Stop();
	return 0;
}



/**
  * @brief	:i2c��Ŀ���ַ����һ���ֽ�
  * @note   :--
  * @param	:addr , Ŀ���ַ 
 			 reg  , Ŀ��Ĵ���
			 *data, ���ջ�����
  * @return	:0, �ɹ�
			 1, ʧ��
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_ReadByte(uint8_t addr, uint8_t reg, uint8_t *data)
{
	if(SW_I2C_Start())	return 1;
	
	SW_I2C_Send(addr<<1);//��ѰĿ���ַ�����ȴ�Ӧ��
	if(SW_I2C_WaitAck())
	{
		SW_I2C_Stop();
		return 1;
	}
	
	SW_I2C_Send(reg);//ָ��Ŀ��Ĵ���
	SW_I2C_WaitAck();
	
	SW_I2C_Start();//��������һ�����ߣ���ʼ�����ݣ�
	SW_I2C_Send(addr<<1 | 0x01);
	SW_I2C_WaitAck();

	*data = SW_I2C_Read(SW_I2C_NACK);//��������
//	SW_I2C_NAck();
	
	SW_I2C_Stop();
	return 0;
}



/**
  * @brief	:i2c��Ŀ���ַ����ָ�����ȵ�����
  * @note   :--
  * @param	:addr , Ŀ���ַ 
 			 reg  , Ŀ��Ĵ���
			 *buff, �����͵�����
			 len  , ���ݳ���
  * @return	:0, �ɹ�
			 1, ʧ��
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_SendBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	uint8_t i;
	
	if(SW_I2C_Start())	return 1;

	SW_I2C_Send(addr<<1);//��ѰĿ���ַ�����ȴ�Ӧ��
	SW_I2C_WaitAck();
	
	SW_I2C_Send(reg);//ָ��Ŀ��Ĵ���
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
  * @brief	:i2c��Ŀ���ַ����ָ�����ȵ�����
  * @note   :--
  * @param	:addr , Ŀ���ַ 
 			 reg  , Ŀ��Ĵ���
			 *buff, ���ջ�����
			 len  , ���ݳ���
  * @return	:0, �ɹ�
			 1, ʧ��
  *
  * @date   :2017/12/20
  * @design :
  **/
uint8_t SW_I2C_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	if(SW_I2C_Start())	return 1;
	
	SW_I2C_Send(addr<<1);//��ѰĿ���ַ�����ȴ�Ӧ��
	if(SW_I2C_WaitAck())
	{
		SW_I2C_Stop();
		return 1;
	}
	
	SW_I2C_Send(reg);//ָ��Ŀ��Ĵ���
	SW_I2C_WaitAck();
	
	SW_I2C_Start();//��������һ�����ߣ���ʼ�����ݣ�
	SW_I2C_Send(addr<<1 | 0x01);
	SW_I2C_WaitAck();
	while(len)
	{
		if(len == 1)//������Ӧ����һֱ�������ݣ�
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










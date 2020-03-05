/**
  ******************************************************************************
  * 
  * OLED 0.96����ʾ�������ļ�����ʹ�����i2c�����spi��Ĭ��Ϊ���i2c
  * 
  * ���ļ�������������ʾ�����㡢���ߡ���ͼ���ܼ�������������ʾ(������С��)�ȣ�
  * ������Ĭ��ʹ��i2cЭ�飬��ͨ���궨���޸�ΪspiЭ�飬���ṩ�����ַ�ʽ�Ŀ��ƣ�
  * ������ΪGUI������񣬳�ѧϰ�ⲻ����Ϊ���ã�ͬʱ��ӭ���ϲ��书�ܣ�
  * 
  * 
  * ʹ��ע�⣺
  * 	1.����ָ��������������ʹ�õĵײ���������i2c��spi��ʵ�ַ�ʽ��
  *		2.ע��oled��ʹ�õ�spiЭ�鲻ͬ��һ��spi����Ҫ�������ã��ɲο�oledspi.c/.h��
  *
  * �汾�޶���
  *		�޸�ʱ��	�汾��	�޸�����
  *		2016-09-09	v1.0	֧�����ģ��I2C����Э�飻
  *		2016-12-24	v2.0	��������(��������)��ָ������ɫ���Գ�(����)������
  *		2016-12-24	v3.0	����6*8���壻
  *		2017-01-02	v4.0	�����е㻭Բ�㷨�͸Ľ��е㻭Բ�㷨��
  *		2017-01-04	v4.1	�����е㻭��Բ�㷨�͸Ľ��е㻭��Բ�㷨��
  *	 // 2017-10-15	v5.0	�޸������ܣ�����STM32.F4XX��NXP.K60��Ƭ���Ŀ��Ʋ�����
  *	 // 2017-12-10	v6.0	���Ӻ��ֿ⣻
  *		2017-12-21	v7.0	1.�ع���ܣ�ʹ���������ꣻ
  *							2.�����ļ����ֽ⿪��������ѡ��ײ��������ͣ�
  *							3.���ֺ�������֧�֣����������Σ�������ʵ�֣�
  *							4.ȡ���Է�ҳѰַ��ʽ��֧�֣�
  *		2017-12-26	v8.0	�ض���printf��oled��Ļ����ʾ����ģʽ�Զ����У�
  *							�˲�����Ҫ����ENABLE_PRINTF_SUPPORT��
  *		2017-12-30	v9.0	1.���Ӷ�printf����ʾ��λ�ò���Խ�����ݴ���ƣ�
  *							2.����Ӳ����������������֮ǰ�������������ʽ֧�֣�
  *		2018-01-19	v10.0	1.�Ż�Ӳ������������֧�ִ�ֱ������
  *                         2.�Ż��������������
  *							3.�ָ�ˮƽ����ֱѰַ��֧�֣�
  *		2018-01-21	v10.1	1.�޸�����OLED_DisplayFLOAT()�������³�������bug��
  *							2.��Ӷ��㺯����OLED_ReadPixelPoint();
  *							3.���Ӷ�emWin v532��֧�֣�
  *		2018-01-22	v10.2	�Ż�printf�ض������ʾ��Ҫ"\\"������ʾ�����⣻
  *
  * ���������
  *		Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI
  * 	All rights reserved			
  *
  *
  *	Create  ��2016��09��09��
  * Update  ��2018��01��22��
  * Author  ��������(SERI.LJI)
  * Email	��liangzongnan0214@163.com
  * Blog    ��liangzongnan.com
  * QQ		��494089986
  * 
  ******************************************************************************
***/



#include "Task_oled.h"
#include "oledfont.h"


/* 0.96 oled������SSD1306�Ĵ������õ���*/
const uint8_t oled_ctl_cmd[]=
{
	0xAE,	//Turn off oled panel
	
	0x20,	//set scan mode and start&end address
	0x02,
	0x21,
	0,
	127,
	0x22,
	0,
	7,
	
	0x00,	//Set low column address
	0x10,	//Set high column address
	0x40,	//Set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	0x81,	//Set contrast control register
	0xCF,	//Set SEG Output Current Brightness
	0xA1,	//Set SEG/Column Mapping			0xA0���ҷ��� 0xA1����
	0xC8,	//Set COM/Row Scan scanDirection  	0xC0���·��� 0xC8����
	0xA6,	//Set normal display
	0xA8,	//Set multiplex ratio(1 to 64)
	0x3F,	//Duty 1/64
	0xD3,	//Set display offset,Shift Mapping RAM Counter (0x00~0x3F)
	0x00,	//Not offset
	0xD5,	//Set display clock divide ratio/oscillator frequency
	0x80,	//Set divide ratio, Set Clock as 100 Frames/Sec
	0xD9,	//Set pre-charge period
	0xF1,	//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	0xDA,	//Set com pins hardware configuration
	0x12,
	0xDB,	//Set vcomh
	0x40,	//Set VCOM Deselect Level

	0x8D,	//Set Charge Pump enable/disable
	0x14,	//Set(0x10) disable
	0xA4,	//Disable Entire Display On  (0xA4/0xA5)
	0xA6,	//Disable Inverse Display On (0xA6/0xA7) 
	0xAF,	//Turn on oled panel
};     
  
  
/* oled�Դ�ռ����ã�*/
//OLED���Դ�(��OLED_ORIGIN_SET�޹�)
// 	   [0]:	 0 1 2 3 ... 127	
//	   [1]:	 0 1 2 3 ... 127	
//	   [2]:	 0 1 2 3 ... 127	
//	   [3]:	 0 1 2 3 ... 127	
//	   [4]:	 0 1 2 3 ... 127	
//	   [5]:	 0 1 2 3 ... 127	
//	   [6]:	 0 1 2 3 ... 127	
//	   [7]:	 0 1 2 3 ... 127 
#if OLED_GRAM_SIZE == 0	
	uint8_t OLED_GRAM[128][8]={0};//oled �Դ�(����ģʽ)
#else
	uint8_t OLED_GRAM[256][16]={0};//oled �Դ�(��չģʽ)
#endif


/* oledȫ�ֿ��Ʊ�����*/
#ifndef ENABLE_EMWIN_SUPPORT					//���±����ڷ�emWin��ʹ�ã�
static uint32_t oled_display_cover_mode  = 1;	//�ַ�����ʾģʽ(Ĭ��1��������ʾ)
static uint32_t oled_display_font_width  = 8;	//�ַ�����ʾ�����С(Ĭ��8��1608����)
static uint32_t oled_display_font_height = 16;	//�ַ�����ʾ�����С(Ĭ��16��1608����)
#endif 	
	
	
/* ------------------------   �������������  ------------------------- */
#include "delay.h"

#define oled_delay_us(t)				delay_us(t)					
#define oled_delay_ms(t)				delay_ms(t)	

/* ------------------------   ��ʼ����������  ------------------------- */
#include "softi2c.h"
	
#define oled_dev_init()						SW_I2C_InitConfig()	
#define oled_write_data(dat)			SW_I2C_WrDat(dat)
#define oled_write_command(cmd)		SW_I2C_WrCmd(cmd)	
	
	
	
/**
  * @brief	:printf����
  * @note   :--����printf�������룬�û�ֱ�ӵ���printf��
			 --�Ż�printf�ض������ʾ��Ҫ"\\"������ʾ�����⣻
  * @date   :2018/01/22
  * @design :
  **/
#ifdef ENABLE_PRINTF_SUPPORT   
	#pragma import(__use_no_semihosting)             
	struct __FILE 
	{ 
		int handle; 
		
	}__stdout;				//��׼����Ҫ��֧�ֺ���                 
	 
	void _sys_exit(int x) 	//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
	{ 
		x = x; 
	} 

	static uint32_t poled_pixelx=0;
	static uint32_t poled_pixely=0;
	void oled_goto_xy(uint32_t x, uint32_t y)
	{			
		if(x > (OLED_SCREEN_WIDTH - oled_display_font_width) 
		|| y > (OLED_SCREEN_HEIGHT - oled_display_font_height)) return;//Խ������ǰһ���趨��ĩβ������ʾ��

		poled_pixelx = x;
		poled_pixely = y;
	}

	///ע�⣺����printf()��ֱ�Ӹ�����ʾ��
	///�ݲ�֧�ֿ����ַ�"\r"��"\n"�ȵ����룡
	int fputc(int chda, FILE *f)
	{			
		OLED_DisplayCHAR(poled_pixelx, poled_pixely, (char)chda);
		
		poled_pixelx +=  oled_display_font_width;	
		if((OLED_SCREEN_WIDTH - poled_pixelx) > oled_display_font_width)//ʣ�಻����ʾһ���ַ���ֱ�ӻ�����ʾ��
		{
			poled_pixely += oled_display_font_height;
			poled_pixelx  = 0;
		}		
		return chda;
	}
#endif



/**
  * @brief	:oled��ʾ��ʼ������
  * @note   :--�޸ģ���֧��ҳѰַģʽ��
			 --����Ĭ��ʹ��ҳѰַ����ʼ��ʱ������չ��������Ļ��
			 --�޸ģ��Ծֲ����ݸ�ʽ��������������δ�䣡-- v10.1
			 --�������ʹ��printf�ض�������ʼ���ʼ����-- v10.2
  * @param	:void
  * @return	:void
  *
  * @date   :2018/01/22
  * @design :
  **/
void OLED_InitConfig(void)
{
	uint32_t i=0;
	
	oled_dev_init();	
	oled_delay_ms(100);
	
	///����ssd1306�Ĵ�����
	for(i=0; i<sizeof(oled_ctl_cmd); i++){
		oled_write_command(oled_ctl_cmd[i]);
	}
	
#ifdef ENABLE_PRINTF_SUPPORT
	///ʹ��printf�ض���ʱ����Ҫ������ʾ��ʼ�㣡
	oled_goto_xy(0, 0);
#endif
	
	OLED_ClearScreen();//��ʼ��������
}
	


/**
  * @brief	:����oled��ʾ 
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DisplayOn(void)
{
	oled_write_command(0x8D);//Set DCDC command
	oled_write_command(0x14);//DCDC ON
	oled_write_command(0xAF);//display ON
}



/**
  * @brief	:�ر�oled��ʾ  
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DisplayOff(void)
{
	oled_write_command(0x8D);//Set DCDC command
	oled_write_command(0x10);//DCDC OFF
	oled_write_command(0xAE);//display OFF
}		   			 



/**
  * @brief	:oled������ʾ����(ҳ����)
  * @note   :--��ҳѰַģʽ�¿�ֱ��ʹ�ã�����ģʽ��Ҫ�������ã�
  * @param	:page  , ҳ����
			 column, ������
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_SetPos(uint32_t page, uint32_t column)
{	
	oled_write_command(0xB0 +  page);			//ҳ��ʾλ��
	oled_write_command(0x10 + (column&0xF0)>>4);//����ʾλ��
	oled_write_command(0x00 + (column&0x0F));	//
}



/**
  * @brief	:oled������ʾ
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_Update(void)
{
	uint32_t column=0, page=0;
	
	for(page=0; page<8; page++)
	{		
		OLED_SetPos(page, 0);//���趨ҳ��ַ���ɣ�			
		for(column=0; column<128; column++){		
			oled_write_data(OLED_GRAM[column][page]);
		}
	}
}



/**
  * @brief	:oled����
  * @note   :--
  * @param	:void
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_ClearScreen(void)
{
	uint32_t column=0, page=0;
		
	for(page=0; page<8; page++)
	{		
		for(column=0; column<128; column++){
			OLED_GRAM[column][page] = 0x00;
		}
	}
	OLED_Update();
}



/**
  * @brief	:�����ص�(��������)
  * @note   :--
  * @param	:pixelX   , ������
			 pixelY   , ������
			 writeMode, ģʽ(0��ɫ 1����)
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawPixelPoint(uint32_t pixelX, uint32_t pixelY, uint32_t writeMode)
{
	uint32_t page=0, page_data=0;
	uint8_t dram_data=0;
	
#if OLED_ORIGIN_SET == 0
    page	  = 7 - pixelY/8;
    page_data = pixelY%8; 
    dram_data = 1<<(7 - page_data);
#else 
    page	  = pixelY/8;
    page_data = pixelY%8; 
    dram_data = 1<<page_data;
#endif
	
    if(writeMode)
	{
		OLED_GRAM[pixelX][page] |=   dram_data;    
	}else{
		OLED_GRAM[pixelX][page] &= (~dram_data);    
	}	
	
#if defined (ENABLE_EMWIN_SUPPORT) || defined (ENABLE_PRINTF_SUPPORT)	
	///����ɨ�贰�ڴ�СΪ���±ʵ㵽��Ļ�Խǵ㣡
	oled_write_command(0x20);	
	oled_write_command(0x00);
	oled_write_command(0x21);	
	oled_write_command(pixelX);
	oled_write_command(127);
	oled_write_command(0x22);
	oled_write_command(page);
	oled_write_command(7);

	OLED_SetPos(page, pixelX);			
	oled_write_data(OLED_GRAM[pixelX][page]);	
#endif	
}



/**
  * @brief	:�����ص�(��������)
  * @note   :--����֧��emWin���ã�
  * @param	:pixelX, ������
			 pixelY, ������
  * @return	:uint8_t, ��ȡ�������ɫֵ
  *
  * @date   :2018/01/21
  * @design :
  **/
uint8_t OLED_ReadPixelPoint(uint32_t pixelX, uint32_t pixelY)
{
	uint32_t page=0, page_data=0;
	uint8_t temp=0;
	
#if OLED_ORIGIN_SET == 0
    page	  = 7 - pixelY/8;
    page_data = pixelY%8; 
#else 
    page	  = pixelY/8;
    page_data = pixelY%8; 
#endif
	
	temp = OLED_GRAM[pixelX][page];
	if(temp & page_data)
	{
		return 1;
	}else{
		return 0;
	}
}



#ifndef ENABLE_EMWIN_SUPPORT
/**
  * @brief	:oled�����������(��������)
  * @note   :--ע�⣺������Ҫ���ⲿ���ñ��������޲�����飡
  * @param	:xStart, ����ʼ����
			 yStart, ����ʼ����
			 xEnd  , ����ֹ����
			 yEnd  , ����ֹ����
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle oled_zone(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd)
{
	_OLEDZoneHandle oled_coor={0, 0, 0, 0};
	
	oled_coor.pixel_row_start		= xStart;
	oled_coor.pixel_column_start	= yStart;
	oled_coor.pixel_row_end			= xEnd;
	oled_coor.pixel_column_end		= yEnd;
	return oled_coor;
}

    

/**
  * @brief	:oled�ֲ�������ʾ
  * @note   :--v10.0��ʼ������ʹ�ñ�������
			 --����̫�Ͻ�������������ҳѰַģʽ��-- by liang_seri 2018.01.21
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
			 direct, ѡ��ˢ�·���(1����  0����)
  * @return	:void
  *
  * @date   :2019/01/19
  * @design :
  **/
void OLED_UpdatePart(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd, uint32_t direct)
{
	uint32_t column=0, page=0;

    if(direct)						//����ˢ��
    {        
        oled_write_command(0x20);	//����ΪˮƽѰַģʽ����ΪҳѰַ�µ��ƶ�������λ����(ˢ��ʱ��̫����)	
        oled_write_command(0x00);
        oled_write_command(0x21);	//����ˮƽѰַ��ʼ�к���ֹ��
        oled_write_command(cStart);
        oled_write_command(cEnd);
        oled_write_command(0x22);	//����ˮƽѰַ��ʼҳ����ֹҳ
        oled_write_command(pStart);
        oled_write_command(pEnd);

        for(page=pStart; page<=pEnd; page++)
        {		
            for(column=cStart; column<=cEnd; column++){		
                oled_write_data(OLED_GRAM[column][page]);
            }
        }
    }else{							//����ˢ��
        oled_write_command(0x20);	//����Ϊ��ֱѰַģʽ����ΪҳѰַ�µ��ƶ�������λ����(ˢ��ʱ��̫����)	
        oled_write_command(0x01);
        oled_write_command(0x21);	//���ô�ֱѰַ��ʼ�к���ֹ��
        oled_write_command(cStart);
        oled_write_command(cEnd);
        oled_write_command(0x22);	//���ô�ֱѰַ��ʼҳ����ֹҳ
        oled_write_command(pStart);
        oled_write_command(pEnd);
        
		for(column=cStart; column<=cEnd; column++){		
        {		
			for(page=pStart; page<=pEnd; page++)
                oled_write_data(OLED_GRAM[column][page]);
            }
        }        
    }
    
    oled_write_command(0x20);		//�ָ�Ĭ��Ѱַģʽ��ҳѰַģʽ��
    oled_write_command(0x02);
	oled_write_command(0x21);		//������ʼ�к���ֹ��
	oled_write_command(0);
	oled_write_command(127);
	oled_write_command(0x22);		//������ʼҳ����ֹҳ
	oled_write_command(0);
	oled_write_command(7);
}



/**
  * @brief	:����(��������)
  * @note   :--
  * @param	:pixelX   , ������
			 pixelY   , ������
			 fontSize , ���С
			 writeMode, ģʽ(0��ɫ  1����)
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawPoint(uint32_t pixelX, uint32_t pixelY, uint32_t size, uint32_t writeMode)
{
	uint32_t row=0, column=0;
	
	for(row=0; row<size; row++)
	{
		for(column=0; column<size; column++){
			OLED_DrawPixelPoint(pixelX+column, pixelY+row, writeMode);
		}
	}
}



/**
  * @brief	:����(�������꣬����Ϊ���㣬������)
  * @note   :--
  * @param	:xStart, ����ʼ����(0~127)
			 yStart, ����ʼ����(0~63)
			 xEnd  , ����ֹ����(0~127)
			 yEnd  , ����ֹ����(0~63)
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd)
{
	int8_t  x_width=0;	//x���� 
	int8_t  y_height=0;	//y��߶�
	int8_t  x_inc=0;	//x�����������
	int8_t  y_inc=0;	//y�����������
	int8_t  rem;		//current remainder 
	uint8_t start=0, end=0;
	uint8_t i=0;
		
	if(yStart == yEnd)//����ˮƽ�ߣ�horizon line
	{ 
		if(xStart > xEnd)
		{
			start = xEnd;
			end   = xStart;
		}else{
			start = xStart;
			end	  = xEnd;
		}
		
		for(i=start; i<=end; i++){ 
			OLED_DrawPixelPoint(i, yStart, 1);
        }
	}else if(xStart == xEnd){//���ƴ�ֱ�ߣ�vertical line
		if(yStart > yEnd) 
		{
			start = yEnd;
			end   = yStart;
		}else{
			start = yStart;
			end   = yEnd;
		}
		
		for(i=start; i<=end; i++){
			OLED_DrawPixelPoint(xStart, i, 1);
        }
	}else{//��������ֱ��
		x_width  = xEnd - xStart;
		y_height = yEnd - yStart;
		
		if(x_width  < 0) x_width  = 0 - x_width;
		if(y_height < 0) y_height = 0 - y_height;
		
		x_inc = (xEnd > xStart) ? 1 : -1;
		y_inc = (yEnd > yStart) ? 1 : -1;
				
		if(x_width >= y_height)
		{
			rem = x_width/2;
			for(; xStart!=xEnd; xStart+=x_inc)
			{
				OLED_DrawPixelPoint(xStart, yStart, 1);
				
				rem += y_height;
				if(rem >= x_width)
				{
                    rem		-= x_width;
                    yStart	+= y_inc;
				}
			}
		}else{
			rem = y_height/2;
			for(; yStart!=yEnd; yStart+=y_inc)
			{
				OLED_DrawPixelPoint(xStart, yStart, 1);

				rem += x_width;
				if(rem >= y_height)
				{
					rem		-= y_height;
					xStart	+= x_inc;
				}
			}
		}
	}
}



/**
  * @brief	:�˷ֶԳƷ�(��������)
  * @note   :--����������İ˷ֶԳƵ�(��Բ�����㷨)
  * @param	:xc, Բ��������
			 yc, Բ��������
             x , ������
			 y , ������
  * @return	:void
  *
  * @date   :2017/01/02
  * @design :
  **/
static void Circle8Point(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y)
{
    //ֱ������ϵ��һ����x�Ὺʼ����ʱ����ת��
    OLED_DrawPixelPoint((xc+x), (yc+y), 1);//1
    OLED_DrawPixelPoint((xc+y), (yc+x), 1);//2
    OLED_DrawPixelPoint((xc-y), (yc+x), 1);//3
    OLED_DrawPixelPoint((xc-x), (yc+y), 1);//4
    OLED_DrawPixelPoint((xc-x), (yc-y), 1);//5
    OLED_DrawPixelPoint((xc-y), (yc-x), 1);//6
    OLED_DrawPixelPoint((xc+y), (yc-x), 1);//7
    OLED_DrawPixelPoint((xc+x), (yc-y), 1);//8
}



/**
  * @brief	:�Ľ���Բ(��������)
  * @note   :--���⸡������(���ϵ㲻ͻ����)��
  * @param	:xc, Բ��������
			 yc, Բ��������
			 r , �뾶
  * @return	:void
  *
  * @date   :2017/01/02
  * @design :
  **/
void OLED_DrawCircle(uint32_t xc, uint32_t yc, uint32_t r)
{
    uint32_t x=0, y=0;
    int32_t d=0;//�Ľ������⸡�����㣡
        
    x = 0;
    y = r;
    d = 3-2*r;
    
    Circle8Point(xc ,yc, x, y);
    while(x < y)
    {
        if(d < 0)
        {
            d += 4*x+6;
        }else{
            d += 4*(x-y)+10;
            --y;
        } 
        ++x;
        Circle8Point(xc, yc, x, y);
    }
}



/**
  * @brief	:�ķֶԳƷ�(��������)
  * @note   :--������������ķֶԳƵ�(����Բ�����㷨)
  * @param	:xc, ��Բ����������
			 yc, ��Բ����������
             x , ������
			 y , ������
  * @return	:void
  *
  * @date   :2017/01/04
  * @design :
  **/
static void Ellipse4Point(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y)
{
    //ֱ������ϵ��һ���޿�ʼ����ʱ����ת��
    OLED_DrawPixelPoint((xc+x), (yc+y), 1);//1
    OLED_DrawPixelPoint((xc-x), (yc+y), 1);//2
    OLED_DrawPixelPoint((xc-x), (yc-y), 1);//3
    OLED_DrawPixelPoint((xc+x), (yc-y), 1);//4
}



/**
  * @brief	:����Բ(��������)
  * @note   :--
  * @param	:xc, ��Բ����������
			 yc, ��Բ����������
             a , �볤�᳤��
			 b , ����᳤��
  * @return	:void
  *
  * @date   :2017/01/04
  * @design :
  **/
void OLED_DrawEllipse(uint32_t xc, uint32_t yc, uint32_t a, uint32_t b)
{
    int32_t x=0;
    int32_t y=b;
	int32_t b2=(int32_t)b;
	
    float sqa=a*a;
    float sqb=b*b;    
    float d=sqb+sqa*(-b2+0.25f);
    	
    Ellipse4Point(xc, yc, x, y);
    while((sqb*(x+1)) < (sqa*(y-0.5f)))
    {
        if(d < 0)
        {
            d += sqb*(2*x+3);
        }else{
            d += sqb*(2*x+3)+sqa*(-2*y+2); 
            --y;
        }
        ++x;
        Ellipse4Point(xc, yc, x, y);
    }
    
	d = (b*(x+0.5))*2 + (a*(y-1))*2 - (a*b)*2;
    while(y > 0)
    {
        if(d < 0)
        {
            d += sqb*(2*x+2)+sqa*(-2*y+3);
            ++x;
        }else{
            d += sqa*(-2*y+3);
        }
        --y;
        Ellipse4Point(xc, yc, x, y);
    }
}



/**
  * @brief	:������(��������)
  * @note   :--
  * @param	:xStart, ����ʼ����
			 yStart, ����ʼ����
			 xEnd  , ����ֹ����
			 yEnd  , ����ֹ����
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawBlock(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd)
{
	OLED_DrawLine(xStart, yStart, xStart, yEnd);//���
	OLED_DrawLine(xEnd, yStart, xEnd, yEnd);//�ҽ�
	OLED_DrawLine(xStart, yStart, xEnd, yStart);//�Ͻ�
	OLED_DrawLine(xStart, yEnd, xEnd, yEnd);//�½�
}



/**
  * @brief	:������(��������)
  * @note   :--
  * @param	:xStart , ����ʼ����
			 yStart , ����ʼ����
			 length , ���γ���
			 width  , ���ο��
			 isFill , �Ƿ����(0����� 1���)
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawRect(uint32_t xStart, uint32_t yStart, uint32_t length, uint32_t width, uint32_t isFill)
{
	uint32_t rect_xs=xStart;
	uint32_t rect_ys=yStart;
	uint32_t rect_xe=xStart+length-1;
	uint32_t rect_ye=yStart+width-1;
	uint32_t x=0, y=0;
	
	OLED_DrawBlock(rect_xs, rect_ys, rect_xe, rect_ye);//���Ʊ߿�
	
	if(isFill)//�ж��Ƿ����
	{
		for(x=xStart; x<(rect_xe+1); x++)
		{
			for(y=yStart; y<(rect_ye+1); y++){
				OLED_DrawPixelPoint(x, y, 1);
			}
		}
	}
}	



/**
  * @brief	:����display�ຯ������ʾģʽ
  * @note   :--
  * @param	:mode, ��ʾģʽ(_OLEDDisplayModeö���ж������ͣ�)
  * @return	:void
  *
  * @date   :2017/12/21
  * @design :
  **/
void OLED_SetDisplayMode(_OLEDDisplayMode mode)
{
	oled_display_cover_mode = (mode != DISP_ANTI) ? 1 : 0;
}



/**
  * @brief	:����display�ຯ������ʾ�����С
  * @note   :--����Ч���壬��ʹ��1608���壡
  * @param	:fontSize, �����С(_OLEDFontSizeö���ж������ͣ�)
  * @return	:void
  *
  * @date   :2017/12/21
  * @design :
  **/
void OLED_SetFontSize(_OLEDFontSize fontSize)
{
	switch(fontSize)
	{
		case FONT_0806	: oled_display_font_width = 6, oled_display_font_height = 8 ; break;
		case FONT_1608	: oled_display_font_width = 8, oled_display_font_height = 16; break;
		
		default			: oled_display_font_width = 8, oled_display_font_height = 16; break;
	}
}



/**
  * @brief	:��ʾASCII��׼�����ַ�(��������)
  * @note   :--
  * @param	:pixelX, ����ʼ����
			 pixelY, ����ʼ����
			 CHAR  , ����ʾ���ַ�
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayCHAR(uint32_t pixelX, uint32_t pixelY, char CHAR)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};
	
	uint32_t i=0, j=0, x0=pixelX, y0=pixelY;
	uint32_t font_ram_size=0;
	uint8_t temp_data=0;
		
	///��ȡָ�������ֿ�����ռ�ֽ�����	
	zone_temp.pixel_row_start	 = pixelX;
	zone_temp.pixel_column_start = pixelY;
	switch(oled_display_font_height)//oledfont.h��ÿһ�б�ʾһ���ַ�����ʾ���룡��ȡ��ʾ����ʹ�ã�
	{
		case 8: {
			zone_temp.pixel_row_end		= pixelX+5;
			zone_temp.pixel_column_end	= pixelY+7;			
			font_ram_size = 6; 
			break;
		}
		
		case 16: {
			zone_temp.pixel_row_end		= pixelX+7;
			zone_temp.pixel_column_end	= pixelY+15;			
			font_ram_size = 16;  
			break;
		}
	}
	
	///д���Դ棺 	
	for(i=0; i<font_ram_size; i++)//���ֽ���
	{
		if(i == oled_display_font_width)//����0806��������壬��Ҫ��ҳ�洢��
		{
			pixelX  = x0;
			pixelY += 8;
			y0	    = pixelY; 
		}

		switch(oled_display_font_height)//ѡ���ֿ�
		{
			case 8  : temp_data = asc_oled_0806[CHAR-0x20][i]; break;
			case 16 : temp_data = asc_oled_1608[CHAR-0x20][i]; break;				
		}

		for(j=0; j<8; j++)
		{
			if(temp_data&0x01)//�˴�����ģ��ȡ�����йأ���ģ��ַ�Ͷ�Ӧҳ�еĵ�λ�����к���ȡģ��
				OLED_DrawPixelPoint(pixelX, pixelY,  oled_display_cover_mode);
			else
				OLED_DrawPixelPoint(pixelX, pixelY, !oled_display_cover_mode);
			
			temp_data >>= 1;
			++pixelY;		
		}	
		++pixelX;//��һ�У�
		pixelY = y0;//�и�λ��
	}
	
	///����д�������Χ��
    return zone_temp;    
}



/**
  * @brief	:��ʾASCII��׼�����ַ���(��������)
  * @note   :--��׼ASCII�ַ�������'\0'��β��
  * @param	:pixelX  , ����ʼ����
			 pixelY  , ����ʼ����
			 *STR    , ����ʾ���ַ���ָ��
             fontSize, �����С(6Ϊ6*8��С 8Ϊ8*16��С)
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplaySTR(uint32_t pixelX, uint32_t pixelY, char *STR)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	uint32_t str_num=0;
	uint32_t i=0, j=0;
	
	while(*(STR+j))
	{
		++str_num;
		++j;
	}
	
	for(i=0; i<str_num; i++){
		OLED_DisplayCHAR(pixelX+i*oled_display_font_width, pixelY, *(STR+i));
	}
	
	///����д�������Χ��
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + str_num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}



/**
  * @brief	:��ʾת�����16��������(��������)
  * @note   :--�޸ģ�ȡ����2λ��ʾģʽ����֧����ʾ4λHEX��
  * @param	:axisX, ����ʼ����
			 axisY, ����ʼ����
			 NUM  , ����ʾ������(ʮ����)
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayHEX(uint32_t axisX, uint32_t axisY, uint16_t NUM)
{	
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	uint32_t i=0;
	uint8_t hex_num[5]={0};//�����һ���ֽڴ��"\0"
	
	for(i=0; i<4; i++)
	{
		switch((NUM>>(12-4*i))&0x000F)//�Ӹ�λ����λ����ת��Ϊ�ַ���
		{
			case  0: hex_num[i] = '0'; break;
			case  1: hex_num[i] = '1'; break;
			case  2: hex_num[i] = '2'; break;
			case  3: hex_num[i] = '3'; break;
			case  4: hex_num[i] = '4'; break;
			case  5: hex_num[i] = '5'; break;
			case  6: hex_num[i] = '6'; break;
			case  7: hex_num[i] = '7'; break;
			case  8: hex_num[i] = '8'; break;
			case  9: hex_num[i] = '9'; break;
			case 10: hex_num[i] = 'A'; break;
			case 11: hex_num[i] = 'B'; break;
			case 12: hex_num[i] = 'C'; break;
			case 13: hex_num[i] = 'D'; break;
			case 14: hex_num[i] = 'E'; break;
			case 15: hex_num[i] = 'F'; break;
		} 	
	}
	
	///����д�������Χ��
	zone_temp = OLED_DisplaySTR(axisX, axisY, (char *)hex_num);//��ʾת�����16��������
	return zone_temp;
}



/**
  * @brief	:��ʾ�޷��ŵ�10��������(��������)
  * @note   :--
  * @param	:pixelX, ����ʼ����
			 pixelY, ����ʼ����
			 NUM   , ����ʾ������
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayUINT(uint32_t pixelX, uint32_t pixelY, uint32_t NUM)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	uint32_t i=0, num, n=0;
	uint8_t res[OLED_MAX_LENGTH_OF_DEC]={0};//�����ʾOLED_MAX_LENGTH_OF_DECλ����

	if(NUM == 0)//����0Ϊ����������������ǣ�
	{
		res[i++] = 0+0x30;//0x30��'0'
	}else{
		while((NUM/10) || (NUM%10))
		{  
			res[i++] = NUM%10+0x30;
			NUM /= 10;  
		} 
	}

	num = i;//��ʾ��jλ10����
	while(i--)
	{  
		OLED_DisplayCHAR(pixelX+i*oled_display_font_width, pixelY, (char)(res[n++]));
	}

	///����д�������Χ��	
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}

	
	
/**
  * @brief	:��ʾ�з��ŵ�10��������(��������)
  * @note   :--
  * @param	:pixelX, ����ʼ����
			 pixelY, ����ʼ����
			 NUM   , ����ʾ������
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayINT(uint32_t pixelX, uint32_t pixelY, int32_t NUM)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};
	
	if(NUM < 0)//��Ϊ������
	{
		OLED_DisplayCHAR(pixelX, pixelY , '-');//��ʾ����
		zone_temp = OLED_DisplayUINT(pixelX+8, pixelY, (0 - NUM));
		zone_temp.pixel_row_start = pixelX;//��ǰ����'-'���ȥ��
	}else{
		zone_temp = OLED_DisplayUINT(pixelX, pixelY, NUM);
	}

	return zone_temp;
}



/**
  * @brief	:��ʾ�ɱ侫�ȵ�С��(��������)
  * @note   :--v10.1 �޸�������ʾ���³������bug��
  * @param	:pixelX, ����ʼ����
			 pixelY, ����ʼ����
			 NUM   , ����ʾ������
			 accu  , ��ȷλ��
  * @return	:_OLEDZoneHandle(OLED_GRAM����)
  *
  * @date   :2018/01/21
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayFLOAT(uint32_t pixelX, uint32_t pixelY, float NUM, uint32_t accu)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	float deci=0.0f;//С��
	uint32_t inte=0;//����

	uint32_t i=0, num=0, temp_i=0;
	uint8_t res[OLED_MAX_LENGTH_OF_DEC];//�����ʾOLED_MAX_LENGTH_OF_DECλ�����������ź�С���㣡
	uint8_t temp[OLED_MAX_LENGTH_OF_DEC-2];
	
 
	///�ж��Ƿ�Ϊ������	
	if(NUM < 0)
	{
		res[num++] = '-';
		NUM = 0-NUM;
	}
 
    inte = (uint32_t)NUM;
    deci = NUM - inte;
	
	///�����������֣�
    if(inte)
    {
		while(inte)
		{
			temp[temp_i++] = inte%10 + 0x30;
			inte /= 10;
		}
	}else{
		temp[temp_i++] = '0';
	}
 
	i = temp_i;
	while(i--)
	{
		res[num++] = temp[--temp_i];//���������--temp_i,ԭ��:ǰһ������д�������temp_i�Ѿ��Լ�1��
	}
	
	///�ж��Ƿ���ʾС���㣺
	if(accu){
		res[num++] = '.';
	}

	///����С�����֣�	
	i = 0;
	while(i != accu)
    {
		deci *= 10;
		inte  = (uint32_t)deci;
		res[num++] = inte + 0x30;
		deci -= inte;
		
		++i;
    }
 
	///��ʾ�������
	i = 0;
	for(i=0; i<num; i++){
		OLED_DisplayCHAR(pixelX+i*oled_display_font_width, pixelY, res[i]);
	}
	
	///����д�������Χ��
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}



/**
  * @brief	:ֹͣ��������(Ӳ��)
  * @note   :--OLED_Move();//�������ú���Ӳ���Զ��ƶ���Ļ��ʹ�ñ��������йر�
			 --ֹͣ�����󣬻�ָ���ʾ����ǰ�����ݣ�
  * @param	:void
  * @return	:void
  *
  * @date   :2018/01/19
  * @design :
  **/
void OLED_MoveStop(void)
{
	oled_write_command(0x2E);	//disactive scroll!	
}



/**
  * @brief	:��������(Ӳ��)
  * @note   :--Ӳ��������ֻ�ܵ���һ�Σ���
			 --ˮƽ��������ֻ�ܰ�ҳ����������OLED_Move(0, 1, DIRECT_LEFT);//0��1ҳȫ�������������
			 --����ʵ�ִ�ֱ�������������²�����OLED_Move(0, 0, DIRECT_UP);//ҳ0�в��������ݣ�
			 --�������ʣ�
				000��5	 frames
				001��64  frames
				010��128 frames
				011��256 frames
				100��3	 frames
				101��4	 frames
				110��25	 frames
				111��2	 frames
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
			 direct, ��Ļ�ƶ�����(_OLEDScrollDirectö���ж������ͣ�)
  * @return	:void
  *
  * @date   :2018/01/19
  * @design :
  **/
void OLED_Move(uint32_t start, uint32_t end, _OLEDScrollDirect direct)
{
	uint32_t temp=0, mask=0;
		
	start &= 0x07;				//�������ƣ���֤Ϊ0~7
	end   &= 0x07;
	
	if(start > end)				//��֤pStar<pEnd��
	{
		temp  = start;
		start = end;
		end   = temp;
	}
	
	///0x29��0x2Aָ��Ҫ��ByteEΪ1����0x26��0x27Ҫ��Ϊ0��
	mask  = (uint32_t)direct & 0x04;
	mask  =  mask>>2;
	mask  = ~mask;
	mask &=  0x01;
		
	oled_write_command(direct);	//scroll direction
	oled_write_command(0x00);
	oled_write_command(start);	//start address
	oled_write_command(0x00);	//set time between each scroll step as 5 frames
	oled_write_command(end);	//end address
	oled_write_command(mask);
	oled_write_command(0xFF);
	
	oled_write_command(0x2F);	//active scroll!
}



/**
  * @brief	:������������(��������)
  * @note   :--��ֹ�ⲿ���ã�
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
			 direct, ��Ļ�ƶ�����(_OLEDScrollDirectö���ж������ͣ�)
             loop  , �Ƿ�ѭ��(1ѭ��  0��ѭ��)
  * @return	:void
  *
  * @date   :2016/12/24
  * @design :
  **/
static void oled_move_horizontal(uint32_t pStart, uint32_t pEnd , uint32_t cStart, uint32_t cEnd, _OLEDScrollDirect direct, uint32_t loop)
{
    uint32_t page=0, column=0;
    uint32_t column_inc=0;
    uint8_t temp=0;
    	
	column_inc = (direct == DIRECT_LEFT) ? 127 : 1;
        
    for(page=pStart; page<=pEnd; page++)
    {
        if(direct == DIRECT_RIGHT)
        {
            temp = OLED_GRAM[cStart][page];//������cStart������

            for(column=cStart; column<=cEnd; column++){
                OLED_GRAM[column][page] = OLED_GRAM[(column+column_inc)%128][page]; 
            }

            if(loop)//����ѭ��ģʽ
            {
                OLED_GRAM[cEnd][page] = temp;
            }else{
                OLED_GRAM[cEnd][page] = 0;
            }
        }else if(direct == DIRECT_LEFT){
            temp = OLED_GRAM[cEnd][page];//������cEnd������

            for(column=cEnd; column>cStart; column--){
                OLED_GRAM[column][page] = OLED_GRAM[(column+column_inc)%128][page]; 
            }

            if(loop)//����ѭ��ģʽ
            {
                OLED_GRAM[cStart][page] = temp;
            }else{
                OLED_GRAM[cStart][page] = 0;
            }
        }
    }   
    
    OLED_UpdatePart(pStart, pEnd, cStart, cEnd, 0);
}



/**
  * @brief	:������������(��������)
  * @note   :--��ֹ�ⲿ���ã�
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
			 direct, ��Ļ�ƶ�����(_OLEDScrollDirectö���ж������ͣ�)
             loop  , �Ƿ�ѭ��(1ѭ��  0��ѭ��)
  * @return	:void
  *
  * @date   :2016/12/24
  * @design :
  **/
static void oled_move_vertical(uint32_t pStart, uint32_t pEnd , uint32_t cStart, uint32_t cEnd, _OLEDScrollDirect direct, uint32_t loop)
{
    uint32_t page=0, column=0;
    uint8_t temp[8]={0};
    
    for(column=cStart; column<=cEnd; column++)
    {
        if(direct == DIRECT_UP)
        {
            for(page=pStart; page<=pEnd; page++)
            {
                temp[page] = (OLED_GRAM[column][page]&0x01)<<7;//ȡ�����λ�����ƶ������λ
                OLED_GRAM[column][page] >>= 1;//�������λ�ƶ����������ƶ�
            }
            
            for(page=pStart; page<=pEnd; page++)
            {
                if(loop)//����ѭ��ģʽ
                {
                    OLED_GRAM[column][page] |= temp[(page+1)%8];
                }else{
                    OLED_GRAM[column][page] |= 0;
                }
            }    
            
            OLED_GRAM[column][pEnd] |= temp[pStart]; 
        }else if(direct == DIRECT_DOWN)
        {
            for(page=pStart; page<=pEnd; page++)
            {
                temp[page] = (OLED_GRAM[column][page]&0x80)>>7;//ȡ�����λ�����ƶ������λ
                OLED_GRAM[column][page] <<= 1;//�������λ�ƶ����������ƶ�
            }
            
            for(page=pStart; page<=pEnd; page++)
            {
                if(loop)//����ѭ��ģʽ
                {
                    OLED_GRAM[column][page] |= temp[(page+7)%8];
                }else{
                    OLED_GRAM[column][page] |= 0;
                }
            }   
            
            OLED_GRAM[column][pStart] |= temp[pEnd]; 
        }        
    }

    OLED_UpdatePart(pStart, pEnd, cStart, cEnd, 1);
}



/**
  * @brief	:��������(��������)
  * @note   :--��С��λΪҳ*�У�
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
			 direct, ��Ļ�ƶ�����(_OLEDScrollDirectö���ж������ͣ�)
             loop  , �Ƿ�ѭ��(1ѭ��  0��ѭ��)
  * @return	:void
  *
  * @date   :2016/12/24
  * @design :
  **/
void OLED_Move2(uint32_t pStart, uint32_t pEnd , uint32_t cStart, uint32_t cEnd, _OLEDScrollDirect direct, uint32_t loop)
{
    switch(direct)
    {
        case DIRECT_UP    : oled_move_vertical(pStart, pEnd, cStart, cEnd, DIRECT_UP, loop); break;
        case DIRECT_DOWN  : oled_move_vertical(pStart, pEnd, cStart, cEnd, DIRECT_DOWN, loop); break;
        case DIRECT_LEFT  : oled_move_horizontal(pStart, pEnd, cStart, cEnd, DIRECT_LEFT, loop); break;
        case DIRECT_RIGHT : oled_move_horizontal(pStart, pEnd, cStart, cEnd, DIRECT_RIGHT, loop); break;
        
        default : break;
    }
}



/**
  * @brief	:��ɫ����(һ��ʵ��)
  * @note   :--ע�⣺��ɫ����������ַ������򣬼���С�任�����СΪ6*8��С�ڴ�ֵ���δ֪��
			 --�޸ģ�v7.0�汾��Ժ��������޸ģ���֧��������ʾ��
			 --���飺�ɸ�����������򻯳����羡������ҳ�в�����������Ҫ��ҳ��
  * @param	:*zone, ��������
  * @return	:void
  *
  * @date   :2017/12/22
  * @design :
  **/
void OLED_Invert(_OLEDZoneHandle *zone)
{
	uint32_t page[2], column[2];
	uint32_t i, j;
	uint8_t mask;
	uint8_t temp[2];
	
	page[0]	  = zone->pixel_column_start/8;
	page[1]	  = zone->pixel_column_end/8;
	column[0] = zone->pixel_row_start;
	column[1] = zone->pixel_row_end;	
		
	for(j=column[0]; j<=column[1]; j++)
	{
		///��ʼһҳ�����������⴦����ֲ�����ʽ������⣡*/
		mask	 =	0xFF;
		temp[0]  =  OLED_GRAM[j][page[0]];
		temp[1]  =  temp[0];
		temp[0]  = ~temp[0];//���ֽڵĸ�λ����ȡ����
		mask	 =  mask<<(zone->pixel_column_start%8);
		temp[0] &=  mask;
		mask  	 = ~mask;
		temp[1] &=  mask;
		temp[0]  =  temp[1] | temp[0];
		OLED_GRAM[j][page[0]] = temp[0];
		
		///��ʼһҳ�����������⴦��*/
		if(page[1] != (page[0]+1))
		{
			for(i=page[0]+1; i<page[1]; i++)
			{
				temp[0] = OLED_GRAM[j][i];
				temp[0] = ~temp[0];
				OLED_GRAM[j][i] = temp[0];
			}		
		}
		
		///���һҳ�����������⴦����ֲ�����ʽ������⣡*/
		if(page[1] != (page[0]))//��ֹ��ҳ��βҳ�ظ�����ɫ��
		{
			mask	 =	0xFF;
			temp[0]  =  OLED_GRAM[j][page[1]];
			temp[1]  =  temp[0];
			temp[0]  = ~temp[0];//���ֽڵĵ�λ����ȡ����
			mask	 =  mask>>(7 - zone->pixel_column_end%8);
			temp[0] &=  mask;
			mask	 = ~mask;
			temp[1] &=  mask;
			temp[0]  =  temp[1] | temp[0];
			OLED_GRAM[j][page[1]] = temp[0];
		}
	}
}



/**
  * @brief	:��ɫ����(��ҳΪ��λ��ɫ)
  * @note   :--������λΪҳ*�У�
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
  * @return	:void
  *
  * @date   :2017/12/22
  * @design :
  **/
void OLED_Invert2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd)
{
	uint32_t column=0, page=0;
	uint8_t temp=0;
	
	for(page=pStart; page<=pEnd; page++)
	{
		for(column=cStart; column<=cEnd; column++)
		{
			temp = OLED_GRAM[column][page];
			temp = ~temp;
			OLED_GRAM[column][page] = temp;
		}
	}
}



/**
  * @brief	:ˮƽ��ת����(һ��ʵ��)
  * @note   :--��δʵ�֣�v7.0�汾���ݲ�֧�ִ˺�����-- 2017.12.23 by seri_liang.
  * @param	:*zone, ��������
  * @return	:void
  *
  * @date   :2016/12/23
  * @design :
  **/
//void OLED_Turn(_OLEDZoneHandle *zone)
//{
//	
//}



/**
  * @brief	:ˮƽ��ת����(��ҳΪ��λ��ת)
  * @note   :--������λΪҳ*�У�
  * @param	:pStart, ��ʼҳ
             pEnd  , ��ֹҳ
             cStart, ��ʼ��
             cEnd  , ��ֹ��
  * @return	:void
  *
  * @date   :2016/12/24
  * @design :
  **/
void OLED_Turn2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd)
{
	uint32_t column=0, page=0;
	uint8_t temp=0;
	
    for(column=0; column<(cEnd - cStart)/2; column++)
    {
        for(page=pStart; page<=pEnd; page++)
        {
            temp = OLED_GRAM[column][page];
            OLED_GRAM[column][page]		 = OLED_GRAM[cEnd-column][page];
            OLED_GRAM[cEnd-column][page] = temp;
        }
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			oled_write_data(BMP[j++]);
		}
	}
}
#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










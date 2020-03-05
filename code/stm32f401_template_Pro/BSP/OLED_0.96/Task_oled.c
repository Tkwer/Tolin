/**
  ******************************************************************************
  * 
  * OLED 0.96寸显示屏驱动文件，可使用软件i2c、软件spi，默认为软件i2c
  * 
  * 本文件包含基本的显示、画点、画线、画图功能及基本的数字显示(整数和小数)等；
  * 本程序默认使用i2c协议，可通过宏定义修改为spi协议，仅提供此两种方式的控制！
  * 本程序为GUI界面服务，除学习外不得作为他用！同时欢迎不断补充功能！
  * 
  * 
  * 使用注意：
  * 	1.请在指定区域内配置所使用的底层驱动，如i2c、spi的实现方式！
  *		2.注意oled所使用的spi协议不同于一般spi，需要特殊设置，可参考oledspi.c/.h！
  *
  * 版本修订：
  *		修改时间	版本号	修改内容
  *		2016-09-09	v1.0	支持软件模拟I2C控制协议；
  *		2016-12-24	v2.0	新添移屏(上下左右)、指定区域反色、对称(左右)操作；
  *		2016-12-24	v3.0	增添6*8字体；
  *		2017-01-02	v4.0	增加中点画圆算法和改进中点画圆算法；
  *		2017-01-04	v4.1	增加中点画椭圆算法和改进中点画椭圆算法；
  *	 // 2017-10-15	v5.0	修改整体框架，兼容STM32.F4XX和NXP.K60单片机的控制操作；
  *	 // 2017-12-10	v6.0	增加汉字库；
  *		2017-12-21	v7.0	1.重构框架，使用像素坐标；
  *							2.驱动文件被分解开，可自由选择底层驱动类型；
  *							3.部分函数不在支持，函数被屏蔽，可自行实现！
  *							4.取消对非页寻址方式的支持！
  *		2017-12-26	v8.0	重定向printf到oled屏幕上显示，此模式自动换行；
  *							此操作需要定义ENABLE_PRINTF_SUPPORT！
  *		2017-12-30	v9.0	1.增加对printf后显示若位置参数越界后的容错机制；
  *							2.增加硬件移屏操作，放弃之前对移屏的软件方式支持；
  *		2018-01-19	v10.0	1.优化硬件移屏操作，支持垂直滚动；
  *                         2.优化软件移屏操作；
  *							3.恢复水平、垂直寻址的支持；
  *		2018-01-21	v10.1	1.修复调用OLED_DisplayFLOAT()函数导致程序挂起的bug！
  *							2.添加读点函数：OLED_ReadPixelPoint();
  *							3.增加对emWin v532的支持！
  *		2018-01-22	v10.2	优化printf重定向后显示需要"\\"辅助显示的问题；
  *
  * 程序出处：
  *		Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI
  * 	All rights reserved			
  *
  *
  *	Create  ：2016年09月09日
  * Update  ：2018年01月22日
  * Author  ：梁宗南(SERI.LJI)
  * Email	：liangzongnan0214@163.com
  * Blog    ：liangzongnan.com
  * QQ		：494089986
  * 
  ******************************************************************************
***/



#include "Task_oled.h"
#include "oledfont.h"


/* 0.96 oled驱动器SSD1306寄存器配置单：*/
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
	0xA1,	//Set SEG/Column Mapping			0xA0左右反置 0xA1正常
	0xC8,	//Set COM/Row Scan scanDirection  	0xC0上下反置 0xC8正常
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
  
  
/* oled显存空间设置：*/
//OLED的显存(与OLED_ORIGIN_SET无关)
// 	   [0]:	 0 1 2 3 ... 127	
//	   [1]:	 0 1 2 3 ... 127	
//	   [2]:	 0 1 2 3 ... 127	
//	   [3]:	 0 1 2 3 ... 127	
//	   [4]:	 0 1 2 3 ... 127	
//	   [5]:	 0 1 2 3 ... 127	
//	   [6]:	 0 1 2 3 ... 127	
//	   [7]:	 0 1 2 3 ... 127 
#if OLED_GRAM_SIZE == 0	
	uint8_t OLED_GRAM[128][8]={0};//oled 显存(正常模式)
#else
	uint8_t OLED_GRAM[256][16]={0};//oled 显存(扩展模式)
#endif


/* oled全局控制变量：*/
#ifndef ENABLE_EMWIN_SUPPORT					//以下变量在非emWin下使用！
static uint32_t oled_display_cover_mode  = 1;	//字符类显示模式(默认1，正常显示)
static uint32_t oled_display_font_width  = 8;	//字符类显示字体大小(默认8，1608字体)
static uint32_t oled_display_font_height = 16;	//字符类显示字体大小(默认16，1608字体)
#endif 	
	
	
/* ------------------------   设置相关依赖：  ------------------------- */
#include "delay.h"

#define oled_delay_us(t)				delay_us(t)					
#define oled_delay_ms(t)				delay_ms(t)	

/* ------------------------   初始化驱动器：  ------------------------- */
#include "softi2c.h"
	
#define oled_dev_init()						SW_I2C_InitConfig()	
#define oled_write_data(dat)			SW_I2C_WrDat(dat)
#define oled_write_command(cmd)		SW_I2C_WrCmd(cmd)	
	
	
	
/**
  * @brief	:printf重载
  * @note   :--调用printf函数必须，用户直接调用printf！
			 --优化printf重定向后显示需要"\\"辅助显示的问题；
  * @date   :2018/01/22
  * @design :
  **/
#ifdef ENABLE_PRINTF_SUPPORT   
	#pragma import(__use_no_semihosting)             
	struct __FILE 
	{ 
		int handle; 
		
	}__stdout;				//标准库需要的支持函数                 
	 
	void _sys_exit(int x) 	//定义_sys_exit()以避免使用半主机模式    
	{ 
		x = x; 
	} 

	static uint32_t poled_pixelx=0;
	static uint32_t poled_pixely=0;
	void oled_goto_xy(uint32_t x, uint32_t y)
	{			
		if(x > (OLED_SCREEN_WIDTH - oled_display_font_width) 
		|| y > (OLED_SCREEN_HEIGHT - oled_display_font_height)) return;//越界后会在前一次设定的末尾接着显示！

		poled_pixelx = x;
		poled_pixely = y;
	}

	///注意：调用printf()后直接更新显示！
	///暂不支持控制字符"\r"、"\n"等的输入！
	int fputc(int chda, FILE *f)
	{			
		OLED_DisplayCHAR(poled_pixelx, poled_pixely, (char)chda);
		
		poled_pixelx +=  oled_display_font_width;	
		if((OLED_SCREEN_WIDTH - poled_pixelx) > oled_display_font_width)//剩余不足显示一个字符，直接换行显示！
		{
			poled_pixely += oled_display_font_height;
			poled_pixelx  = 0;
		}		
		return chda;
	}
#endif



/**
  * @brief	:oled显示初始化配置
  * @note   :--修改：仅支持页寻址模式！
			 --增添：默认使用页寻址，初始化时窗口扩展到整个屏幕！
			 --修改：对局部内容格式进行修正，内容未变！-- v10.1
			 --增添：添加使能printf重定向后的起始点初始化；-- v10.2
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
	
	///配置ssd1306寄存器！
	for(i=0; i<sizeof(oled_ctl_cmd); i++){
		oled_write_command(oled_ctl_cmd[i]);
	}
	
#ifdef ENABLE_PRINTF_SUPPORT
	///使能printf重定向时，需要设置显示起始点！
	oled_goto_xy(0, 0);
#endif
	
	OLED_ClearScreen();//初始化后清屏
}
	


/**
  * @brief	:开启oled显示 
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
  * @brief	:关闭oled显示  
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
  * @brief	:oled设置显示坐标(页、列)
  * @note   :--仅页寻址模式下可直接使用，其他模式需要额外设置！
  * @param	:page  , 页坐标
			 column, 列坐标
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_SetPos(uint32_t page, uint32_t column)
{	
	oled_write_command(0xB0 +  page);			//页显示位置
	oled_write_command(0x10 + (column&0xF0)>>4);//行显示位置
	oled_write_command(0x00 + (column&0x0F));	//
}



/**
  * @brief	:oled更新显示
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
		OLED_SetPos(page, 0);//仅设定页地址即可！			
		for(column=0; column<128; column++){		
			oled_write_data(OLED_GRAM[column][page]);
		}
	}
}



/**
  * @brief	:oled清屏
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
  * @brief	:画像素点(像素坐标)
  * @note   :--
  * @param	:pixelX   , 行坐标
			 pixelY   , 列坐标
			 writeMode, 模式(0反色 1正常)
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
	///设置扫描窗口大小为从下笔点到屏幕对角点！
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
  * @brief	:读像素点(像素坐标)
  * @note   :--函数支持emWin调用！
  * @param	:pixelX, 行坐标
			 pixelY, 列坐标
  * @return	:uint8_t, 读取到点的颜色值
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
  * @brief	:oled坐标点打包函数(像素坐标)
  * @note   :--注意：尽量不要再外部调用本函数，无参数检查！
  * @param	:xStart, 行起始坐标
			 yStart, 列起始坐标
			 xEnd  , 行终止坐标
			 yEnd  , 列终止坐标
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
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
  * @brief	:oled局部更新显示
  * @note   :--v10.0开始，重新使用本函数！
			 --不是太严谨，调用完回设成页寻址模式！-- by liang_seri 2018.01.21
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
			 direct, 选择刷新方向(1纵向  0横向)
  * @return	:void
  *
  * @date   :2019/01/19
  * @design :
  **/
void OLED_UpdatePart(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd, uint32_t direct)
{
	uint32_t column=0, page=0;

    if(direct)						//纵向刷新
    {        
        oled_write_command(0x20);	//设置为水平寻址模式，因为页寻址下的移动操作错位严重(刷新时间太长！)	
        oled_write_command(0x00);
        oled_write_command(0x21);	//设置水平寻址起始列和终止列
        oled_write_command(cStart);
        oled_write_command(cEnd);
        oled_write_command(0x22);	//设置水平寻址起始页和终止页
        oled_write_command(pStart);
        oled_write_command(pEnd);

        for(page=pStart; page<=pEnd; page++)
        {		
            for(column=cStart; column<=cEnd; column++){		
                oled_write_data(OLED_GRAM[column][page]);
            }
        }
    }else{							//横向刷新
        oled_write_command(0x20);	//设置为垂直寻址模式，因为页寻址下的移动操作错位严重(刷新时间太长！)	
        oled_write_command(0x01);
        oled_write_command(0x21);	//设置垂直寻址起始列和终止列
        oled_write_command(cStart);
        oled_write_command(cEnd);
        oled_write_command(0x22);	//设置垂直寻址起始页和终止页
        oled_write_command(pStart);
        oled_write_command(pEnd);
        
		for(column=cStart; column<=cEnd; column++){		
        {		
			for(page=pStart; page<=pEnd; page++)
                oled_write_data(OLED_GRAM[column][page]);
            }
        }        
    }
    
    oled_write_command(0x20);		//恢复默认寻址模式，页寻址模式！
    oled_write_command(0x02);
	oled_write_command(0x21);		//设置起始列和终止列
	oled_write_command(0);
	oled_write_command(127);
	oled_write_command(0x22);		//设置起始页和终止页
	oled_write_command(0);
	oled_write_command(7);
}



/**
  * @brief	:画点(像素坐标)
  * @note   :--
  * @param	:pixelX   , 行坐标
			 pixelY   , 列坐标
			 fontSize , 点大小
			 writeMode, 模式(0反色  1正常)
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
  * @brief	:画线(像素坐标，左上为基点，右下增)
  * @note   :--
  * @param	:xStart, 行起始坐标(0~127)
			 yStart, 列起始坐标(0~63)
			 xEnd  , 行终止坐标(0~127)
			 yEnd  , 列终止坐标(0~63)
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd)
{
	int8_t  x_width=0;	//x轴宽度 
	int8_t  y_height=0;	//y轴高度
	int8_t  x_inc=0;	//x方向自增标记
	int8_t  y_inc=0;	//y方向自增标记
	int8_t  rem;		//current remainder 
	uint8_t start=0, end=0;
	uint8_t i=0;
		
	if(yStart == yEnd)//绘制水平线，horizon line
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
	}else if(xStart == xEnd){//绘制垂直线，vertical line
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
	}else{//绘制任意直线
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
  * @brief	:八分对称法(像素坐标)
  * @note   :--画出给定点的八分对称点(画圆基础算法)
  * @param	:xc, 圆心行坐标
			 yc, 圆心列坐标
             x , 给定点
			 y , 给定点
  * @return	:void
  *
  * @date   :2017/01/02
  * @design :
  **/
static void Circle8Point(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y)
{
    //直角坐标系第一象限x轴开始，逆时针旋转！
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
  * @brief	:改进画圆(像素坐标)
  * @note   :--避免浮点运算(轴上点不突进！)！
  * @param	:xc, 圆心行坐标
			 yc, 圆心列坐标
			 r , 半径
  * @return	:void
  *
  * @date   :2017/01/02
  * @design :
  **/
void OLED_DrawCircle(uint32_t xc, uint32_t yc, uint32_t r)
{
    uint32_t x=0, y=0;
    int32_t d=0;//改进，避免浮点运算！
        
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
  * @brief	:四分对称法(像素坐标)
  * @note   :--画出给定点的四分对称点(画椭圆基础算法)
  * @param	:xc, 椭圆中心行坐标
			 yc, 椭圆中心列坐标
             x , 给定点
			 y , 给定点
  * @return	:void
  *
  * @date   :2017/01/04
  * @design :
  **/
static void Ellipse4Point(uint32_t xc, uint32_t yc, uint32_t x, uint32_t y)
{
    //直角坐标系第一象限开始，逆时针旋转！
    OLED_DrawPixelPoint((xc+x), (yc+y), 1);//1
    OLED_DrawPixelPoint((xc-x), (yc+y), 1);//2
    OLED_DrawPixelPoint((xc-x), (yc-y), 1);//3
    OLED_DrawPixelPoint((xc+x), (yc-y), 1);//4
}



/**
  * @brief	:画椭圆(像素坐标)
  * @note   :--
  * @param	:xc, 椭圆中心行坐标
			 yc, 椭圆中心列坐标
             a , 半长轴长度
			 b , 半短轴长度
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
  * @brief	:画方框(像素坐标)
  * @note   :--
  * @param	:xStart, 行起始坐标
			 yStart, 列起始坐标
			 xEnd  , 行终止坐标
			 yEnd  , 列终止坐标
  * @return	:void
  *
  * @date   :2016/09/09
  * @design :
  **/
void OLED_DrawBlock(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd)
{
	OLED_DrawLine(xStart, yStart, xStart, yEnd);//左界
	OLED_DrawLine(xEnd, yStart, xEnd, yEnd);//右界
	OLED_DrawLine(xStart, yStart, xEnd, yStart);//上界
	OLED_DrawLine(xStart, yEnd, xEnd, yEnd);//下界
}



/**
  * @brief	:画矩形(像素坐标)
  * @note   :--
  * @param	:xStart , 行起始坐标
			 yStart , 列起始坐标
			 length , 矩形长度
			 width  , 矩形宽度
			 isFill , 是否填充(0不填充 1填充)
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
	
	OLED_DrawBlock(rect_xs, rect_ys, rect_xe, rect_ye);//绘制边框
	
	if(isFill)//判断是否填充
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
  * @brief	:设置display类函数的显示模式
  * @note   :--
  * @param	:mode, 显示模式(_OLEDDisplayMode枚举中定义类型！)
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
  * @brief	:设置display类函数的显示字体大小
  * @note   :--若无效字体，则使用1608字体！
  * @param	:fontSize, 字体大小(_OLEDFontSize枚举中定义类型！)
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
  * @brief	:显示ASCII标准字体字符(像素坐标)
  * @note   :--
  * @param	:pixelX, 行起始坐标
			 pixelY, 列起始坐标
			 CHAR  , 待显示的字符
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
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
		
	///获取指定字体字库中所占字节数：	
	zone_temp.pixel_row_start	 = pixelX;
	zone_temp.pixel_column_start = pixelY;
	switch(oled_display_font_height)//oledfont.h中每一行表示一个字符的显示编码！获取显示区域使用！
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
	
	///写入显存： 	
	for(i=0; i<font_ram_size; i++)//总字节数
	{
		if(i == oled_display_font_width)//对于0806以外的字体，需要多页存储！
		{
			pixelX  = x0;
			pixelY += 8;
			y0	    = pixelY; 
		}

		switch(oled_display_font_height)//选择字库
		{
			case 8  : temp_data = asc_oled_0806[CHAR-0x20][i]; break;
			case 16 : temp_data = asc_oled_1608[CHAR-0x20][i]; break;				
		}

		for(j=0; j<8; j++)
		{
			if(temp_data&0x01)//此处跟字模提取方法有关：字模地址低对应页中的低位，先行后列取模！
				OLED_DrawPixelPoint(pixelX, pixelY,  oled_display_cover_mode);
			else
				OLED_DrawPixelPoint(pixelX, pixelY, !oled_display_cover_mode);
			
			temp_data >>= 1;
			++pixelY;		
		}	
		++pixelX;//下一列！
		pixelY = y0;//行复位！
	}
	
	///计算写入的区域范围：
    return zone_temp;    
}



/**
  * @brief	:显示ASCII标准字体字符串(像素坐标)
  * @note   :--标准ASCII字符串，以'\0'结尾！
  * @param	:pixelX  , 行起始坐标
			 pixelY  , 列起始坐标
			 *STR    , 待显示的字符串指针
             fontSize, 字体大小(6为6*8大小 8为8*16大小)
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
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
	
	///计算写入的区域范围：
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + str_num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}



/**
  * @brief	:显示转化后的16进制数字(像素坐标)
  * @note   :--修改：取消了2位显示模式，仅支持显示4位HEX！
  * @param	:axisX, 行起始坐标
			 axisY, 列起始坐标
			 NUM  , 待显示的数字(十进制)
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayHEX(uint32_t axisX, uint32_t axisY, uint16_t NUM)
{	
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	uint32_t i=0;
	uint8_t hex_num[5]={0};//多余的一个字节存放"\0"
	
	for(i=0; i<4; i++)
	{
		switch((NUM>>(12-4*i))&0x000F)//从高位到低位依此转换为字符！
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
	
	///计算写入的区域范围：
	zone_temp = OLED_DisplaySTR(axisX, axisY, (char *)hex_num);//显示转换后的16进制数字
	return zone_temp;
}



/**
  * @brief	:显示无符号的10进制数字(像素坐标)
  * @note   :--
  * @param	:pixelX, 行起始坐标
			 pixelY, 列起始坐标
			 NUM   , 待显示的数字
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayUINT(uint32_t pixelX, uint32_t pixelY, uint32_t NUM)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	uint32_t i=0, num, n=0;
	uint8_t res[OLED_MAX_LENGTH_OF_DEC]={0};//最大显示OLED_MAX_LENGTH_OF_DEC位数！

	if(NUM == 0)//数字0为特殊情况，单独考虑！
	{
		res[i++] = 0+0x30;//0x30即'0'
	}else{
		while((NUM/10) || (NUM%10))
		{  
			res[i++] = NUM%10+0x30;
			NUM /= 10;  
		} 
	}

	num = i;//显示了j位10进制
	while(i--)
	{  
		OLED_DisplayCHAR(pixelX+i*oled_display_font_width, pixelY, (char)(res[n++]));
	}

	///计算写入的区域范围：	
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}

	
	
/**
  * @brief	:显示有符号的10进制数字(像素坐标)
  * @note   :--
  * @param	:pixelX, 行起始坐标
			 pixelY, 列起始坐标
			 NUM   , 待显示的数字
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
  *
  * @date   :2016/09/09
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayINT(uint32_t pixelX, uint32_t pixelY, int32_t NUM)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};
	
	if(NUM < 0)//若为负数！
	{
		OLED_DisplayCHAR(pixelX, pixelY , '-');//显示复数
		zone_temp = OLED_DisplayUINT(pixelX+8, pixelY, (0 - NUM));
		zone_temp.pixel_row_start = pixelX;//向前，把'-'算进去！
	}else{
		zone_temp = OLED_DisplayUINT(pixelX, pixelY, NUM);
	}

	return zone_temp;
}



/**
  * @brief	:显示可变精度的小数(像素坐标)
  * @note   :--v10.1 修复函数显示导致程序挂起bug！
  * @param	:pixelX, 行起始坐标
			 pixelY, 列起始坐标
			 NUM   , 待显示的数字
			 accu  , 精确位数
  * @return	:_OLEDZoneHandle(OLED_GRAM坐标)
  *
  * @date   :2018/01/21
  * @design :
  **/
_OLEDZoneHandle OLED_DisplayFLOAT(uint32_t pixelX, uint32_t pixelY, float NUM, uint32_t accu)
{
	_OLEDZoneHandle zone_temp={0, 0, 0, 0};

	float deci=0.0f;//小数
	uint32_t inte=0;//整数

	uint32_t i=0, num=0, temp_i=0;
	uint8_t res[OLED_MAX_LENGTH_OF_DEC];//最大显示OLED_MAX_LENGTH_OF_DEC位数，包括符号和小数点！
	uint8_t temp[OLED_MAX_LENGTH_OF_DEC-2];
	
 
	///判断是否为负数：	
	if(NUM < 0)
	{
		res[num++] = '-';
		NUM = 0-NUM;
	}
 
    inte = (uint32_t)NUM;
    deci = NUM - inte;
	
	///反序整数部分：
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
		res[num++] = temp[--temp_i];//后面必须是--temp_i,原因:前一步中在写入数组后temp_i已经自加1！
	}
	
	///判断是否显示小数点：
	if(accu){
		res[num++] = '.';
	}

	///正序小数部分：	
	i = 0;
	while(i != accu)
    {
		deci *= 10;
		inte  = (uint32_t)deci;
		res[num++] = inte + 0x30;
		deci -= inte;
		
		++i;
    }
 
	///显示最后结果：
	i = 0;
	for(i=0; i<num; i++){
		OLED_DisplayCHAR(pixelX+i*oled_display_font_width, pixelY, res[i]);
	}
	
	///计算写入的区域范围：
	zone_temp = oled_zone(pixelX, pixelY, (pixelX + num*oled_display_font_width - 1), (pixelY + oled_display_font_height - 1));
	return zone_temp;
}



/**
  * @brief	:停止移屏操作(硬件)
  * @note   :--OLED_Move();//函数调用后，由硬件自动移动屏幕，使用本函数进行关闭
			 --停止滚动后，会恢复显示滚动前的内容！
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
  * @brief	:移屏操作(硬件)
  * @note   :--硬件移屏，只能调用一次！！
			 --水平移屏操作只能按页滚动！即：OLED_Move(0, 1, DIRECT_LEFT);//0和1页全部内容向左滚动
			 --若想实现垂直滚动，按照如下操作：OLED_Move(0, 0, DIRECT_UP);//页0中不能有数据！
			 --滚动速率：
				000：5	 frames
				001：64  frames
				010：128 frames
				011：256 frames
				100：3	 frames
				101：4	 frames
				110：25	 frames
				111：2	 frames
  * @param	:pStart, 起始页
             pEnd  , 终止页
			 direct, 屏幕移动方向(_OLEDScrollDirect枚举中定义类型！)
  * @return	:void
  *
  * @date   :2018/01/19
  * @design :
  **/
void OLED_Move(uint32_t start, uint32_t end, _OLEDScrollDirect direct)
{
	uint32_t temp=0, mask=0;
		
	start &= 0x07;				//参数限制，保证为0~7
	end   &= 0x07;
	
	if(start > end)				//保证pStar<pEnd！
	{
		temp  = start;
		start = end;
		end   = temp;
	}
	
	///0x29和0x2A指令要求ByteE为1，而0x26和0x27要求为0！
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
  * @brief	:左右移屏操作(任意区域)
  * @note   :--禁止外部调用！
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
			 direct, 屏幕移动方向(_OLEDScrollDirect枚举中定义类型！)
             loop  , 是否循环(1循环  0不循环)
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
            temp = OLED_GRAM[cStart][page];//保护第cStart列数据

            for(column=cStart; column<=cEnd; column++){
                OLED_GRAM[column][page] = OLED_GRAM[(column+column_inc)%128][page]; 
            }

            if(loop)//采用循环模式
            {
                OLED_GRAM[cEnd][page] = temp;
            }else{
                OLED_GRAM[cEnd][page] = 0;
            }
        }else if(direct == DIRECT_LEFT){
            temp = OLED_GRAM[cEnd][page];//保护第cEnd列数据

            for(column=cEnd; column>cStart; column--){
                OLED_GRAM[column][page] = OLED_GRAM[(column+column_inc)%128][page]; 
            }

            if(loop)//采用循环模式
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
  * @brief	:上下移屏操作(任意区域)
  * @note   :--禁止外部调用！
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
			 direct, 屏幕移动方向(_OLEDScrollDirect枚举中定义类型！)
             loop  , 是否循环(1循环  0不循环)
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
                temp[page] = (OLED_GRAM[column][page]&0x01)<<7;//取出最低位，并移动至最高位
                OLED_GRAM[column][page] >>= 1;//数据向低位移动，即向上移动
            }
            
            for(page=pStart; page<=pEnd; page++)
            {
                if(loop)//采用循环模式
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
                temp[page] = (OLED_GRAM[column][page]&0x80)>>7;//取出最高位，并移动至最低位
                OLED_GRAM[column][page] <<= 1;//数据向高位移动，即向下移动
            }
            
            for(page=pStart; page<=pEnd; page++)
            {
                if(loop)//采用循环模式
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
  * @brief	:移屏操作(任意区域)
  * @note   :--最小单位为页*列！
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
			 direct, 屏幕移动方向(_OLEDScrollDirect枚举中定义类型！)
             loop  , 是否循环(1循环  0不循环)
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
  * @brief	:反色操作(一般实现)
  * @note   :--注意：反色操作仅针对字符类区域，即最小变换区域大小为6*8，小于此值结果未知！
			 --修改：v7.0版本后对函数进行修改，不支持立即显示！
			 --建议：可根据自身情况简化程序，如尽量在整页中操作，尽量不要跨页！
  * @param	:*zone, 操作区域
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
		///起始一页不完整，特殊处理：拆分操作公式便于理解！*/
		mask	 =	0xFF;
		temp[0]  =  OLED_GRAM[j][page[0]];
		temp[1]  =  temp[0];
		temp[0]  = ~temp[0];//对字节的高位进行取反！
		mask	 =  mask<<(zone->pixel_column_start%8);
		temp[0] &=  mask;
		mask  	 = ~mask;
		temp[1] &=  mask;
		temp[0]  =  temp[1] | temp[0];
		OLED_GRAM[j][page[0]] = temp[0];
		
		///起始一页不完整，特殊处理：*/
		if(page[1] != (page[0]+1))
		{
			for(i=page[0]+1; i<page[1]; i++)
			{
				temp[0] = OLED_GRAM[j][i];
				temp[0] = ~temp[0];
				OLED_GRAM[j][i] = temp[0];
			}		
		}
		
		///最后一页不完整，特殊处理：拆分操作公式便于理解！*/
		if(page[1] != (page[0]))//防止首页和尾页重复被反色！
		{
			mask	 =	0xFF;
			temp[0]  =  OLED_GRAM[j][page[1]];
			temp[1]  =  temp[0];
			temp[0]  = ~temp[0];//对字节的低位进行取反！
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
  * @brief	:反色操作(以页为单位反色)
  * @note   :--操作单位为页*列！
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
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
  * @brief	:水平翻转操作(一般实现)
  * @note   :--暂未实现：v7.0版本后暂不支持此函数！-- 2017.12.23 by seri_liang.
  * @param	:*zone, 操作区域
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
  * @brief	:水平翻转操作(以页为单位翻转)
  * @note   :--操作单位为页*列！
  * @param	:pStart, 起始页
             pEnd  , 终止页
             cStart, 起始列
             cEnd  , 终止列
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
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
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










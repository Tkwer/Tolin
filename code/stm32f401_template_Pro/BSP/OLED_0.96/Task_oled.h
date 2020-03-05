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



#ifndef __TASK_OLED_H__
#define __TASK_OLED_H__



/*Driver_oled.h Command Define*/
#define OLED_SIZE_NORMAL					(0)               		//显存大小，正常显示模式，不提供翻页功能(默认)
#define OLED_SIZE_EXPAND          (1)               		//显存大小，扩展显示模式，提供翻页功能

#define OLED_ORIGIN_LEFT_DOWN			(0)               		//原点选择，左下角
#define OLED_ORIGIN_LEFT_UP				(1)               		//原点选择，左上角(默认)


#define OLED_I2C_ADDRESS					(0x78)	        		//硬件地址(固定)
#define	OLED_MAX_LENGTH_OF_DEC		(10)		        	//显示最大十位数的位数
#define OLED_SCREEN_WIDTH					(128)					//显示屏宽度(像素点)
#define OLED_SCREEN_HEIGHT				(64)					//显示屏高度(像素点)
#define OLED_GRAM_SIZE						OLED_SIZE_NORMAL		//显存大小选择
#define OLED_ORIGIN_SET						OLED_ORIGIN_LEFT_UP		//坐标原点选择
#define OLED_ADDRESSING_MODE			OLED_ADDRESSING_P_MODE	//寻址模式

//#define ENABLE_PRINTF_SUPPORT									//printf重定向支持    

//#define ENABLE_EMWIN_SUPPORT									//使能emWin支持



/*Driver_oled.h Needed Library Define*/
#include <stdint.h>
#include <stdio.h>


/*Driver_oled.h Param Class Define*/
///*oled寻址方式枚举*/
typedef enum
{
	SCAN_MODE_H = (uint8_t)0x00,	//水平寻址
	SCAN_MODE_V = (uint8_t)0x01,	//垂直寻址
	SCAN_MODE_P = (uint8_t)0x02,	//页寻址
}_OLEDScanMode;

///*display函数字体大小枚举*/
typedef enum
{
	FONT_0806 = (uint8_t)8,			//
	FONT_1608 = (uint8_t)16,		//
}_OLEDFontSize;

///*display函数显示模式枚举*/
typedef enum
{
	DISP_NORMAL = (uint8_t)1,		//正常模式
	DISP_ANTI	= (uint8_t)0,		//反色模式
}_OLEDDisplayMode;


///*界面操作函数参数枚举*/
typedef enum
{
    DIRECT_UP 	 = (uint8_t)0x29,
    DIRECT_DOWN  = (uint8_t)0x2A,
    DIRECT_LEFT	 = (uint8_t)0x26,
    DIRECT_RIGHT = (uint8_t)0x27,	
}_OLEDScrollDirect;

///*当前句柄*/
typedef struct
{
	uint32_t pixel_row_start;		//坐标系下，x -> row
	uint32_t pixel_row_end;			//
	uint32_t pixel_column_start;	//坐标系下，y -> column
	uint32_t pixel_column_end;		//
}_OLEDZoneHandle;



/*Driver_oled.h Function Define*/
#ifdef ENABLE_PRINTF_SUPPORT   
void oled_goto_xy(uint32_t x, uint32_t y);//printf，设定起始点
#endif


void OLED_InitConfig(void);//oled显示初始化配置

void OLED_DisplayOn(void);//开启oled显示
void OLED_DisplayOff(void);//关闭oled显示
void OLED_SetPos(uint32_t page, uint32_t column);//oled设置显示坐标(页、列)
void OLED_Update(void);//oled更新显示
void OLED_ClearScreen(void);//oled清屏

void OLED_DrawPixelPoint(uint32_t pixelX, uint32_t pixelY, uint32_t writeMode);//画像素点(像素坐标)
uint8_t OLED_ReadPixelPoint(uint32_t pixelX, uint32_t pixelY);//读像素点(像素坐标)


///以下函数在开启emWin后不被支持！！！
#ifndef ENABLE_EMWIN_SUPPORT
_OLEDZoneHandle oled_zone(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//oled坐标点打包函数(像素坐标)
void OLED_UpdatePart(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd, uint32_t direct);//oled局部更新显示

void OLED_DrawPoint(uint32_t pixelX, uint32_t pixelY, uint32_t size, uint32_t writeMode);//画点(像素坐标)
void OLED_DrawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//画线(像素坐标)
void OLED_DrawCircle(uint32_t xc, uint32_t yc, uint32_t r);//改进画圆(像素坐标)
void OLED_DrawEllipse(uint32_t xc, uint32_t yc, uint32_t a, uint32_t b);//画椭圆(像素坐标)
void OLED_DrawBlock(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//画方框(像素坐标)
void OLED_DrawRect(uint32_t xStart, uint32_t yStart, uint32_t length, uint32_t width, uint32_t isFill);//画矩形(像素坐标)
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_SetDisplayMode(_OLEDDisplayMode mode);//设置display类函数的显示模式(覆盖or清除)
void OLED_SetFontSize(_OLEDFontSize fontSize);//设置display类函数的显示字体大小
_OLEDZoneHandle OLED_DisplayCHAR(uint32_t pixelX, uint32_t pixelY, char CHAR);//显示ASCII标准字体字符(像素坐标)
_OLEDZoneHandle OLED_DisplaySTR(uint32_t pixelX, uint32_t pixelY, char *STR);//显示ASCII标准字体字符串(像素坐标)
_OLEDZoneHandle OLED_DisplayHEX(uint32_t pixelX, uint32_t pixelY, uint16_t NUM);//显示转化后的16进制数字(像素坐标)
_OLEDZoneHandle OLED_DisplayUINT(uint32_t pixelX, uint32_t pixelY, uint32_t NUM);//显示无符号的10进制数字(像素坐标)
_OLEDZoneHandle OLED_DisplayINT(uint32_t pixelX, uint32_t pixelY, int32_t NUM);//显示有符号的10进制数字(像素坐标)
_OLEDZoneHandle OLED_DisplayFLOAT(uint32_t pixelX, uint32_t pixelY, float NUM, uint32_t accu);//显示可变精度的小数(像素坐标)

void OLED_MoveStop(void);//停止移屏操作(硬件)
void OLED_Move(uint32_t pStart, uint32_t pEnd, _OLEDScrollDirect direct);//移屏操作(硬件)
void OLED_Move2(uint32_t pStart, uint32_t pEnd , uint32_t cStart, uint32_t cEnd, _OLEDScrollDirect direct, uint32_t loop);//移屏操作(任意区域)
void OLED_Invert(_OLEDZoneHandle *zone);//反色操作(一般实现)
void OLED_Invert2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd);//反色操作(以页为单位反色)
void OLED_Turn2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd);//水平翻转操作(以页为单位翻转)
#endif



#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










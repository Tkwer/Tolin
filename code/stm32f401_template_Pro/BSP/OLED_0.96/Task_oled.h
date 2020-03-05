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



#ifndef __TASK_OLED_H__
#define __TASK_OLED_H__



/*Driver_oled.h Command Define*/
#define OLED_SIZE_NORMAL					(0)               		//�Դ��С��������ʾģʽ�����ṩ��ҳ����(Ĭ��)
#define OLED_SIZE_EXPAND          (1)               		//�Դ��С����չ��ʾģʽ���ṩ��ҳ����

#define OLED_ORIGIN_LEFT_DOWN			(0)               		//ԭ��ѡ�����½�
#define OLED_ORIGIN_LEFT_UP				(1)               		//ԭ��ѡ�����Ͻ�(Ĭ��)


#define OLED_I2C_ADDRESS					(0x78)	        		//Ӳ����ַ(�̶�)
#define	OLED_MAX_LENGTH_OF_DEC		(10)		        	//��ʾ���ʮλ����λ��
#define OLED_SCREEN_WIDTH					(128)					//��ʾ�����(���ص�)
#define OLED_SCREEN_HEIGHT				(64)					//��ʾ���߶�(���ص�)
#define OLED_GRAM_SIZE						OLED_SIZE_NORMAL		//�Դ��Сѡ��
#define OLED_ORIGIN_SET						OLED_ORIGIN_LEFT_UP		//����ԭ��ѡ��
#define OLED_ADDRESSING_MODE			OLED_ADDRESSING_P_MODE	//Ѱַģʽ

//#define ENABLE_PRINTF_SUPPORT									//printf�ض���֧��    

//#define ENABLE_EMWIN_SUPPORT									//ʹ��emWin֧��



/*Driver_oled.h Needed Library Define*/
#include <stdint.h>
#include <stdio.h>


/*Driver_oled.h Param Class Define*/
///*oledѰַ��ʽö��*/
typedef enum
{
	SCAN_MODE_H = (uint8_t)0x00,	//ˮƽѰַ
	SCAN_MODE_V = (uint8_t)0x01,	//��ֱѰַ
	SCAN_MODE_P = (uint8_t)0x02,	//ҳѰַ
}_OLEDScanMode;

///*display���������Сö��*/
typedef enum
{
	FONT_0806 = (uint8_t)8,			//
	FONT_1608 = (uint8_t)16,		//
}_OLEDFontSize;

///*display������ʾģʽö��*/
typedef enum
{
	DISP_NORMAL = (uint8_t)1,		//����ģʽ
	DISP_ANTI	= (uint8_t)0,		//��ɫģʽ
}_OLEDDisplayMode;


///*���������������ö��*/
typedef enum
{
    DIRECT_UP 	 = (uint8_t)0x29,
    DIRECT_DOWN  = (uint8_t)0x2A,
    DIRECT_LEFT	 = (uint8_t)0x26,
    DIRECT_RIGHT = (uint8_t)0x27,	
}_OLEDScrollDirect;

///*��ǰ���*/
typedef struct
{
	uint32_t pixel_row_start;		//����ϵ�£�x -> row
	uint32_t pixel_row_end;			//
	uint32_t pixel_column_start;	//����ϵ�£�y -> column
	uint32_t pixel_column_end;		//
}_OLEDZoneHandle;



/*Driver_oled.h Function Define*/
#ifdef ENABLE_PRINTF_SUPPORT   
void oled_goto_xy(uint32_t x, uint32_t y);//printf���趨��ʼ��
#endif


void OLED_InitConfig(void);//oled��ʾ��ʼ������

void OLED_DisplayOn(void);//����oled��ʾ
void OLED_DisplayOff(void);//�ر�oled��ʾ
void OLED_SetPos(uint32_t page, uint32_t column);//oled������ʾ����(ҳ����)
void OLED_Update(void);//oled������ʾ
void OLED_ClearScreen(void);//oled����

void OLED_DrawPixelPoint(uint32_t pixelX, uint32_t pixelY, uint32_t writeMode);//�����ص�(��������)
uint8_t OLED_ReadPixelPoint(uint32_t pixelX, uint32_t pixelY);//�����ص�(��������)


///���º����ڿ���emWin�󲻱�֧�֣�����
#ifndef ENABLE_EMWIN_SUPPORT
_OLEDZoneHandle oled_zone(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//oled�����������(��������)
void OLED_UpdatePart(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd, uint32_t direct);//oled�ֲ�������ʾ

void OLED_DrawPoint(uint32_t pixelX, uint32_t pixelY, uint32_t size, uint32_t writeMode);//����(��������)
void OLED_DrawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//����(��������)
void OLED_DrawCircle(uint32_t xc, uint32_t yc, uint32_t r);//�Ľ���Բ(��������)
void OLED_DrawEllipse(uint32_t xc, uint32_t yc, uint32_t a, uint32_t b);//����Բ(��������)
void OLED_DrawBlock(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd);//������(��������)
void OLED_DrawRect(uint32_t xStart, uint32_t yStart, uint32_t length, uint32_t width, uint32_t isFill);//������(��������)
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_SetDisplayMode(_OLEDDisplayMode mode);//����display�ຯ������ʾģʽ(����or���)
void OLED_SetFontSize(_OLEDFontSize fontSize);//����display�ຯ������ʾ�����С
_OLEDZoneHandle OLED_DisplayCHAR(uint32_t pixelX, uint32_t pixelY, char CHAR);//��ʾASCII��׼�����ַ�(��������)
_OLEDZoneHandle OLED_DisplaySTR(uint32_t pixelX, uint32_t pixelY, char *STR);//��ʾASCII��׼�����ַ���(��������)
_OLEDZoneHandle OLED_DisplayHEX(uint32_t pixelX, uint32_t pixelY, uint16_t NUM);//��ʾת�����16��������(��������)
_OLEDZoneHandle OLED_DisplayUINT(uint32_t pixelX, uint32_t pixelY, uint32_t NUM);//��ʾ�޷��ŵ�10��������(��������)
_OLEDZoneHandle OLED_DisplayINT(uint32_t pixelX, uint32_t pixelY, int32_t NUM);//��ʾ�з��ŵ�10��������(��������)
_OLEDZoneHandle OLED_DisplayFLOAT(uint32_t pixelX, uint32_t pixelY, float NUM, uint32_t accu);//��ʾ�ɱ侫�ȵ�С��(��������)

void OLED_MoveStop(void);//ֹͣ��������(Ӳ��)
void OLED_Move(uint32_t pStart, uint32_t pEnd, _OLEDScrollDirect direct);//��������(Ӳ��)
void OLED_Move2(uint32_t pStart, uint32_t pEnd , uint32_t cStart, uint32_t cEnd, _OLEDScrollDirect direct, uint32_t loop);//��������(��������)
void OLED_Invert(_OLEDZoneHandle *zone);//��ɫ����(һ��ʵ��)
void OLED_Invert2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd);//��ɫ����(��ҳΪ��λ��ɫ)
void OLED_Turn2(uint32_t pStart, uint32_t pEnd, uint32_t cStart, uint32_t cEnd);//ˮƽ��ת����(��ҳΪ��λ��ת)
#endif



#endif

/* ******************** Copyright (C), 2017-2017, TYUT TSS-plan by SERI.LJI ******************** */










#ifndef _Adafruit_NeoPixel_H
#define _Adafruit_NeoPixel_H

#include "stdint.h"
#include "ws2812b.h"

/*���ȿɵ�                  G R B         */
#define NEO_RED(x)    Color(0,x,0)      //��
#define NEO_GRN(x)    Color(x,0,0)      //��
#define NEO_BLU(x)    Color(0,0,x)      //��
#define NEO_YLW(x)    Color(x,x,0)      //��
#define NEO_MAG(x)    Color(0,x,x)      //���
#define NEO_CYA(x)    Color(x,0,x)      //��
#define NEO_WHI(x)    Color(x,x,x)      //��

//������    grn       
///*�̶�ֵ����*/           //x1,0x40,1
//#define FIX_RED       Color(1,85,1)     //��
//                        // x12,x37,0
//#define FIX_ORG       Color(23,75,0)    //��
//                         // x37,x36,0
//#define FIX_YLW       Color(75,74,1)    //��
//                            //0x28,0,1
//#define FIX_GRN       Color(75,0,9)     //��ɫ
//                         //x28,0,x25   
//#define FIX_CYA       Color(45,0,42)    //����ɫ
//                           //x1,x5,x50 
//#define FIX_BLU       Color(2,7,150)    //��ɫ
//                          //x1,x28,x22  
//#define FIX_PUR       Color(1,75,67)    //��ɫ
//                          //x18,x18,x18
//#define FIX_WHI       Color(30,30,30)   //��ɫ
//#define FIX_BLK       Color(0,0,0)      //��ɫ(��)

/*�̶�ֵ����*/          
#define FIX_RED       Color(0x1,0x40,0x01)  //��
#define FIX_ORG       Color(0x12,0x37,0)    //��
#define FIX_YLW       Color(0x37,0x36,0)    //��
#define FIX_GRN       Color(0x28,0,0x01)    //��ɫ 
#define FIX_CYA       Color(0x28,0,0x25)    //��
#define FIX_BLU       Color(0x01,0x05,0x50) //��ɫ 
#define FIX_PUR       Color(0x01,0x28,0x22) //��ɫ
#define FIX_WHI       Color(0x18,0x18,0x18) //��ɫ
#define FIX_BLK       Color(0,0,0)          //��ɫ(��)

void     setAllPixelColor(uint8_t r, uint8_t g, uint8_t b);
void     SetAllPixelColor(uint32_t c);
void     setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void     SetPixelColor(uint16_t n, uint32_t c);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
uint32_t Wheel(uint8_t WheelPos);
void     rainbow(uint8_t wait);
void     rainbowCycle(uint8_t wait) ;
void     theaterChase(uint32_t c, uint8_t wait);
void     theaterChaseRainbow(uint8_t wait);
void     colorWipe(uint32_t c);
void     WS2812B_Test(void);
void     WS2812B_Init(void);

#endif


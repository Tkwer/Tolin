#ifndef _WS2812B_H
#define _WS2812B_H

#include "tim.h"
#include "dma.h"
#include "sys.h"
#include "Adafruit_NeoPixel.h"

#include "stdint.h"
#define PIXEL_MAX  8
#define FLASH_REFRESH_RATE      (50)                        //led刷新频率
#define FLASH_REFRESH_PERIOD    (1000/FLASH_REFRESH_RATE)   //刷新周期
extern uint8_t rBuffer[PIXEL_MAX];
extern uint8_t gBuffer[PIXEL_MAX];
extern uint8_t bBuffer[PIXEL_MAX];

void    WS2812B_Init(void);
void 		WS2812_Process(uint8_t model);
void    WS2812_set_flash(uint8_t flash,uint16_t angle);

void    WS2812_set_Wheel_1(int16_t speed_L,int16_t speed_R);
void    WS2812B_Test(void);

void    set_micdir(uint16_t data);

void 		WS2812_flash_1(void);
void 		WS2812_show(void);
#endif

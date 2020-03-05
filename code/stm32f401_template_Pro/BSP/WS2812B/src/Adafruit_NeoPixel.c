#include "Adafruit_NeoPixel.h"
#include "delay.h"

extern uint8_t rBuffer[PIXEL_MAX];
extern uint8_t gBuffer[PIXEL_MAX];
extern uint8_t bBuffer[PIXEL_MAX];
extern void WS2812_show(void);
extern uint32_t HAL_GetTick(void);

void  setAllPixelColor(uint8_t g, uint8_t r, uint8_t b)
{
    uint8_t i = 0;
    for(i = 0; i < PIXEL_MAX; i++)
    {
        gBuffer[i] = 0;
        rBuffer[i] = 0;
        bBuffer[i] = 0;
    }
    for(i = 0; i < PIXEL_MAX; i++)
    {
        gBuffer[i] = g;
        rBuffer[i] = r;
        bBuffer[i] = b;
    }

}
void  SetAllPixelColor(uint32_t c)
{
    uint8_t i = 0;
    for(i = 0; i < PIXEL_MAX; i++)
    {
        gBuffer[i] = (uint8_t)(c >> 16);
        rBuffer[i] = (uint8_t)(c >> 8);
        bBuffer[i] = (uint8_t)c;
    }

}
void setPixelColor(uint16_t n, uint8_t g, uint8_t r, uint8_t b)
{
    if(n < PIXEL_MAX)
    {
        gBuffer[n] = g;
        rBuffer[n] = r;
        bBuffer[n] = b;
    }
}
void SetPixelColor(uint16_t n, uint32_t c)
{
    if(n < PIXEL_MAX)
    {
        gBuffer[n] = (uint8_t)(c >> 16);
        rBuffer[n] = (uint8_t)(c >> 8);
        bBuffer[n] = (uint8_t)c;
    }

}

uint32_t Color(uint8_t g, uint8_t r, uint8_t b)
{
    return ((uint32_t)g << 16) | ((uint32_t)r <<  8) | b;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos)
{
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170)
    {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
//2¨ºo?
void rainbow(uint8_t wait)
{
           uint32_t timestamp = HAL_GetTick();
           uint16_t i;
    static uint8_t  j;   
    static uint32_t next_time = 0;
    
    uint32_t flag = 0;  
    if(next_time < wait)
    {
        if((uint64_t)timestamp + wait -  next_time > 0)
            flag = 1;
    }
    else if(timestamp > next_time)
    {
        flag = 1;
    }
    if(flag)// && (timestamp - next_time < wait*5))
    {
        j++;
        next_time = timestamp + wait;
        for(i = 0; i < PIXEL_MAX; i++)
        {
            SetPixelColor(i, Wheel((i + j) & 255));
        }
    }

}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{          
           uint32_t timestamp = HAL_GetTick();
           uint16_t i;
    static uint8_t  j;
    static uint32_t next_time = 0;
    
    static uint8_t  loop = 0;
    if(loop == 0) next_time = timestamp; loop = 1;  //首次调用初始化

    if((timestamp > next_time))// && (timestamp - next_time < wait*5))
    {
        j++;
        next_time = timestamp + wait;
        for(i = 0; i < PIXEL_MAX; i++)
        {
            SetPixelColor(i, Wheel(((i * 256 /(PIXEL_MAX)) + j) & 255));
        }
    }
}
//Theatre-style crawling lights.o??¨¹¦Ì?
void theaterChase(uint32_t c, uint8_t wait)
{
    int i, j, q;
    for (j = 0; j < 10; j++)
    {
        //do 10 cycles of chasing
        for (q = 0; q < 3; q++)
        {
            for ( i = 0; i < PIXEL_MAX; i = i + 1)   //turn every one pixel on
            {
                SetPixelColor(i + q, c);
            }
            WS2812_show();
            delay_ms(wait);

            for ( i = 0; i < PIXEL_MAX; i = i + 1)   //turn every one pixel off
            {
                SetPixelColor(i + q, 0);
            }
            //WS2812_show();
            //delay_ms(wait);
        }
    }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
    int i, j, q;
    for (j = 0; j < 256; j++)
    {
        // cycle all 256 colors in the wheel
        for (q = 0; q < 3; q++)
        {
            for ( i = 0; i < PIXEL_MAX; i = i + 1)   //turn every one pixel on
            {
                SetPixelColor(i + q, Wheel( (i + j) % 255));
            }
            WS2812_show();
            delay_ms(wait);

            for ( i = 0; i < PIXEL_MAX; i = i + 1)   //turn every one pixel off
            {
                SetPixelColor(i + q, 0);
            }
        }
    }
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c)
{
    uint16_t i = 0;
    for( i = 0; i < PIXEL_MAX; i++)
    {
        SetPixelColor(i, c);
    }
}
void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {
  
  //if(whiteLength >= PIXEL_MAX) whiteLength = PIXEL_MAX - 1;

  int head = whiteLength - 1;
  int tail = 0;

  int loops = 3;
  int loopNum = 0;

  //char true = 1;
  static unsigned long lastTime = 0;


  while(1){
    for(int j=0; j<256; j++) {
      for(uint16_t i=0; i<PIXEL_MAX; i++) {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ){
          SetPixelColor(i,NEO_WHI(255));
        }
        else{
          SetPixelColor(i, Wheel(((i * 256 / PIXEL_MAX) + j) & 255));
        }
        
      }

      if(HAL_GetTick() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if(head == PIXEL_MAX){
          loopNum++;
        }
        lastTime = HAL_GetTick();
      }

      if(loopNum == loops) return;
    
      head%=PIXEL_MAX;
      tail%=PIXEL_MAX;
        //strip.show();
        //delay(wait);
    }
  }
  
}

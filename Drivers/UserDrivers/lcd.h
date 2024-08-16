/*
 * file:  lcd.h
 * brief: LCD driver header file
 */
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdint.h"

/* contorl signal port */
#define LCD_RES_PORT  GPIOB
#define LCD_DC_PORT   GPIOB
#define LCD_CS_PORT   GPIOB
#define LCD_BL_PORT   GPIOB
/* spi port */
#define LCD_SPI_PORT  GPIOA
/* contorl signal pin */
#define LCD_RES_PIN   GPIO_PIN_5
#define LCD_DC_PIN    GPIO_PIN_6
#define LCD_CS_PIN    GPIO_PIN_7
#define LCD_BL_PIN    GPIO_PIN_8
/* spi pin */
#define LCD_DATA_PIN  GPIO_PIN_7
#define LCD_CLK_PIN   GPIO_PIN_5

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430
#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
#define LIGHTGREEN     	 0X841F
#define LGRAY 			 0XC618
#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12
#define PURPLE           0X8010

/* set direction */
/* 0: positive  1: negative  2: turn right   3: turn left */
#define USE_HORIZONTAL 2

#if USE_HORIZONTAL==0 | USE_HORIZONTAL==1
    #define LCD_WIDTH  (128 + 2)
    #define LCD_HEIGHT (160 + 1)
#else
    #define LCD_WIDTH  (160 + 1)
    #define LCD_HEIGHT (128 + 2)
#endif

void LCD_Init(void);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void LCD_Show_Char(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
void LCD_Show_String(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
void LCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint16_t len, uint16_t fc, uint16_t bc,uint8_t sizey);
void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);
uint16_t LCD_Draw_Curve(uint8_t xoffset, uint8_t yoffset, int16_t rawvalue);

#ifdef __cplusplus
 }
#endif

#endif /* __LCD_H */
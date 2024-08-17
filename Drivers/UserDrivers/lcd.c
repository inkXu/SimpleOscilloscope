/*
 * file:  lcd.c
 * brief: LCD driver
 */

#include "lcd.h"
#include "gd32e23x.h"
#include "systick.h"
#include "stdint.h"
#include "lcdfonts.h"
#include <stdio.h>

static void LCD_Write_Cmd(uint8_t cmd);
static void LCD_Write_Data(uint16_t data);
static void LCD_Write_Data8(uint8_t data);
static void LCD_Display_Position(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/*
 * @brief:  initialize LCD screen
 * @param:  None
 * @retval: None
 */
void LCD_Init(void)
{
    gpio_bit_reset(LCD_RES_PORT, LCD_RES_PIN);
    delay_1ms(100);
    gpio_bit_set(LCD_RES_PORT, LCD_RES_PIN);
    delay_1ms(100);
    
    gpio_bit_set(LCD_BL_PORT, LCD_BL_PIN);
    delay_1ms(100);
    
	//************* Start Initial Sequence **********//
	LCD_Write_Cmd(0x11); //Sleep out 
	delay_1ms(120);              //Delay 120ms 
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_Write_Cmd(0xB1); 
	LCD_Write_Data8(0x05); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Cmd(0xB2); 
	LCD_Write_Data8(0x05);
	LCD_Write_Data8(0x3C); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Cmd(0xB3); 
	LCD_Write_Data8(0x05); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Data8(0x05); 
	LCD_Write_Data8(0x3C); 
	LCD_Write_Data8(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_Write_Cmd(0xB4); //Dot inversion 
	LCD_Write_Data8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_Write_Cmd(0xC0); 
	LCD_Write_Data8(0x28); 
	LCD_Write_Data8(0x08); 
	LCD_Write_Data8(0x04); 
	LCD_Write_Cmd(0xC1); 
	LCD_Write_Data8(0XC0); 
	LCD_Write_Cmd(0xC2); 
	LCD_Write_Data8(0x0D); 
	LCD_Write_Data8(0x00); 
	LCD_Write_Cmd(0xC3); 
	LCD_Write_Data8(0x8D); 
	LCD_Write_Data8(0x2A); 
	LCD_Write_Cmd(0xC4); 
	LCD_Write_Data8(0x8D); 
	LCD_Write_Data8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	LCD_Write_Cmd(0xC5); //VCOM 
	LCD_Write_Data8(0x1A); 
	LCD_Write_Cmd(0x36); //MX, MY, RGB mode 
	#if USE_HORIZONTAL==0
        LCD_Write_Data8(0x00);
    #elif USE_HORIZONTAL==1
        LCD_Write_Data8(0xC0);
	#elif USE_HORIZONTAL==2
        LCD_Write_Data8(0x70);
	#else
        LCD_Write_Data8(0xA0); 
    #endif
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_Write_Cmd(0xE0); 
	LCD_Write_Data8(0x04); 
	LCD_Write_Data8(0x22); 
	LCD_Write_Data8(0x07); 
	LCD_Write_Data8(0x0A); 
	LCD_Write_Data8(0x2E); 
	LCD_Write_Data8(0x30); 
	LCD_Write_Data8(0x25); 
	LCD_Write_Data8(0x2A); 
	LCD_Write_Data8(0x28); 
	LCD_Write_Data8(0x26); 
	LCD_Write_Data8(0x2E); 
	LCD_Write_Data8(0x3A); 
	LCD_Write_Data8(0x00); 
	LCD_Write_Data8(0x01); 
	LCD_Write_Data8(0x03); 
	LCD_Write_Data8(0x13); 
	LCD_Write_Cmd(0xE1); 
	LCD_Write_Data8(0x04); 
	LCD_Write_Data8(0x16); 
	LCD_Write_Data8(0x06); 
	LCD_Write_Data8(0x0D); 
	LCD_Write_Data8(0x2D); 
	LCD_Write_Data8(0x26); 
	LCD_Write_Data8(0x23); 
	LCD_Write_Data8(0x27); 
	LCD_Write_Data8(0x27); 
	LCD_Write_Data8(0x25); 
	LCD_Write_Data8(0x2D); 
	LCD_Write_Data8(0x3B); 
	LCD_Write_Data8(0x00); 
	LCD_Write_Data8(0x01); 
	LCD_Write_Data8(0x04); 
	LCD_Write_Data8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_Write_Cmd(0x3A); //65k mode, RGB 5-6-5-bit input
	LCD_Write_Data8(0x05); 
	LCD_Write_Cmd(0x29); //Displa
}

/*
 * @brief    transfer data to LCD screen
 * @param    data: 8 bit data
 * @retval   None
 */
void LCD_Write_Data8(uint8_t data)
{
    gpio_bit_reset(LCD_CS_PORT, LCD_CS_PIN);

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
    spi_i2s_data_transmit(SPI0, data);

    gpio_bit_set(LCD_CS_PORT, LCD_CS_PIN);

}

/*
 * @brief    transfer data to LCD screen
 * @param    data: 16 bit data
 * @retval   None
 */
void LCD_Write_Data(uint16_t data)
{
    LCD_Write_Data8(data >> 8);
    LCD_Write_Data8(data & 0XFF);
}

/*
 * @brief    initialize LCD screen
 * @param    cmd: command
 * @retval   None
 */
void LCD_Write_Cmd(uint8_t cmd)
{
    gpio_bit_reset(LCD_DC_PORT, LCD_DC_PIN);
    gpio_bit_reset(LCD_CS_PORT, LCD_CS_PIN);

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
    spi_i2s_data_transmit(SPI0, cmd);

    gpio_bit_set(LCD_CS_PORT, LCD_CS_PIN);
    gpio_bit_set(LCD_DC_PORT, LCD_DC_PIN);
}

/*
 * @brief    set the display position
 * @param    x1, y1: start point
 * @param    x2, y2: end point
 * @retval   None
 */
void LCD_Display_Position(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_Write_Cmd(0x2A);
    LCD_Write_Data(x1);
    LCD_Write_Data(x2);
    LCD_Write_Cmd(0x2B);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2);
    LCD_Write_Cmd(0x2C);
}


void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i=0, j=0;
    LCD_Display_Position(xsta, ysta, xend-1, yend-1);
    for(i = ysta; i < yend; i ++)
    {
        for(j = xsta; j < xend; j ++)
        {
            LCD_Write_Data(color);
        }
    }
}


void LCD_Draw_Point(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_Display_Position(x,y,x,y);
	LCD_Write_Data(color);
} 


void LCD_Draw_Line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;
	else if (delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x;
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_Draw_Point(uRow,uCol,color);
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
	LCD_Draw_Line(x1,y1,x2,y1,color);
	LCD_Draw_Line(x1,y1,x1,y2,color);
	LCD_Draw_Line(x1,y2,x2,y2,color);
	LCD_Draw_Line(x2,y1,x2,y2,color);
}


void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_Draw_Point(x0-b,y0-a,color);             //3           
		LCD_Draw_Point(x0+b,y0-a,color);             //0           
		LCD_Draw_Point(x0-a,y0+b,color);             //1                
		LCD_Draw_Point(x0-a,y0-b,color);             //2             
		LCD_Draw_Point(x0+b,y0+a,color);             //4               
		LCD_Draw_Point(x0+a,y0-b,color);             //5
		LCD_Draw_Point(x0+a,y0+b,color);             //6 
		LCD_Draw_Point(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))
		{
			b--;
		}
	}
}


void LCD_Show_Char(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t temp,sizex,t,m=0;
	uint16_t i,TypefaceNum;
	uint16_t x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';
	LCD_Display_Position(x,y,x+sizex-1,y+sizey-1);
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];
		else if(sizey==16)temp=ascii_1608[num][i];
		else if(sizey==24)temp=ascii_2412[num][i];
		else if(sizey==32)temp=ascii_3216[num][i];
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)
			{
				if(temp&(0x01<<t))LCD_Write_Data(fc);
				else LCD_Write_Data(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else
			{
				if(temp&(0x01<<t)) LCD_Draw_Point(x,y,fc);
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}

void LCD_Show_String(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{         
	while(*p != '\0')
	{       
		LCD_Show_Char(x, y, *p, fc, bc, sizey, mode);
		x += sizey / 2;
		p ++;
	}  
}

uint16_t LCD_Draw_Curve(uint8_t xoffset, uint8_t yoffset, int16_t rawvalue)
{
	static uint8_t  first_point = 1;
	static uint16_t lastX, lastY;
	uint16_t y;
	
	y = (uint16_t)(yoffset - rawvalue);
	if (first_point) {
		first_point = 0;
		LCD_Draw_Point(xoffset, y, GREEN);
		lastX = xoffset;
		lastY = y;
	} else {
		if (lastX < LCD_WIDTH - 2) {
			LCD_Display_Position(lastX + 1, 26, lastX + 1, 94);
			for(int i = 26; i <= 94; i ++) {
				LCD_Write_Data(BLACK);
			}
			LCD_Draw_Line(lastX, lastY, lastX + 1, y, YELLOW);
			lastX ++;
			lastY = y;
		} else {
			for(int i = 26; i <= 94; i ++) {
				LCD_Draw_Point(xoffset, i, BLACK);
			}
			LCD_Draw_Point(xoffset, y, GREEN);
			lastX = xoffset;
			lastY = y;
		}
	}
	return lastX;
}

uint32_t mypow(uint8_t m, uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;
	return result;
}

void LCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint16_t len, uint16_t fc, uint16_t bc,uint8_t sizey)
{         	
	uint8_t t,temp,sizex;
	uint16_t num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_Show_Char(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_Show_Char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}
void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;
	uint8_t sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_Show_Char(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_Show_Char(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}

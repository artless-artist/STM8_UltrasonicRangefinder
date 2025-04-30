#ifndef __IIC_OLED_DRIVER_H
#define __IIC_OLED_DRIVER_H

#include "Delay.h"
#include "stdint.h"
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);

#define SIZE 8
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

#endif
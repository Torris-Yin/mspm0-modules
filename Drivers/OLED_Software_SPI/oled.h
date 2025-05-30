/*
 * SysConfig Configuration Steps:
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_OLED".
 *     3. Name the pins as "PIN_SCL", "PIN_SDA", "PIN_RES", "PIN_DC" and "PIN_CS".
 *     4. Set the pins according to your needs.
 */

#ifndef __OLED_H
#define __OLED_H

#include "ti_msp_dl_config.h" 	 

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#ifndef GPIO_OLED_PIN_SCL_PORT
#define GPIO_OLED_PIN_SCL_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_SDA_PORT
#define GPIO_OLED_PIN_SDA_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_RES_PORT
#define GPIO_OLED_PIN_RES_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_DC_PORT
#define GPIO_OLED_PIN_DC_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_CS_PORT
#define GPIO_OLED_PIN_CS_PORT GPIO_OLED_PORT 
#endif

//----------------------------------------------------------------------------------
//OLED SSD1306 SPI  时钟D0
#define		OLED_SCL_Set()			    (DL_GPIO_setPins(GPIO_OLED_PIN_SCL_PORT, GPIO_OLED_PIN_SCL_PIN))
#define		OLED_SCL_Clr()				(DL_GPIO_clearPins(GPIO_OLED_PIN_SCL_PORT, GPIO_OLED_PIN_SCL_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 SPI 数据D1
#define		OLED_SDA_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_SDA_PORT, GPIO_OLED_PIN_SDA_PIN))
#define		OLED_SDA_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_SDA_PORT, GPIO_OLED_PIN_SDA_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 复位/RES
#define		OLED_RES_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_RES_PORT, GPIO_OLED_PIN_RES_PIN))
#define		OLED_RES_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_RES_PORT, GPIO_OLED_PIN_RES_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 数据/命令DC
#define		OLED_DC_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_DC_PORT, GPIO_OLED_PIN_DC_PIN))
#define		OLED_DC_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_DC_PORT, GPIO_OLED_PIN_DC_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 片选CS
#define		OLED_CS_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_CS_PORT, GPIO_OLED_PIN_CS_PIN))
#define		OLED_CS_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_CS_PORT, GPIO_OLED_PIN_CS_PIN))
					   

//OLED控制用函数
void delay_ms(unsigned int ms);
void OLED_ColorTurn(unsigned char i);
void OLED_DisplayTurn(unsigned char i);
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char sizey);
unsigned int oled_pow(unsigned char m,unsigned char n);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char sizey);
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char sizey);
void OLED_ShowChinese(unsigned char x,unsigned char y,unsigned char no,unsigned char sizey);
void OLED_DrawBMP(unsigned char x,unsigned char y,unsigned char sizex, unsigned char sizey,unsigned char BMP[]);
void OLED_Init(void);

#endif /* #ifndef __OLED_H */

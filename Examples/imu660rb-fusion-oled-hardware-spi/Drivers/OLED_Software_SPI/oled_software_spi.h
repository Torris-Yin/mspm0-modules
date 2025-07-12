/*
 * SysConfig Configuration Steps:
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_OLED".
 *     3. Name the pins as "PIN_OLED_SCL", "PIN_OLED_SDA", "PIN_OLED_RES", "PIN_OLED_DC" and "PIN_OLED_CS".
 *     4. Set the pins according to your needs.
 */

#ifndef __OLED_SOFTWARE_SPI_H
#define __OLED_SOFTWARE_SPI_H

#include "ti_msp_dl_config.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#ifndef GPIO_OLED_PIN_OLED_SCL_PORT
#define GPIO_OLED_PIN_OLED_SCL_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_OLED_SDA_PORT
#define GPIO_OLED_PIN_OLED_SDA_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_OLED_RES_PORT
#define GPIO_OLED_PIN_OLED_RES_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_OLED_DC_PORT
#define GPIO_OLED_PIN_OLED_DC_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_OLED_CS_PORT
#define GPIO_OLED_PIN_OLED_CS_PORT GPIO_OLED_PORT 
#endif

//----------------------------------------------------------------------------------
//OLED SSD1306 SPI  时钟D0
#define		OLED_SCL_Set()			    (DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SCL_PORT, GPIO_OLED_PIN_OLED_SCL_PIN))
#define		OLED_SCL_Clr()				(DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SCL_PORT, GPIO_OLED_PIN_OLED_SCL_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 SPI 数据D1
#define		OLED_SDA_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SDA_PORT, GPIO_OLED_PIN_OLED_SDA_PIN))
#define		OLED_SDA_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SDA_PORT, GPIO_OLED_PIN_OLED_SDA_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 复位/RES
#define		OLED_RES_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_OLED_RES_PORT, GPIO_OLED_PIN_OLED_RES_PIN))
#define		OLED_RES_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_RES_PORT, GPIO_OLED_PIN_OLED_RES_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 数据/命令DC
#define		OLED_DC_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_OLED_DC_PORT, GPIO_OLED_PIN_OLED_DC_PIN))
#define		OLED_DC_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_DC_PORT, GPIO_OLED_PIN_OLED_DC_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 片选CS
#define		OLED_CS_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_OLED_CS_PORT, GPIO_OLED_PIN_OLED_CS_PIN))
#define		OLED_CS_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_CS_PORT, GPIO_OLED_PIN_OLED_CS_PIN))
					   

//OLED控制用函数
void delay_ms(uint32_t ms);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t sizey);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey);
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[]);
void OLED_Init(void);

#endif /* #ifndef __OLED_SOFTWARE_SPI_H */

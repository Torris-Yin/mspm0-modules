/*
 * SysConfig Configuration Steps:
 *   I2C:
 *     1. Add an I2C module.
 *     2. Name it as "I2C_OLED".
 *     3. Check the box "Enable Controller Mode".
 *     4. Set "Standard Bus Speed" to "Fast Mode (400kHz)". (optional)
 *     5. Set the pins according to your needs.
 */
 
#ifndef __OLED_HARDWARE_I2C_H
#define __OLED_HARDWARE_I2C_H

#include "ti_msp_dl_config.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

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
void oled_i2c_sda_unlock(void);

#endif /* #ifndef __OLED_HARDWARE_I2C_H */

/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "main.h"

uint8_t oled_buffer[32];

int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();

    // MPU6050_Init();
    OLED_Init();
    // Ultrasonic_Init();
    // BNO08X_Init();
    // WIT_Init();
    // VL53L0X_Init();
    // LSM6DSV16X_Init();
    IMU660RB_Init();

    /* Don't remove this! */
    Interrupt_Init();

    OLED_ShowString(0,7,(uint8_t *)"IMU660RB Demo",8);

    OLED_ShowString(0,0,(uint8_t *)"Pitch",8);
    OLED_ShowString(0,2,(uint8_t *)" Roll",8);
    OLED_ShowString(0,4,(uint8_t *)"  Yaw",8);

    OLED_ShowString(16*6,3,(uint8_t *)"Accel",8);
    OLED_ShowString(17*6,4,(uint8_t *)"Gyro",8);

    while (1) 
    {
        sprintf((char *)oled_buffer, "%-6.1f", euler.angle.pitch);
        OLED_ShowString(5*8,0,oled_buffer,16);
        sprintf((char *)oled_buffer, "%-6.1f", euler.angle.roll);
        OLED_ShowString(5*8,2,oled_buffer,16);
        sprintf((char *)oled_buffer, "%-6.1f", euler.angle.yaw);
        OLED_ShowString(5*8,4,oled_buffer,16);

        sprintf((char *)oled_buffer, "%6.0f", acceleration_mg[0]/1000.0);
        OLED_ShowString(15*6,0,oled_buffer,8);
        sprintf((char *)oled_buffer, "%6.0f", acceleration_mg[1]/1000.0);
        OLED_ShowString(15*6,1,oled_buffer,8);
        sprintf((char *)oled_buffer, "%6.0f", acceleration_mg[2]/1000.0);
        OLED_ShowString(15*6,2,oled_buffer,8);

        sprintf((char *)oled_buffer, "%6.0f", angular_rate_mdps[0]/1000.0);
        OLED_ShowString(15*6,5,oled_buffer,8);
        sprintf((char *)oled_buffer, "%6.0f", angular_rate_mdps[1]/1000.0);
        OLED_ShowString(15*6,6,oled_buffer,8);
        sprintf((char *)oled_buffer, "%6.0f", angular_rate_mdps[2]/1000.0);
        OLED_ShowString(15*6,7,oled_buffer,8);
    }
}

#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "clock.h"
#include "mpu6050.h"

void SysTick_Handler(void)
{
    tick_ms++;
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
        /* MPU6050 INT */
        #if defined GPIO_MPU6050_INT_IIDX
        case GPIO_MPU6050_INT_IIDX:
            Read_Quad();
            break;
        #endif
    }
}

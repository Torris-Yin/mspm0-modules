#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "clock.h"
#include "mpu6050.h"
#include "bno08x_uart_rvc.h"

void SysTick_Handler(void)
{
    tick_ms++;
}

#if defined UART_BNO08X_INST_IRQHandler
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;
    extern uint8_t dmaBuffer[19];

    DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_CH0_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);

    for(int i=2; i<=14; i++)
        checkSum += dmaBuffer[i];

    if((rxSize == 19) && (dmaBuffer[0] == 0xAA) && (dmaBuffer[1] == 0xAA) && (checkSum == dmaBuffer[18]))
    {
        bno08x_data.index = dmaBuffer[2];
        bno08x_data.yaw = (int16_t)((dmaBuffer[4]<<8)|dmaBuffer[3]) / 100.0;
        bno08x_data.pitch = (int16_t)((dmaBuffer[6]<<8)|dmaBuffer[5]) / 100.0;
        bno08x_data.roll = (int16_t)((dmaBuffer[8]<<8)|dmaBuffer[7]) / 100.0;
        bno08x_data.ax = (dmaBuffer[10]<<8)|dmaBuffer[9];
        bno08x_data.ay = (dmaBuffer[12]<<8)|dmaBuffer[11];
        bno08x_data.az = (dmaBuffer[14]<<8)|dmaBuffer[13];
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
}
#endif

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

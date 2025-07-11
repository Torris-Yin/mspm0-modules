#include "bno08x_uart_rvc.h"

uint8_t bno08x_dmaBuffer[19];

BNO08X_Data_t bno08x_data;

void BNO08X_Init(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t)(&UART_BNO08X_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t) &bno08x_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);

    NVIC_EnableIRQ(UART_BNO08X_INST_INT_IRQN);
}
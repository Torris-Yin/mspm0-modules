#include "bno08x_uart_rvc.h"

uint8_t dmaBuffer[64];

BNO08X_Data_t bno08x_data;

void BNO08X_Init(void)
{
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_BNO08X_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, 63);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);

    NVIC_EnableIRQ(UART_BNO08X_INST_INT_IRQN);
}
/*
 * SysConfig Configuration Steps:
 *   UART:
 *     1. Add an UART module.
 *     2. Name it as "UART_BNO08X".
 *     3. Set "Target Baud Rate" to "115200".
 *     4. Set "Communication Direction " to "RX only".
 *     5. Check the box "Enable FIFOs".
 *     6. Set "RX Timeout Interrupt Counts" to "1".
 *     7. Check the "RX timeout" box at "Enable Interrupts".
 *     8. Set "Configure DMA RX Trigger" to "UART RX interrupt".
 *     9. Set "DMA Channel RX Name" to "DMA_BNO08X".
 *     10. Set "Address Mode" to "Fixed addr. to Block addr.".
 *     11. Set "Source Length" and "Destination Length" to "Byte".
 *     12. Set the pin according to your needs.
 */

#ifndef _BNO08X_UART_RVC_H_
#define _BNO08X_UART_RVC_H_

#include "ti_msp_dl_config.h"

typedef struct {
    uint8_t index;
    float pitch;
    float roll;
    float yaw;
    int16_t ax;
    int16_t ay;
    int16_t az;
} BNO08X_Data_t;

extern BNO08X_Data_t bno08x_data;

void BNO08X_Init(void);

#endif  /* #ifndef _BNO08X_UART_RVC_H_ */
/*
 * SysConfig Configuration Steps:
 *   I2C:
 *     1. Add an I2C module.
 *     2. Name it as "I2C_VL53L0X".
 *     3. Check the box "Enable Controller Mode".
 *     4. Set "Standard Bus Speed" to "Fast Mode (400kHz)". (optional)
 *     5. Set the pins according to your needs.
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_VL53L0X".
 *     3. Name the pins as "PIN_VL53L0X_XSHUT" and "PIN_VL53L0X_GPIO1".
 *     PIN_VL53L0X_GPIO1:
 *       4. Set Direction to "Input".
 *       6. Check the box "Enable Interrupts".
 *       7. Set "Interrupt Priority" to "Level 3 - Lowest".
 *       8. Set "Trigger Polarity" to "Trigger on Falling Edge".
 *       9. Set the pin according to your needs.
 */

#ifndef VL53L0X_H_
#define VL53L0X_H_

#include "ti_msp_dl_config.h"
#include "vl53l0x_api.h"

#ifndef GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PORT
#define GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PORT GPIO_VL53L0X_PORT 
#endif

#ifndef GPIO_VL53L0X_PIN_VL53L0X_GPIO1_PORT
#define GPIO_VL53L0X_PIN_VL53L0X_GPIO1_PORT GPIO_VL53L0X_PORT 
#endif

extern VL53L0X_RangingMeasurementData_t RangingMeasurementData;

void VL53L0X_Init(void);
void Read_VL53L0X(void);

#endif /* #ifndef VL53L0X_H_ */
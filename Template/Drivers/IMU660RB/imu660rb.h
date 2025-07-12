/*
 * SysConfig Configuration Steps:
 *   SPI:
 *     1. Add a SPI module.
 *     2. Name it as "SPI_IMU660RB".
 *     3. Set "Frame Format" to "Motorola 3-wire".
 *     4. Set the pins according to your needs.
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_IMU660RB".
 *     3. Name the pins as "PIN_IMU660RB_CS" and "PIN_IMU660RB_INT1".
 *     PIN_IMU660RB_INT1:
 *       4. Set Direction to "Input".
 *       6. Check the box "Enable Interrupts".
 *       7. Set "Interrupt Priority" to "Level 3 - Lowest".
 *       8. Set "Trigger Polarity" to "Trigger on Rising Edge".
 *       9. Set the pin according to your needs.
 */

#ifndef _IMU660RB_H_
#define _IMU660RB_H_

#include "ti_msp_dl_config.h"
#include "Fusion.h"

#ifndef GPIO_IMU660RB_PIN_IMU660RB_CS_PORT
#define GPIO_IMU660RB_PIN_IMU660RB_CS_PORT GPIO_IMU660RB_PORT 
#endif

#ifndef GPIO_IMU660RB_PIN_IMU660RB_INT1_PORT
#define GPIO_IMU660RB_PIN_IMU660RB_INT1_PORT GPIO_IMU660RB_PORT 
#endif

extern float acceleration_mg[3];
extern float angular_rate_mdps[3];

extern FusionAhrs ahrs;
extern FusionEuler euler;

void IMU660RB_Init(void);
void Read_IMU660RB(void);

#endif  /* #ifndef _IMU660RB_H_ */
/*
 * SysConfig Configuration Steps:
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_ULTRASONIC".
 *     3. Name the pins as "PIN_ULTRASONIC_TRIG" and "PIN_ULTRASONIC_ECHO".
 *     4. Set the "Direction" of "PIN_ULTRASONIC_ECHO" to "Input".
 *     5. Set the pins according to your needs.
 *   TIMER:
 *     1. Add a TIMER module.
 *     2. Name the group as "TIMER_ULTRASONIC".
 *     3. Set the "Timer Clock Prescaler" to "80".
 *     4. Set "Timer Mode" to "One-shot Up Counting".
 *     5. Set "Desired Timer Period" to "40 ms".
 *     6. Choose the timer according to your needs.
 */

#ifndef __ULTRASONIC_GPIO_H
#define __ULTRASONIC_GPIO_H

#include "ti_msp_dl_config.h"

#ifndef GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PORT
#define GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PORT GPIO_ULTRASONIC_PORT 
#endif

#ifndef GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PORT
#define GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PORT GPIO_ULTRASONIC_PORT 
#endif

void Ultrasonic_Init(void);
uint16_t Read_Ultrasonic(void);

#endif /* #ifndef __ULTRASONIC_GPIO_H */
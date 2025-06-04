/*
 * SysConfig Configuration Steps:
 *   TIMER-PWM:
 *     1. Add a TIMER-PWM module.
 *     2. Name the group as "PWM_ULTRASONIC".
 *     3. Set the "Timer Clock Prescaler" to "250".
 *     4. Set the "PWM Period Count" to "64000".
 *     5. Check the box "Start Timer".
 *     6. Set the "PWM Channel(s)" to "PWM Channel 0".
 *     7. Set the "Desired Duty Cycle (%)" to "50".
 *     8. Set the pin according to your needs.
 *   TIMER-CAPTURE:
 *     1. Add a TIMER-CAPTURE module.
 *     2. Name the group as "CAPTURE_ULTRASONIC".
 *     3. Set the "Timer Clock Prescaler" to "80".
 *     4. Set the "Desired Timer Period" to "50 ms".
 *     5. Set the "Capture Mode" to "Pulse-Width Capture Up Counting".
 *     6. Set the pin according to your needs.
 */

#ifndef __ULTRASONIC_CAPTURE_H
#define __ULTRASONIC_CAPTURE_H

#include "ti_msp_dl_config.h"

#if defined GPIO_CAPTURE_ULTRASONIC_C0_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_0_INDEX
#elif defined GPIO_CAPTURE_ULTRASONIC_C1_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_1_INDEX
#elif defined GPIO_CAPTURE_ULTRASONIC_C2_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_2_INDEX
#elif defined GPIO_CAPTURE_ULTRASONIC_C3_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_3_INDEX
#elif defined GPIO_CAPTURE_ULTRASONIC_C4_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_4_INDEX
#elif defined GPIO_CAPTURE_ULTRASONIC_C5_PORT
#define CAPTURE_ULTRASONIC_CC_IDX DL_TIMER_CC_5_INDEX
#endif

void Ultrasonic_Init(void);
uint16_t Read_Ultrasonic(void);

#endif /* #ifndef __ULTRASONIC_CAPTURE_H */
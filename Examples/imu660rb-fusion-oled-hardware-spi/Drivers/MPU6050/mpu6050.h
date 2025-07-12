/*
 * SysConfig Configuration Steps:
 *   I2C:
 *     1. Add an I2C module.
 *     2. Name it as "I2C_MPU6050".
 *     3. Check the box "Enable Controller Mode".
 *     4. Set Standard Bus Speed to "Fast Mode (400kHz)". (optional)
 *     5. Set the pins according to your needs.
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_MPU6050".
 *     3. Name the pin as "PIN_MPU6050_INT".
 *     4. Set Direction to "Input".
 *     5. Set "Internal Resistor" to "Pull-Up Resistor".
 *     6. Check the box "Enable Interrupts".
 *     7. Set "Interrupt Priority" to "Level 3 - Lowest".
 *     8. Set "Trigger Polarity" to "Trigger on Falling Edge".
 *     9. Set the pin according to your needs.
 */

#ifndef _MPU6050_H_
#define _MPU6050_H_

extern short gyro[3], accel[3];
extern float pitch, roll, yaw;

void MPU6050_Init(void);
int Read_Quad(void);

#endif  /* #ifndef _MPU6050_H_ */
#ifndef _MSPM0_I2C_H_
#define _MSPM0_I2C_H_

void mpu6050_i2c_sda_unlock(void);

int mspm0_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data);

int mspm0_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data);

#endif  /* #ifndef _MSPM0_I2C_H_ */

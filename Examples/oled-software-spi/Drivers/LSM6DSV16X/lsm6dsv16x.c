#include "lsm6dsv16x.h"
#include "lsm6dsv16x_reg.h"

#include "ti_msp_dl_config.h"
#include "clock.h"
#include "interrupt.h"
#include "string.h"

#define BOOT_TIME         (10)
#define I2C_TIMEOUT_MS    (10)
#define RAD_TO_DEG        (180.0f / M_PI)

#define LSM6DSV16X_ADDR   (0x6A)

static uint8_t whoamI;
static lsm6dsv16x_fifo_sflp_raw_t fifo_sflp;

lsm6dsv16x_fifo_status_t fifo_status;
stmdev_ctx_t dev_ctx;
lsm6dsv16x_reset_t rst;
lsm6dsv16x_pin_int_route_t pin_int;

float_t quat[4];
short gyro[3], accel[3];
float pitch, roll, yaw;

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void platform_delay(uint32_t ms);

static void quat_to_euler(const float q[4], float *roll, float *pitch, float *yaw)
{
    // q = [x, y, z, w] 格式
    float x = q[0];
    float y = q[1];
    float z = q[2];
    float w = q[3];

    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = 2.0f * (w * y - z * x);
    if (fabsf(sinp) >= 1.0f)
        *pitch = copysignf(M_PI / 2.0f, sinp); // use 90 degrees if out of range
    else
        *pitch = asinf(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    *yaw = atan2f(siny_cosp, cosy_cosp);

    *roll *= RAD_TO_DEG;
    *pitch *= RAD_TO_DEG;
    *yaw *= RAD_TO_DEG;
}

static float_t npy_half_to_float(uint16_t h)
{
    union { float_t ret; uint32_t retbits; } conv;
    conv.retbits = lsm6dsv16x_from_f16_to_f32(h);
    return conv.ret;
}

static void sflp2q(float_t quat[4], uint8_t raw[6])
{
    float_t sumsq = 0;
    uint16_t sflp[3];

    memcpy(&sflp[0], &raw[0], 2);
    memcpy(&sflp[1], &raw[2], 2);
    memcpy(&sflp[2], &raw[4], 2);

    quat[0] = npy_half_to_float(sflp[0]);
    quat[1] = npy_half_to_float(sflp[1]);
    quat[2] = npy_half_to_float(sflp[2]);

    for (uint8_t i = 0; i < 3; i++)
        sumsq += quat[i] * quat[i];

    if (sumsq > 1.0f) {
        float_t n = sqrtf(sumsq);
        quat[0] /= n;
        quat[1] /= n;
        quat[2] /= n;
        sumsq = 1.0f;
    }

    quat[3] = sqrtf(1.0f - sumsq);
}

void LSM6DSV16X_Init(void)
{
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;

    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);

    /* Check device ID */
    lsm6dsv16x_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LSM6DSV16X_ID)
    {
        return;
    }

    /* Restore default configuration */
    lsm6dsv16x_reset_set(&dev_ctx, LSM6DSV16X_GLOBAL_RST);
    do {
        lsm6dsv16x_reset_get(&dev_ctx, &rst);
    } while (rst != LSM6DSV16X_READY);

    /* Enable Block Data Update */
    lsm6dsv16x_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    /* Set full scale */
    lsm6dsv16x_xl_full_scale_set(&dev_ctx, LSM6DSV16X_4g);
    lsm6dsv16x_gy_full_scale_set(&dev_ctx, LSM6DSV16X_2000dps);

    /* Set FIFO batch of sflp data */
    fifo_sflp.game_rotation = 1;
    fifo_sflp.gravity = 0;
    fifo_sflp.gbias = 0;
    lsm6dsv16x_fifo_sflp_batch_set(&dev_ctx, fifo_sflp);
    lsm6dsv16x_fifo_xl_batch_set(&dev_ctx, LSM6DSV16X_XL_BATCHED_AT_60Hz);
    lsm6dsv16x_fifo_gy_batch_set(&dev_ctx, LSM6DSV16X_GY_BATCHED_AT_60Hz);

    /* Set FIFO mode to Stream mode (aka Continuous Mode) */
    lsm6dsv16x_fifo_mode_set(&dev_ctx, LSM6DSV16X_STREAM_MODE);

    /* Set Output Data Rate */
    lsm6dsv16x_xl_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_60Hz);
    lsm6dsv16x_gy_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_60Hz);
    lsm6dsv16x_sflp_data_rate_set(&dev_ctx, LSM6DSV16X_SFLP_60Hz);
    lsm6dsv16x_sflp_game_rotation_set(&dev_ctx, PROPERTY_ENABLE);

    pin_int.drdy_xl = PROPERTY_ENABLE;
    lsm6dsv16x_pin_int2_route_set(&dev_ctx, &pin_int);
    lsm6dsv16x_data_ready_mode_set(&dev_ctx, LSM6DSV16X_DRDY_PULSED);

    /* Enable INT_GROUP1 handler. */
    enable_group1_irq = 1;
}

void Read_LSM6DSV16X(void)
{
    uint16_t num;
    int16_t datax, datay, dataz;
    lsm6dsv16x_fifo_status_t fifo_status;

    /* Read watermark flag */
    lsm6dsv16x_fifo_status_get(&dev_ctx, &fifo_status);
    num = fifo_status.fifo_level;

    while (num--) 
    {
        lsm6dsv16x_fifo_out_raw_t f_data;
        uint8_t *axis;

        /* Read FIFO sensor value */
        lsm6dsv16x_fifo_out_raw_get(&dev_ctx, &f_data);
        memcpy(&datax, &f_data.data[0], 2);
        memcpy(&datay, &f_data.data[2], 2);
        memcpy(&dataz, &f_data.data[4], 2);

        switch (f_data.tag) 
        {
            case LSM6DSV16X_SFLP_GAME_ROTATION_VECTOR_TAG:
                sflp2q(quat, &f_data.data[0]);
                quat_to_euler(quat, &roll, &pitch, &yaw);
                break;
            case LSM6DSV16X_XL_NC_TAG:
                accel[0] = lsm6dsv16x_from_fs4_to_mg(datax);
                accel[1] = lsm6dsv16x_from_fs4_to_mg(datay);
                accel[2] = lsm6dsv16x_from_fs4_to_mg(dataz);
                break;
            case LSM6DSV16X_GY_NC_TAG:
                gyro[0] = lsm6dsv16x_from_fs2000_to_mdps(datax)/1000;
                gyro[1] = lsm6dsv16x_from_fs2000_to_mdps(datay)/1000;
                gyro[2] = lsm6dsv16x_from_fs2000_to_mdps(dataz)/1000;
                break;
            default:
                break;
        }
    }
}

static int mspm0_i2c_disable(void)
{
    DL_I2C_reset(I2C_LSM6DSV16X_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_LSM6DSV16X_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_LSM6DSV16X_IOMUX_SDA,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_LSM6DSV16X_SCL_PORT, GPIO_I2C_LSM6DSV16X_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_LSM6DSV16X_SCL_PORT, GPIO_I2C_LSM6DSV16X_SCL_PIN);
    return 0;
}

static int mspm0_i2c_enable(void)
{
    DL_I2C_reset(I2C_LSM6DSV16X_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_LSM6DSV16X_IOMUX_SDA,
        GPIO_I2C_LSM6DSV16X_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_LSM6DSV16X_IOMUX_SCL,
        GPIO_I2C_LSM6DSV16X_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_LSM6DSV16X_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_LSM6DSV16X_IOMUX_SCL);
    DL_I2C_enablePower(I2C_LSM6DSV16X_INST);
    SYSCFG_DL_I2C_LSM6DSV16X_init();
    return 0;
}

static void mspm0_i2c_sda_unlock(void)
{
    uint8_t cycleCnt = 0;
    mspm0_i2c_disable();
    do
    {
        DL_GPIO_clearPins(GPIO_I2C_LSM6DSV16X_SCL_PORT, GPIO_I2C_LSM6DSV16X_SCL_PIN);
        mspm0_delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_LSM6DSV16X_SCL_PORT, GPIO_I2C_LSM6DSV16X_SCL_PIN);
        mspm0_delay_ms(1);

        if(DL_GPIO_readPins(GPIO_I2C_LSM6DSV16X_SDA_PORT, GPIO_I2C_LSM6DSV16X_SDA_PIN))
            break;
    }while(++cycleCnt < 100);
    mspm0_i2c_enable();
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
    unsigned int cnt = len;
    unsigned char const *ptr = bufp;
    unsigned long start, cur;

    if (!len)
        return 0;

    mspm0_get_clock_ms(&start);

    DL_I2C_transmitControllerData(I2C_LSM6DSV16X_INST, reg);
    DL_I2C_clearInterruptStatus(I2C_LSM6DSV16X_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_LSM6DSV16X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_LSM6DSV16X_INST, LSM6DSV16X_ADDR, DL_I2C_CONTROLLER_DIRECTION_TX, len+1);

    do {
        unsigned fillcnt;
        fillcnt = DL_I2C_fillControllerTXFIFO(I2C_LSM6DSV16X_INST, ptr, cnt);
        cnt -= fillcnt;
        ptr += fillcnt;

        mspm0_get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            mspm0_i2c_sda_unlock();
            return -1;
        }
    } while (!DL_I2C_getRawInterruptStatus(I2C_LSM6DSV16X_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

    return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    unsigned i = 0;
    unsigned long start, cur;

    if (!len)
        return 0;

    mspm0_get_clock_ms(&start);

    DL_I2C_transmitControllerData(I2C_LSM6DSV16X_INST, reg);
    I2C_LSM6DSV16X_INST->MASTER.MCTR = I2C_MCTR_RD_ON_TXEMPTY_ENABLE;
    DL_I2C_clearInterruptStatus(I2C_LSM6DSV16X_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_LSM6DSV16X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_LSM6DSV16X_INST, LSM6DSV16X_ADDR, DL_I2C_CONTROLLER_DIRECTION_RX, len);

    do {
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_LSM6DSV16X_INST))
        {
            uint8_t c;
            c = DL_I2C_receiveControllerData(I2C_LSM6DSV16X_INST);
            if (i < len)
            {
                bufp[i] = c;
                ++i;
            }
        }
        
        mspm0_get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            mspm0_i2c_sda_unlock();
            return -1;
        }
    } while(!DL_I2C_getRawInterruptStatus(I2C_LSM6DSV16X_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

    if (!DL_I2C_isControllerRXFIFOEmpty(I2C_LSM6DSV16X_INST))
    {
        uint8_t c;
        c = DL_I2C_receiveControllerData(I2C_LSM6DSV16X_INST);
        if (i < len)
        {
            bufp[i] = c;
            ++i;
        }
    }

    I2C_LSM6DSV16X_INST->MASTER.MCTR = 0;
    DL_I2C_flushControllerTXFIFO(I2C_LSM6DSV16X_INST);

    if(i == len)
        return 0;
    else
        return -1;
    return 0;
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
    mspm0_delay_ms(ms);
}

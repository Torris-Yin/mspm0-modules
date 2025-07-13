#include "imu660rb.h"
#include "lsm6dsr_reg.h"

#include "ti_msp_dl_config.h"
#include "clock.h"
#include "interrupt.h"

#define BOOT_TIME         (10)
#define OFFSET_CAL_TIME   (50)

#define ODR_COEFF_12Hz5   (512)
#define ODR_COEFF_26Hz    (256)
#define ODR_COEFF_52Hz    (128)
#define ODR_COEFF_104Hz   (64)
#define ODR_COEFF_208Hz   (32)
#define ODR_COEFF_416Hz   (16)
#define ODR_COEFF_833Hz   (8)
#define ODR_COEFF_1667Hz  (4)
#define ODR_COEFF_3333Hz  (2)
#define ODR_COEFF_6667Hz  (1)

static stmdev_ctx_t dev_ctx;

float acceleration_mg[3];
float angular_rate_mdps[3];

static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static uint8_t whoamI, rst;
static float samplePeriod, sampleRate;

static FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
static FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
static FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};

FusionAhrs ahrs;
FusionEuler euler;
static FusionOffset offset;

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void platform_delay(uint32_t ms);

void IMU660RB_Init(void)
{
    lsm6dsr_pin_int1_route_t int1_route;
    uint8_t offset_cnt;
    int8_t freq_fine;

    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;

    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);
    /* Check device ID */
    lsm6dsr_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LSM6DSR_ID)
        return;

    /* Restore default configuration */
    lsm6dsr_reset_set(&dev_ctx, PROPERTY_ENABLE);

    do {
        lsm6dsr_reset_get(&dev_ctx, &rst);
    } while (rst);

    /* Disable I3C interface */
    lsm6dsr_i3c_disable_set(&dev_ctx, LSM6DSR_I3C_DISABLE);

    /* Enable Block Data Update */
    lsm6dsr_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

    /* Set Output Data Rate */
    lsm6dsr_xl_data_rate_set(&dev_ctx, LSM6DSR_XL_ODR_52Hz);
    lsm6dsr_gy_data_rate_set(&dev_ctx, LSM6DSR_GY_ODR_52Hz);

    /* Set full scale */
    lsm6dsr_xl_full_scale_set(&dev_ctx, LSM6DSR_2g);
    lsm6dsr_gy_full_scale_set(&dev_ctx, LSM6DSR_2000dps);
    
    lsm6dsr_gy_filter_lp1_set(&dev_ctx, 1);

    lsm6dsr_pin_int1_route_get(&dev_ctx, &int1_route);
    int1_route.int1_ctrl.int1_drdy_xl = PROPERTY_ENABLE;
    lsm6dsr_pin_int1_route_set(&dev_ctx, &int1_route);
    lsm6dsr_data_ready_mode_set(&dev_ctx, LSM6DSR_DRDY_PULSED);

    lsm6dsr_odr_cal_reg_get(&dev_ctx, &freq_fine);
    sampleRate = (6667 + ((0.0015 * freq_fine) * 6667)) / ODR_COEFF_52Hz;
    samplePeriod = 1.0 / sampleRate;

    /* Enable INT_GROUP1 handler. */
    enable_group1_irq = 1;

    FusionAhrsInitialise(&ahrs);
    FusionOffsetInitialise(&offset, sampleRate);

    platform_delay(200);

    offset_cnt = OFFSET_CAL_TIME;
    while(offset_cnt)
    {
        uint8_t reg;
        lsm6dsr_gy_flag_data_ready_get(&dev_ctx, &reg);

        if (reg)
        {
            offset_cnt--;
            lsm6dsr_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);

            angular_rate_mdps[0] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[0]);
            angular_rate_mdps[1] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[1]);
            angular_rate_mdps[2] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[2]);

            gyroscopeOffset.array[0] += angular_rate_mdps[0] / 1000.0;
            gyroscopeOffset.array[1] += angular_rate_mdps[1] / 1000.0;
            gyroscopeOffset.array[2] += angular_rate_mdps[2] / 1000.0;
        }
    }

    gyroscopeOffset.array[0] /= OFFSET_CAL_TIME;
    gyroscopeOffset.array[1] /= OFFSET_CAL_TIME;
    gyroscopeOffset.array[2] /= OFFSET_CAL_TIME;
}

void Read_IMU660RB(void)
{
    lsm6dsr_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
    acceleration_mg[0] = lsm6dsr_from_fs2g_to_mg(data_raw_acceleration[0]);
    acceleration_mg[1] = lsm6dsr_from_fs2g_to_mg(data_raw_acceleration[1]);
    acceleration_mg[2] = lsm6dsr_from_fs2g_to_mg(data_raw_acceleration[2]);

    lsm6dsr_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
    angular_rate_mdps[0] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[0]);
    angular_rate_mdps[1] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[1]);
    angular_rate_mdps[2] = lsm6dsr_from_fs2000dps_to_mdps(data_raw_angular_rate[2]);

    FusionVector accelerometer = {acceleration_mg[0]/1000.0, acceleration_mg[1]/1000.0, acceleration_mg[2]/1000.0};
    FusionVector gyroscope = {angular_rate_mdps[0]/1000.0, angular_rate_mdps[1]/1000.0, angular_rate_mdps[2]/1000.0};

    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, samplePeriod);
    euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
}

static uint8_t spiTransferByte(const uint8_t data)
{
    uint8_t read_data = 0;

    DL_SPI_transmitData8(SPI_IMU660RB_INST, data);
    while(DL_SPI_isRXFIFOEmpty(SPI_IMU660RB_INST));
    read_data = DL_SPI_receiveData8(SPI_IMU660RB_INST);
    while(DL_SPI_isBusy(SPI_IMU660RB_INST));

    return read_data;
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
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
    DL_GPIO_clearPins(GPIO_IMU660RB_PIN_IMU660RB_CS_PORT, GPIO_IMU660RB_PIN_IMU660RB_CS_PIN);

    spiTransferByte(reg);

    while(len--)
    {
        spiTransferByte(*bufp++);
    }

    DL_GPIO_setPins(GPIO_IMU660RB_PIN_IMU660RB_CS_PORT, GPIO_IMU660RB_PIN_IMU660RB_CS_PIN);

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
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
    DL_GPIO_clearPins(GPIO_IMU660RB_PIN_IMU660RB_CS_PORT, GPIO_IMU660RB_PIN_IMU660RB_CS_PIN);

    reg |= 0x80;
    spiTransferByte(reg);

    while(len--)
    {
        *bufp++ = spiTransferByte(0);
    }
        
    DL_GPIO_setPins(GPIO_IMU660RB_PIN_IMU660RB_CS_PORT, GPIO_IMU660RB_PIN_IMU660RB_CS_PIN);

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

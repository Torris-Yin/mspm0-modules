#include "vl53l0x.h"
#include "vl53l0x_api.h"
#include "clock.h"
#include "interrupt.h"

typedef enum {
	LONG_RANGE 		= 0, /*!< Long range mode */
	HIGH_SPEED 		= 1, /*!< High speed mode */
	HIGH_ACCURACY	= 2, /*!< High accuracy mode */
} RangingConfig_e;

RangingConfig_e RangingConfig = HIGH_ACCURACY;

VL53L0X_Dev_t VL53L0XDevs[]={
        {.Id=0, .I2cDevAddr=0x29, .Present=1},
};

VL53L0X_RangingMeasurementData_t RangingMeasurementData;

void VL53L0X_Init(void)
{
    uint16_t Id;
    int status = 0;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;

    FixPoint1616_t signalLimit = (FixPoint1616_t)(0.25*65536);
	FixPoint1616_t sigmaLimit = (FixPoint1616_t)(18*65536);
	uint32_t timingBudget = 33000;
	uint8_t preRangeVcselPeriod = 14;
	uint8_t finalRangeVcselPeriod = 10;

    VL53L0X_Dev_t *pDev;
    pDev = &VL53L0XDevs[0];

    DL_GPIO_clearPins(GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PORT, GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PIN);
    mspm0_delay_ms(1);
    DL_GPIO_setPins(GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PORT, GPIO_VL53L0X_PIN_VL53L0X_XSHUT_PIN);
    mspm0_delay_ms(1);

    status += VL53L0X_WrByte(pDev, 0x88, 0x00);
    status += VL53L0X_RdWord(pDev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
    status += VL53L0X_DataInit(pDev);
    status += VL53L0X_StaticInit(pDev);
    status += VL53L0X_PerformRefSpadManagement(pDev, &refSpadCount, &isApertureSpads);
    status += VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);

    status += VL53L0X_SetDeviceMode(pDev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    status += VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    status += VL53L0X_SetLimitCheckEnable(pDev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);

    /* Ranging configuration */
    switch(RangingConfig) 
    {
        case LONG_RANGE:
            signalLimit = (FixPoint1616_t)(0.1*65536);
            sigmaLimit = (FixPoint1616_t)(60*65536);
            timingBudget = 33000;
            preRangeVcselPeriod = 18;
            finalRangeVcselPeriod = 14;
            break;
        case HIGH_ACCURACY:
            signalLimit = (FixPoint1616_t)(0.25*65536);
            sigmaLimit = (FixPoint1616_t)(18*65536);
            timingBudget = 200000;
            preRangeVcselPeriod = 14;
            finalRangeVcselPeriod = 10;
            break;
        case HIGH_SPEED:
            signalLimit = (FixPoint1616_t)(0.25*65536);
            sigmaLimit = (FixPoint1616_t)(32*65536);
            timingBudget = 20000;
            preRangeVcselPeriod = 14;
            finalRangeVcselPeriod = 10;
            break;
    }

    status += VL53L0X_SetLimitCheckValue(pDev,  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
    status += VL53L0X_SetLimitCheckValue(pDev,  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
    status += VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pDev,  timingBudget);
    status += VL53L0X_SetVcselPulsePeriod(pDev,  VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
    status += VL53L0X_SetVcselPulsePeriod(pDev,  VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
    status += VL53L0X_PerformRefCalibration(pDev, &VhvSettings, &PhaseCal);

    status += VL53L0X_SetGpioConfig(pDev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY, VL53L0X_INTERRUPTPOLARITY_LOW);
    status += VL53L0X_ClearInterruptMask(pDev, 0);

    if (status || (Id != 0xEEAA))
        return;

    /* Enable INT_GROUP1 handler. */
    enable_group1_irq = 1;

    VL53L0X_StartMeasurement(pDev);
}

void Read_VL53L0X(void)
{
    VL53L0X_Dev_t *pDev;
    pDev = &VL53L0XDevs[0];

    VL53L0X_GetRangingMeasurementData(pDev, &RangingMeasurementData);
    VL53L0X_ClearInterruptMask(pDev, 0);
}

//?????#include "hal.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_api.h"

#include "ti_msp_dl_config.h"
#include <string.h>
#include "clock.h"

#define I2C_TIME_OUT_BASE   10
#define I2C_TIME_OUT_BYTE   1
#define VL53L0X_OsDelay(...) mspm0_delay_ms(2)

//extern I2C_HandleTypeDef hi2c1;
//#define VL53L0X_pI2cHandle    (&hi2c1)

/* when not customized by application define dummy one */
#ifndef VL53L0X_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L0X_GetI2cBus(...) (void)0
#endif

#ifndef VL53L0X_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L0X_PutI2cBus(...) (void)0
#endif

#ifndef VL53L0X_OsDelay
#   define  VL53L0X_OsDelay(...) (void)0
#endif


uint8_t _I2CBuffer[64];

void _I2CDisable(void)
{
    DL_I2C_reset(I2C_VL53L0X_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_VL53L0X_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_VL53L0X_IOMUX_SDA,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_VL53L0X_SCL_PORT, GPIO_I2C_VL53L0X_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_VL53L0X_SCL_PORT, GPIO_I2C_VL53L0X_SCL_PIN);
}

void _I2CEnable(void)
{
    DL_I2C_reset(I2C_VL53L0X_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_VL53L0X_IOMUX_SDA,
        GPIO_I2C_VL53L0X_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_VL53L0X_IOMUX_SCL,
        GPIO_I2C_VL53L0X_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_VL53L0X_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_VL53L0X_IOMUX_SCL);
    DL_I2C_enablePower(I2C_VL53L0X_INST);
    SYSCFG_DL_I2C_VL53L0X_init();
}

void _I2CUnlock(void)
{
    uint8_t cycleCnt = 0;
    _I2CDisable();
    do
    {
        DL_GPIO_clearPins(GPIO_I2C_VL53L0X_SCL_PORT, GPIO_I2C_VL53L0X_SCL_PIN);
        mspm0_delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_VL53L0X_SCL_PORT, GPIO_I2C_VL53L0X_SCL_PIN);
        mspm0_delay_ms(1);

        if(DL_GPIO_readPins(GPIO_I2C_VL53L0X_SDA_PORT, GPIO_I2C_VL53L0X_SDA_PIN))
            break;
    }while(++cycleCnt < 100);
    _I2CEnable();
}

int _I2CWrite(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count)
{
    int i2c_time_out = I2C_TIME_OUT_BASE+ count* I2C_TIME_OUT_BYTE;
    unsigned int cnt = count;
    unsigned char const *ptr = pdata;
    unsigned long start, cur;

    if (!pdata)
        return 0;

    mspm0_get_clock_ms(&start);

    DL_I2C_clearInterruptStatus(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_VL53L0X_INST, Dev->I2cDevAddr, DL_I2C_CONTROLLER_DIRECTION_TX, count);

    do {
        unsigned fillcnt;
        fillcnt = DL_I2C_fillControllerTXFIFO(I2C_VL53L0X_INST, ptr, cnt);
        cnt -= fillcnt;
        ptr += fillcnt;

        mspm0_get_clock_ms(&cur);
        if(cur >= (start + i2c_time_out))
        {
            _I2CUnlock();
            return -1;
        }
    } while (!DL_I2C_getRawInterruptStatus(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

    return 0;
}

int _I2CRead(VL53L0X_DEV Dev, uint8_t *pdata, uint32_t count)
{
    int status;
    int i2c_time_out = I2C_TIME_OUT_BASE+ count* I2C_TIME_OUT_BYTE;
    unsigned i = 0;
    unsigned long start, cur;

    if (!count)
        return 0;

    mspm0_get_clock_ms(&start);

    DL_I2C_clearInterruptStatus(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);

    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_startControllerTransfer(I2C_VL53L0X_INST, Dev->I2cDevAddr|1, DL_I2C_CONTROLLER_DIRECTION_RX, count);

    do {
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_VL53L0X_INST))
        {
            uint8_t c;
            c = DL_I2C_receiveControllerData(I2C_VL53L0X_INST);
            if (i < count)
            {
                pdata[i] = c;
                ++i;
            }
        }
        
        mspm0_get_clock_ms(&cur);
        if(cur >= (start + i2c_time_out))
        {
            _I2CUnlock();
            return -1;
        }
    } while(!DL_I2C_getRawInterruptStatus(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

    if (!DL_I2C_isControllerRXFIFOEmpty(I2C_VL53L0X_INST))
    {
        uint8_t c;
        c = DL_I2C_receiveControllerData(I2C_VL53L0X_INST);
        if (i < count)
        {
            pdata[i] = c;
            ++i;
        }
    }

    if(i == count)
        return 0;
    else
        return -1;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    int status_int;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    if (count > sizeof(_I2CBuffer) - 1) {
        return VL53L0X_ERROR_INVALID_PARAMS;
    }
    _I2CBuffer[0] = index;
    memcpy(&_I2CBuffer[1], pdata, count);
    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, count + 1);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    VL53L0X_PutI2cBus();
    return Status;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;
    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, &index, 1);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, pdata, count);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
done:
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index;
    _I2CBuffer[1] = data;

    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index;
    _I2CBuffer[1] = data >> 8;
    _I2CBuffer[2] = data & 0x00FF;

    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 3);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;
    _I2CBuffer[0] = index;
    _I2CBuffer[1] = (data >> 24) & 0xFF;
    _I2CBuffer[2] = (data >> 16) & 0xFF;
    _I2CBuffer[3] = (data >> 8)  & 0xFF;
    _I2CBuffer[4] = (data >> 0 ) & 0xFF;
    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 5);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t data;

    Status = VL53L0X_RdByte(Dev, index, &data);
    if (Status) {
        goto done;
    }
    data = (data & AndData) | OrData;
    Status = VL53L0X_WrByte(Dev, index, data);
done:
    return Status;
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;

    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, &index, 1);
    if( status_int ){
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, data, 1);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }
done:
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;

    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, &index, 1);

    if( status_int ){
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint16_t)_I2CBuffer[0]<<8) + (uint16_t)_I2CBuffer[1];
done:
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;

    VL53L0X_GetI2cBus();
    status_int = _I2CWrite(Dev, &index, 1);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 4);
    if (status_int != 0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint32_t)_I2CBuffer[0]<<24) + ((uint32_t)_I2CBuffer[1]<<16) + ((uint32_t)_I2CBuffer[2]<<8) + (uint32_t)_I2CBuffer[3];

done:
    VL53L0X_PutI2cBus();
    return Status;
}

VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev) {
    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    // do nothing
    VL53L0X_OsDelay();
    return status;
}

//end of file

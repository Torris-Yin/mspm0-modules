#include "ultrasonic_capture.h"

void Ultrasonic_Init(void)
{

}

uint16_t Read_Ultrasonic(void)
{
    return DL_Timer_getCaptureCompareValue(CAPTURE_ULTRASONIC_INST, CAPTURE_ULTRASONIC_CC_IDX) * 0.17;
}
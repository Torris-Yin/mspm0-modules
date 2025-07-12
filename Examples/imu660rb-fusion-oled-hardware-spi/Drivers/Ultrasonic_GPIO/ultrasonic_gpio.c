#include "ultrasonic_gpio.h"
#include "clock.h"

#define ULTRASONIC_TIMEOUT_MS  (10)

void Ultrasonic_Init(void)
{

}

uint16_t Read_Ultrasonic(void)
{
    unsigned long start, cur;
    uint16_t distVal;

    mspm0_get_clock_ms(&start);
    DL_Timer_setTimerCount(TIMER_ULTRASONIC_INST, 0);
    DL_Timer_clearInterruptStatus(TIMER_ULTRASONIC_INST, DL_TIMER_INTERRUPT_LOAD_EVENT);

    DL_GPIO_setPins(GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PORT, GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PIN);
    DL_Common_delayCycles(CPUCLK_FREQ/100000);
    DL_GPIO_clearPins(GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PORT, GPIO_ULTRASONIC_PIN_ULTRASONIC_TRIG_PIN);

    while (!DL_GPIO_readPins(GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PORT, GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PIN))
    {
        mspm0_get_clock_ms(&cur);
        if(cur >= (start + ULTRASONIC_TIMEOUT_MS))
        {
            return 0;
        }
    }

    DL_Timer_startCounter(TIMER_ULTRASONIC_INST);

    while (DL_GPIO_readPins(GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PORT, GPIO_ULTRASONIC_PIN_ULTRASONIC_ECHO_PIN))
    {
        if(DL_Timer_getRawInterruptStatus(TIMER_ULTRASONIC_INST, DL_TIMER_INTERRUPT_LOAD_EVENT))
        {
            DL_Timer_stopCounter(TIMER_ULTRASONIC_INST);
            return 0;
        }
    }

    DL_Timer_stopCounter(TIMER_ULTRASONIC_INST);
    distVal = DL_Timer_getTimerCount(TIMER_ULTRASONIC_INST) * 0.17;
    return distVal;
}
#include "ac_driver_wifi_interrupt.h"

typedef void (*interrupt_function)(void);

static interrupt_function interrupt_function_wifi;

char setInterruptFunc(void *args)
{
    interrupt_function_wifi = (interrupt_function) args;
    return 1;
}
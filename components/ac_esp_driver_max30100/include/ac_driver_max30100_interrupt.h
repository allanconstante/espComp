#pragma once

#include <stdint.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "../../ac_driver_controller/include/ac_driver_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SET_MAX30100_INTERRUPT,
    ENABLE_MAX30100_INTERRUPT,
    DISABLE_MAX30100_INTERRUPT,
    END_MAX30100_INTERRUPT
} ac_max30100_driver_interrupt_functions_list_t;

ac_driver_t* ac_get_max30100_driver_interrupt(void);

#ifdef __cplusplus
}
#endif
#pragma once

#include <stdint.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "ac_driver_max30100_registers.h"
#include "../../ac_driver_controller/include/ac_driver_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MAX30100_GET_RAW_DATA,
    MAX30100_GET_TEMPERATURE,
    MAX30100_START_TEMPERTURA_READING,
    MAX30100_IS_TEMPERATURE_READY,
    END_MAX30100
} ac_max30100_driver_functions_list_t;

ac_driver_t* ac_get_max30100_driver(void);

#ifdef __cplusplus
}
#endif
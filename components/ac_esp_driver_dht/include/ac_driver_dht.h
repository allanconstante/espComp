#pragma once

#include <stdint.h>
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "../../ac_driver_controller/include/ac_driver_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DHT_TEMPERATURE,
    DHT_HUMIDITY,
    DHT_END
} ac_dht_driver_functions_list_t;

ac_driver_t* ac_get_dht_driver(void);

#define GRP TIMER_GROUP_0
#define TIM TIMER_0
#define PIN GPIO_NUM_15

#ifdef __cplusplus
}
#endif
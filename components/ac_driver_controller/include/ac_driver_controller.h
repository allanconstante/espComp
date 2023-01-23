#pragma once

#include <stdint.h>
#include "ac_driver_types.h"
#include "../../ac_esp_driver_dht/include/ac_driver_dht.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXIMUM_NUMBER_DRIVERS 6

typedef enum
{
    DRIVER_DHT,
    DRIVER_END
} ac_driver_list_t;

static ac_get_driver_pointer_t drivers_initialization_vector[DRIVER_END] = 
{
  ac_get_dht_driver
};

char ac_initialize_driver(int new_driver);
char ac_call_driver(int driver_id, int function_id, void *parameters);

#ifdef __cplusplus
}
#endif
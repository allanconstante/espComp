#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "ac_driver_types.h"
// Inserir includes dos drivers ---------------------------------------------------------------------------------------
#include "../../ac_esp_driver_dht/include/ac_driver_dht.h"
#include "../../ac_esp_driver_wifi/include/ac_driver_wifi.h"
// --------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#define MAXIMUM_NUMBER_DRIVERS 6

typedef enum
{
    // Inserir ID dos drivers -----------------------------------------------------------------------------------------
    DRIVER_DHT,
    DRIVER_WIFI,
    // ----------------------------------------------------------------------------------------------------------------
    END
} ac_driver_list_t;

static ac_get_driver_pointer_t drivers_initialization_vector[END] = 
{
  // Inserir função de inicialização dos drivers ----------------------------------------------------------------------
  ac_get_dht_driver,
  ac_get_wifi_driver
  // ------------------------------------------------------------------------------------------------------------------
};

char ac_initialize_driver(int new_driver);
char ac_call_driver(int driver_id, int function_id, void *parameters);

#ifdef __cplusplus
}
#endif
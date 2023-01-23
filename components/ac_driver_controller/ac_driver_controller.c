#include "ac_driver_controller.h"

static ac_driver_t* drivers_loaded[MAXIMUM_NUMBER_DRIVERS];
static int number_drivers_loaded = 0;

char ac_initialize_driver(int new_driver)
{
    char resp = 0;
    if (number_drivers_loaded < MAXIMUM_NUMBER_DRIVERS) {
        drivers_loaded[number_drivers_loaded] = drivers_initialization_vector[new_driver]();
        resp = drivers_loaded[number_drivers_loaded]->driver_initialization((void*)new_driver);
        if (resp == 1) ++number_drivers_loaded;
    }
    return resp;
}

char ac_call_driver(int driver_id, int function_id, void *args)
{
    for (int i = 0; i < number_drivers_loaded; ++i) {
        if (driver_id == drivers_loaded[i]->driver_id) {
            return drivers_loaded[i]->driver_function[function_id](args);
        }
    }
    return 0;
}
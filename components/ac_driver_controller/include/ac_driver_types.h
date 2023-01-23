#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char (*ac_driver_function_pointer_t)(void* arg);
struct ac_driver_t
{
    int driver_id;
    ac_driver_function_pointer_t *driver_function;
    ac_driver_function_pointer_t  driver_initialization;
};
typedef struct ac_driver_t ac_driver_t;
typedef ac_driver_t* (*ac_get_driver_pointer_t)(void);

#ifdef __cplusplus
}
#endif
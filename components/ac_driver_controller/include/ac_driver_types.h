
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char (*ac_driver_function_pointer)(void* arg);
struct ac_driver_t
{
    int drv_id;
    ptrFuncDrv *drv_func;
    ptrFuncDrv  drv_init;
};
typedef struct ac_driver_t ac_driver_t;
typedef ac_driver_t* (*ptrgetDrv)(void);

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------------------------------------------------
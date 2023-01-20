/**
  ******************************************************************************
  * @file           : ctrdrv.c
  * @brief          : Driver controller
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include "ctrdrv.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static acDriver* driversLoaded[QNTD_DRV];
static int qntDrvLoaded;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
char initDriver(char newDriver)
{
    char resp = 0;
    if(qntDrvLoaded < QNTD_DRV)
    {
        driversLoaded[qntDrvLoaded]=drvInitVect[newDriver]();
        resp=driversLoaded[qntDrvLoaded]->drv_init((void*)newDriver);
        if(resp == 1) qntDrvLoaded ++;
    }
    return resp;
}

char callDriver(int drv_id, char func_id, void *parameters)
{
    char i;
    for(i=0; i<qntDrvLoaded; i++)
    {
        if(drv_id == driversLoaded[i]->drv_id)
        {
            return driversLoaded[i]->drv_func[func_id](parameters);
        }
    }
    return 0;
}
/**
  ******************************************************************************
  * @file           : ctrdrv.h
  * @brief          : Driver controller
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "dd_types.h"
#include "br.com.aconstante.dhtDriver.h"
//....

/* Exported types ------------------------------------------------------------*/

/* Exported enums ------------------------------------------------------------*/
enum
{
    DRV_DHT,
    DRV_END
};

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
static ptrgetDrv drvInitVect[DRV_END] = 
{
  getDhtDriver
};

/* Exported functions prototypes ---------------------------------------------*/
char callDriver(int drv_init, char func_id, void *parameters);
char initDriver(char newDriver);

/* Exported Defines ----------------------------------------------------------*/
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
#include "../../br.com.aconstante.dhtDriver/include/br.com.aconstante.dhtDriver.h"
//....

/* Exported Defines ----------------------------------------------------------*/
#define QNTD_DRV 20

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
char initDriver(char newDriver);
char callDriver(int drv_init, char func_id, void *parameters);
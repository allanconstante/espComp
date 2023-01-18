/**
  ******************************************************************************
  * @file           : wifi.h
  * @brief          : Header for wifi.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/* Exported types ------------------------------------------------------------*/
// -------------------------

/* Exported constants --------------------------------------------------------*/
// -------------------------

/* Exported macro ------------------------------------------------------------*/
// -------------------------

/* Exported functions prototypes ---------------------------------------------*/

void wifiStart(void);
void wifiConnect(void);
void wifiDisconnect(void);
wifi_ap_record_t* wifiScan(void);

char wifiStatus(void);

/* Exported Defines ----------------------------------------------------------*/

#define READY           1
#define CONNECTED       2
#define DISCONNECTED    3
#define FAIL            4
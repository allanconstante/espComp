/**
  ******************************************************************************
  * @file           : .h
  * @brief          : .
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
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* Exported types ------------------------------------------------------------*/
// -------------------------

/* Exported constants --------------------------------------------------------*/
// -------------------------

/* Exported macro ------------------------------------------------------------*/
// -------------------------

/* Exported functions prototypes ---------------------------------------------*/
float getTemperature(void);
float getHumidity(void);
void provSensor(timer_group_t, timer_idx_t, gpio_num_t, uint8_t);
void getRawData(uint32_t *, uint8_t *);
void getSensorData(float *, float *);

/* Exported Defines ----------------------------------------------------------*/
#define DHT11 1
#define DHT22 2
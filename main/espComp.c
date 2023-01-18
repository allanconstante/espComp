
#include "br.com.aconstante.dht.h"
#include "br.com.aconstante.wifi.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"

float temp;
float umd;

void initDevices(void);

void app_main(void)
{
    initDevices();
    wifiConnect();
    provSensor(TIMER_GROUP_0, TIMER_0, GPIO_NUM_15, DHT22);

    while (1)
    {      
        vTaskDelay(pdMS_TO_TICKS(5000));
        getSensorData(&temp, &umd);
        printf("Temperatura:    %.2f°C\r\n", temp);
        printf("Umidade:        %.0f%%\r\n", umd);
    }
}

void initDevices(void)
{
     nvs_flash_init();
     wifiStart();
}

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

#include "mqtt_client.h"

float temp;
float umd;

void initDevices(void);
//void mqttStart(void);

/*
static esp_err_t mqttEvent(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int teste;
    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT","MQTT conectado");
            teste = esp_mqtt_client_publish(client, "allanconstante/Feeds/Longhi_Temperatura", "30.00", 0, 1, 0);
            ESP_LOGI("MQTT","ID = %d", teste);
            break;
        
        default:
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI("MQTT", "Base = %s ID = %d", base, event_id);
    mqttEvent(event_data);
}
*/

void app_main(void)
{
    initDevices();
    wifiConnect();
    provSensor(TIMER_GROUP_0, TIMER_0, GPIO_NUM_15, DHT22);
    //vTaskDelay(pdMS_TO_TICKS(10000));
    //mqttStart();

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

/*
void mqttStart(void)
{
    esp_mqtt_client_config_t mqtt_cfg = 
    {
        .host = "io.adafruit.com",
        .port = 1883,
        .username = "teste",
        .password = "teste",

    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client,ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
*/
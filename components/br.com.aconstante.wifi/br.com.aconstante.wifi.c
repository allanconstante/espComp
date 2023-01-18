/**
  ******************************************************************************
  * @file           : wifi.c
  * @brief          : Provisionamento do wifi
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include "br.com.aconstante.wifi.h"

/* Private typedef -----------------------------------------------------------*/
// -------------------------

/* Private define ------------------------------------------------------------*/

#define TAG "Wifi"

#define WIFI_SSID "Allan"           // Nome da rede.
#define WIFI_PASS "12345678"        // Senha do Wifi.
#define WIFI_MAXIMUM_RETRY 10       // Numero de tentativas de conexão.

#define DEFAULT_SCAN_LIST_SIZE 10   //Tamanho da lista do Scan

#define WIFI_READY          BIT0
#define WIFI_CONNECTED      BIT1
#define WIFI_FAIL           BIT2
#define WIFI_SCAN           BIT3

/* Private macro -------------------------------------------------------------*/
// -------------------------

/* Private variables ---------------------------------------------------------*/

EventBits_t bits;
SemaphoreHandle_t connetc_control;

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

/* Private function prototypes -----------------------------------------------*/

void wifiReconnect(void);
static void event_handler(void*, esp_event_base_t, int32_t, void*);

/* Private user code ---------------------------------------------------------*/

void wifiStart(void)
{
    s_wifi_event_group = xEventGroupCreate();
    connetc_control = xSemaphoreCreateMutex();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT(); 

    wifi_config_t config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    esp_wifi_init(&wifi_config);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
                                                        &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                                                        &event_handler, NULL);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &config);

    esp_wifi_start();

}

//Esta função trata os eventos do wifi.
static void event_handler(void* arg, esp_event_base_t event_base, 
                                            int32_t event_id, void* event_data)
{
    if( (event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START) )
    {
        xEventGroupSetBits(s_wifi_event_group, WIFI_READY);
        ESP_LOGI(TAG, "Wifi iniciado.");
    }
    else if( (event_base == WIFI_EVENT) && 
                                    (event_id == WIFI_EVENT_STA_DISCONNECTED) )
    {
        if(xSemaphoreTake(connetc_control, portMAX_DELAY))
        {
            bits = xEventGroupGetBits(s_wifi_event_group);
            if( (bits & WIFI_CONNECTED) )
            {
                wifiReconnect();
            }
            xSemaphoreGive(connetc_control);
        }
    }
    else if( (event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP) )
    {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED);
        xEventGroupClearBits(s_wifi_event_group, WIFI_FAIL);
        ESP_LOGI(TAG, "Conexão bem sucedida");
        s_retry_num = 0;
    }
}

void wifiConnect(void)
{
    if(xSemaphoreTake(connetc_control, portMAX_DELAY))
    {
        esp_wifi_connect();
        xEventGroupSetBits(s_wifi_event_group,  WIFI_CONNECTED);
        xSemaphoreGive(connetc_control);
    }
}

void wifiDisconnect(void)
{
    if(xSemaphoreTake(connetc_control, portMAX_DELAY))
    {
        esp_wifi_disconnect();
        xEventGroupClearBits(s_wifi_event_group,  WIFI_CONNECTED);
        xSemaphoreGive(connetc_control);
    }
}

void wifiReconnect(void)
{
    if(s_retry_num < WIFI_MAXIMUM_RETRY)
    {
        esp_wifi_connect();
        ++s_retry_num;
        ESP_LOGI(TAG, "Tentativa %d", s_retry_num);
    }
    else
    {
        xEventGroupClearBits(s_wifi_event_group,  WIFI_CONNECTED);
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL);
        ESP_LOGE(TAG, "Falha na conexão do Wifi");
    }
}

char wifiStatus(void)
{

    char status = 0;

    bits = xEventGroupGetBits(s_wifi_event_group);
    if( (bits & WIFI_READY) )
    {
        status = READY;
    }
    else if( (bits & WIFI_CONNECTED) )
    {
        status = CONNECTED;
    }
    else if( !(bits & WIFI_CONNECTED) )
    {
        status = DISCONNECTED;
    }
    else if( (bits & WIFI_FAIL) )
    {
        status = FAIL;
    }
    return status;
}

wifi_ap_record_t* wifiScan(void)
{
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    uint16_t ap_count = 0;

    memset(ap_info, 0, sizeof(ap_info));
    
    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_records(&number, ap_info);
    esp_wifi_scan_get_ap_num(&ap_count);

    //ESP_LOGI(TAG, "Total de redes encontradas = %u", ap_count);

    //for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
    //{
        
    //    ESP_LOGI(TAG, "RSSI: %d\tCanal: %d\tSSID: %s", 
    //                                            ap_info[i].rssi,
    //                                            ap_info[i].primary,
    //                                            ap_info[i].ssid);
    //}
    return &ap_info;
}

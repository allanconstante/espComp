#include "ac_driver_wifi.h"

#define TAG "Wifi"

#define WIFI_SSID            CONFIG_WIFI_SSID
#define WIFI_PASS            CONFIG_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY   CONFIG_MAXIMUM_RETRY

#define DEFAULT_SCAN_LIST_SIZE 10   //Tamanho da lista do Scan

#define WIFI_READY          BIT0
#define WIFI_CONNECTED      BIT1
#define WIFI_FAIL           BIT2
#define WIFI_SCAN           BIT3

EventBits_t bits;
SemaphoreHandle_t connetc_control;

static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

static ac_driver_t wifi_driver;
static ac_driver_function_pointer_t wifi_functions[END_WIFI];

static char initialize_wifi_driver(void *parameters);
static char connect_wifi(void *parameters);
static char disconnect_wifi(void *parameters);

static void start_wifi(void);
static void reconnect_wifi(void);
static void event_handler(void*, esp_event_base_t, int32_t, void*);
wifi_ap_record_t* wifiScan(void);

static char connect_wifi(void *parameters)
{
    if (xSemaphoreTake(connetc_control, portMAX_DELAY)) {
        esp_wifi_connect();
        xEventGroupSetBits(s_wifi_event_group,  WIFI_CONNECTED);
        xSemaphoreGive(connetc_control);
    }
    return 1;
}

static char disconnect_wifi(void *parameters)
{
    if (xSemaphoreTake(connetc_control, portMAX_DELAY)) {
        esp_wifi_disconnect();
        xEventGroupClearBits(s_wifi_event_group,  WIFI_CONNECTED);
        xSemaphoreGive(connetc_control);
    }
    return 1;
}

static char initialize_wifi_driver(void *parameters)
{
    start_wifi();
    wifi_driver.driver_id = (int) parameters;
    ESP_LOGI(TAG, "Driver inicializado");
    return 1;
}

ac_driver_t* ac_get_wifi_driver(void)
{
  wifi_driver.driver_initialization = initialize_wifi_driver;
  wifi_functions[CONNECT] = connect_wifi;
  wifi_functions[DISCONNECT] = disconnect_wifi;
  wifi_driver.driver_function = &wifi_functions[0]; //Estudar.
  ESP_LOGI(TAG, "Get driver");
  return &wifi_driver;
}

static void start_wifi(void)
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
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &config);
    esp_wifi_start();
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ( (event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START) ) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_READY);
        ESP_LOGI(TAG, "Wifi iniciado.");
    } else if ( (event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED) ) {
        if (xSemaphoreTake(connetc_control, portMAX_DELAY)) {
            bits = xEventGroupGetBits(s_wifi_event_group);
            if ( (bits & WIFI_CONNECTED) ) reconnect_wifi();
            xSemaphoreGive(connetc_control);
        }
    } else if ( (event_base == IP_EVENT) && (event_id == IP_EVENT_STA_GOT_IP) ) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED);
        xEventGroupClearBits(s_wifi_event_group, WIFI_FAIL);
        ESP_LOGI(TAG, "Conexão bem sucedida");
        s_retry_num = 0;
    }
}

static void reconnect_wifi(void)
{
    if (s_retry_num < WIFI_MAXIMUM_RETRY) {
        esp_wifi_connect();
        ++s_retry_num;
        ESP_LOGI(TAG, "Tentativa %d", s_retry_num);
    } else {
        xEventGroupClearBits(s_wifi_event_group,  WIFI_CONNECTED);
        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL);
        ESP_LOGE(TAG, "Falha na conexão do Wifi");
    }
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

#include "espComp.h"

uint8_t state = 0;
uint8_t teste = 0;

static void funcTeste(void *args);

void app_main(void)
{
    //ac_initialize_driver(DRIVER_DHT);
    //ac_initialize_driver(DRIVER_WIFI);
    //ac_call_driver(DRIVER_WIFI, CONNECT, NULL);
    ac_initialize_driver(DRIVER_MAX30100);

    while (1) {

        if(state == 0){
            ac_call_driver(DRIVER_MAX30100, START_TEMPERTURA_READING, NULL);
            state = 1;
        } else if(state == 1){
            ac_call_driver(DRIVER_MAX30100, IS_TEMPERATURE_READY, &teste);
            //vTaskDelay(pdMS_TO_TICKS(100));
            if(!teste) {
                ac_call_driver(DRIVER_MAX30100, GET_TEMPERATURE_MAX30100, NULL);
                state = 0;
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        }
        /*   
        vTaskDelay(pdMS_TO_TICKS(5000));
        ac_call_driver(DRIVER_DHT, GET_TEMPERATURE, (void*) &temp);
        printf("Temperatura:    %.2f°C\r\n", temp);
        vTaskDelay(pdMS_TO_TICKS(5000));
        ac_call_driver(DRIVER_DHT, GET_HUMIDITY, (void*) &umd);
        printf("Umidade:        %.0f%%\r\n", umd);
        */
    }
}
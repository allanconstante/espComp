#include "espComp.h"

uint8_t teste = 0;
uint8_t cont = 0;

static void funcTeste(void *args);

void app_main(void)
{
    //ac_initialize_driver(DRIVER_DHT);
    //ac_initialize_driver(DRIVER_WIFI);
    //ac_call_driver(DRIVER_WIFI, CONNECT, NULL);
    ac_initialize_driver(DRIVER_MAX30100);
    ac_initialize_driver(DRIVER_MAX30100_INTERRUPT);
    ac_call_driver(DRIVER_MAX30100_INTERRUPT, SET_MAX30100_INTERRUPT, funcTeste);
    ac_call_driver(DRIVER_MAX30100_INTERRUPT, ENABLE_MAX30100_INTERRUPT, NULL);

    while (1)
    {
        if(teste == 0) {
            printf("Nadica de nada até agora.\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            ++cont;
        }
        else {
            teste = 0;
            printf("Feitoria!!! Tu é o cara meu!!!\n");        
        }

        if(cont == 5) {
            ac_call_driver(DRIVER_MAX30100, GET_TEMPERATURE_MAX30100, NULL);
            printf("Eai!!!\n");
            cont = 0;
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

static void funcTeste(void *args)
{
    teste = 1;
}
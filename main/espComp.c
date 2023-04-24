#include "espComp.h"

uint8_t state = 0;
uint8_t teste = 0;
float temperatura = 0;
uint8_t raw[4];
uint8_t fifo_write = 0;
uint8_t fifo_read = 0;
uint8_t over_flow = 0;

void app_main(void)
{
    //ac_initialize_driver(DRIVER_DHT);
    //ac_initialize_driver(DRIVER_WIFI);
    //ac_call_driver(DRIVER_WIFI, CONNECT, NULL);
    ac_initialize_driver(DRIVER_MAX30100);

    while (1) {

        if(state == 0){
            ac_call_driver(DRIVER_MAX30100, MAX30100_START_TEMPERTURA_READING, NULL);
            state = 1;
        } else if(state == 1){
            ac_call_driver(DRIVER_MAX30100, MAX30100_IS_TEMPERATURE_READY, &teste);
            if(!teste) {
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_TEMPERATURE, &temperatura);
                state = 0;
                //printf("%0.2f\n", temperatura);
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_RAW_DATA, raw);
                printf("Data raw: 0x%02x%02x%02x%02x\n", raw[0], raw[1], raw[2], raw[3]);
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_WRITE_POINTER, &fifo_write);
                printf("FIFO Write: 0x%02x\n", fifo_write);
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_READ_POINTER, &fifo_read);
                printf("FIFO Read: 0x%02x\n", fifo_read);
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_OVER_FLOW_COUNTER, &over_flow);
                printf("Over flow: 0x%02x\n", over_flow);
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
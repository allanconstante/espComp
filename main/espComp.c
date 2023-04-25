#include "espComp.h"

uint8_t fifo_write = 0;
uint8_t fifo_read = 0;
uint8_t over_flow = 0;

float temperatura = 0;

int size = 0;

static void plotSensor(void);

void app_main(void)
{
    ac_initialize_driver(DRIVER_MAX30100);
    ac_call_driver(DRIVER_MAX30100, MAX30100_CLEAR, NULL);
    while (1) {
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_WRITE_POINTER, &fifo_write);
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_READ_POINTER, &fifo_read);
        size = (int)fifo_write - (int)fifo_read;
        if ( size > 0 ) plotSensor();
        else if ( size < 0 ) {
            size = 16 - (int)fifo_read;
            plotSensor();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void plotSensor(void)
{
    static float prev_w_ir = 0;
    static float prev_w_red = 0;

    float w = 0;
    float alpha = 0.95;
    float result_ir = 0;
    float result_red = 0;

    uint8_t raw[4];

    uint16_t data_ir[size];
    uint16_t data_red[size];

    for( int i = 0; i < size; ++i ){
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_RAW_DATA, raw);
        data_ir[i] = ( (uint16_t)raw[0]<<8 ) | raw[1];
        data_red[i] = ( (uint16_t)raw[2]<<8 ) | raw[3];
    }
    
    for( int i = 0; i < size; ++i ){
        
        w = (float)data_ir[i] + alpha * prev_w_ir;
        result_ir = w - prev_w_ir;
        prev_w_ir = w;

        w = (float)data_red[i] + alpha * prev_w_red;
        result_red = w - prev_w_red;
        prev_w_red = w;

        printf("%f,%f\n", result_ir, result_red);
    }
}
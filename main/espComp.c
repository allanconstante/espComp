#include "espComp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

xQueueHandle ir_data;
xQueueHandle red_data;

static void readSensor(void *parameters);
static void plot(void *parameters);
static void dataRaw(int size);

void app_main(void)
{
    ir_data = xQueueCreate(20, sizeof(float));
    red_data = xQueueCreate(20, sizeof(float));
    ac_initialize_driver(DRIVER_MAX30100);
    ac_call_driver(DRIVER_MAX30100, MAX30100_CLEAR, NULL);
    xTaskCreate(readSensor, "Loop principal", 2048, NULL, 1, NULL);
    xTaskCreate(plot, "Plot", 2048, NULL, 2, NULL);
}

static void readSensor(void *parameters)
{
    int size = 0;
    uint8_t fifo_write = 0;
    uint8_t fifo_read = 0;
    while ( 1 ) {
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_WRITE_POINTER, &fifo_write);
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_READ_POINTER, &fifo_read);
        size = (int)fifo_write - (int)fifo_read;
        if ( size > 0 ) dataRaw(size);
        else if ( size < 0 ) {
            size = 16 - (int)fifo_read;
            dataRaw(size);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    
}

static void plot(void *parameters)
{
    static float data_ir[5] = { 0, 0, 0, 0, 0 };
    static float data_red[5] = { 0, 0, 0, 0, 0 };
    float ir = 0;
    float red = 0;
    while ( 1 ) {
        ir = 0;
        red = 0;
        for ( int  i = 0; i < 5; ++i ) {
            ir = ir + data_ir[i]/5;
            red = red + data_red[i]/5;
        }
        for ( int  i = 0; i < 5; ++i ) {
            if ( i < 4) {
                data_ir[i] = data_ir[i+1];
                data_red[i] = data_red[i+1];
            } else {
                xQueueReceive( ir_data, data_ir+i, 5000 / portTICK_PERIOD_MS );
                xQueueReceive( red_data, data_red+i, 5000 / portTICK_PERIOD_MS );        
            }
        }
        printf("%f,%f\n", ir, red);
    }
    
}

static void dataRaw(int size)
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

        xQueueSend( ir_data, &result_ir, 1000 / portTICK_PERIOD_MS );
        xQueueSend( red_data, &result_red, 1000 / portTICK_PERIOD_MS );
    }
}
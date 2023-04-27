#include "espComp.h"

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

struct tm data;
time_t tt;

xQueueHandle raw_ir_data;
xQueueHandle raw_red_data;
xQueueHandle ir_data;
xQueueHandle red_data;

static void read_sensor( void *parameters );
static void filter_signal( void *parameters );
static void motus( void *parameters );

static void moving_average( float *buffer, float new_data, float *average );
static void remove_dc_level ( uint16_t data, float *prev, float *fx );
static int get_seconds( void );
static int get_minutes( void );

void app_main(void)
{
    raw_ir_data = xQueueCreate(20, sizeof(uint16_t));
    raw_red_data = xQueueCreate(20, sizeof(uint16_t));

    ir_data = xQueueCreate(20, sizeof(float));
    red_data = xQueueCreate(20, sizeof(float));

    ac_initialize_driver(DRIVER_MAX30100);
    ac_call_driver(DRIVER_MAX30100, MAX30100_CLEAR, NULL);

    xTaskCreate(read_sensor, "Sensor reading", 2048, NULL, 1, NULL);
    xTaskCreate(filter_signal, "Plot", 2048, NULL, 2, NULL);
    xTaskCreate(motus, "Plot", 2048, NULL, 3, NULL);
}

static void read_sensor(void *parameters)
{
    int size = 0;

    uint8_t fifo_write = 0;
    uint8_t fifo_read = 0;
    uint8_t raw_data[4];
    
    uint16_t ir;
    uint16_t red;

    while ( 1 ) {

        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_WRITE_POINTER, &fifo_write);
        ac_call_driver(DRIVER_MAX30100, MAX30100_GET_FIFO_READ_POINTER, &fifo_read);

        size = (int)fifo_write - (int)fifo_read;
        if ( size < 0 ) size = 16 - (int)fifo_read;

        for( int i = 0; i < size; ++i ){
                
                ac_call_driver(DRIVER_MAX30100, MAX30100_GET_RAW_DATA, raw_data);

                ir = ( (uint16_t)raw_data[0]<<8 ) | raw_data[1];
                red = ( (uint16_t)raw_data[2]<<8 ) | raw_data[3];

                xQueueSend( raw_ir_data, &ir, 1000 / portTICK_PERIOD_MS );
                xQueueSend( raw_red_data, &red, 1000 / portTICK_PERIOD_MS );
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void filter_signal( void *parameters )
{
    static float buffer_ir[5] = { 0, 0, 0, 0, 0 };
    static float buffer_red[5] = { 0, 0, 0, 0, 0 };
    static float prev_ir = 0;
    static float prev_red = 0;
    uint16_t ir_raw;
    uint16_t red_raw;
    float ir = 0;
    float red = 0;
    while ( 1 )
    {
        xQueueReceive( raw_ir_data, &ir_raw, 5000 / portTICK_PERIOD_MS );
        xQueueReceive( raw_red_data, &red_raw, 5000 / portTICK_PERIOD_MS );

        remove_dc_level(ir_raw, &prev_ir, &ir);                         // Remove o nivel DC dos dados do LED IR
        remove_dc_level(red_raw, &prev_red, &red);                      // Remove o nivel DC dos dados do LED Red
        moving_average(buffer_ir, ir, &ir);                             // Faz a média movel dos dados do LED IR
        moving_average(buffer_red, red, &red);                          // Faz a média movel dos dados do LED Red

        xQueueSend( ir_data, &ir, 1000 / portTICK_PERIOD_MS );
        xQueueSend( red_data, &red, 1000 / portTICK_PERIOD_MS );
    }
    
}

static void motus ( void *parameters )
{

    static uint8_t state = 0;
    static uint8_t state_cont = 0;
    static uint8_t num_pulso = 0;

    float ir = 0;
    float red = 0;
    float pulso = 0;
    
    int seconds = get_seconds();
    int minutes = get_minutes();

    while ( 1 ) {
        
        xQueueReceive( ir_data, &ir, 5000 / portTICK_PERIOD_MS );
        xQueueReceive( red_data, &red, 5000 / portTICK_PERIOD_MS );
        
        if ( (state_cont == 0) && (ir > 0) ) state_cont = 1;
        else if ( (state_cont == 1) && (ir < 0) ) state_cont = 2;
        else if ( (state_cont == 2) && (ir > 0) ) {
            state_cont = 3;
            pulso = 60;
            ++num_pulso;
        } else if ( state_cont == 3 ) {
            state_cont = 0;
            pulso = 0;
        }

        if ( state == 0 ) {
            seconds = get_seconds();
            num_pulso = 0;
            state = 1;
            printf("State = 1");
        } else if ( (( get_seconds() - seconds ) == 5 ) && ( state == 1 ) ) {
            printf( " %d BPM (%d)\n", ( num_pulso * 12 ), num_pulso );
            state = 0;
        } else if ( (( get_seconds() - seconds ) > 10) && ( state == 1 ) ){
            printf("State = 0");
            state = 0;
        }

        //printf("%f,%f,%f\n", ir, red, pulso);
    }
}

static void remove_dc_level ( uint16_t data, float *prev, float *fx )
{
    float alpha = 0.95;
    float aux = 0;
    aux = (float)data + alpha * (*prev);
    *fx = aux - *prev;
    *prev = aux;
}

static void moving_average ( float *buffer, float new_data, float *average )
{
    float data = 0;
    for ( int  i = 0; i < 5; ++i ) {
        data = data + *(buffer+i)/5;
        if ( i < 4) *(buffer+i) = *(buffer+i+1);
        else *(buffer+i) = new_data;
    }
    *average = data;
}

static int get_seconds( void )
{
    tt = time(NULL);
    data = *gmtime(&tt);
    return data.tm_sec;
}

static int get_minutes( void )
{
    tt = time(NULL);
    data = *gmtime(&tt);
    return data.tm_min;
}